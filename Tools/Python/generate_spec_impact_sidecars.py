#!/usr/bin/env python3
"""Generate provenance-aware affected-file sidecars from pinned NovaGenesis Git objects.

This is a documentary inventory, not proof that every listed path was changed by a SPEC.
"""
from __future__ import annotations
import argparse
import re
import subprocess
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BRANCHES = {
    'AIOPT3': '24a3851e1d5a90f039b9e148a1af38dd03c4d925',
    'AIOPT4': '0dd0dd4b2fd0a9d416aff232ad07672fe39746b6',
    'AIOPT5': '8d2386dcb10d2b37f133d4b24c2cb96761753d82',
}
SPEC_ID = re.compile(r'^(SPEC-\d{3}|SPEC-MUSL-\d{3})')
PATH_EXT = re.compile(r'(?:(?:[A-Za-z0-9_.-]+/)+)?[A-Za-z0-9_.-]+\.(?:cpp|cxx|hpp|hxx|bash|service|yaml|yml|toml|json|xml|conf|cmake|ini|txt|md|sh|py|cc|hh|h|c|mk|sse|raw|log|csv|png|jpg|jpeg|dat|pdf|svg|sha256|bin|so)(?![A-Za-z0-9])', re.I)
CODE = re.compile(r'`([^`]+)`')


def git(*args: str) -> str:
    return subprocess.check_output(['git', *args], cwd=ROOT, text=True, stderr=subprocess.PIPE).strip()


def tree_paths(branch: str) -> list[str]:
    return [p for p in git('ls-tree', '-r', '--name-only', BRANCHES[branch], '--', 'Specs').splitlines()
            if p.startswith('Specs/SPEC-') and p.endswith('.md')]


def get_text(branch: str, path: str) -> str:
    return git('show', f'{BRANCHES[branch]}:{path}')


def meta(text: str, name: str) -> str:
    m = re.search(rf'^\*\*{re.escape(name)}:\*\*\s*(.*?)\s*$', text, re.M | re.I)
    return m.group(1).strip() if m else ''


def title(text: str) -> str:
    return next((line.lstrip('# ').strip() for line in text.splitlines() if line.startswith('# SPEC-')), '(title not found)')


def applicability(snapshot_branch: str, declared_branch: str, status: str, filename: str) -> str:
    status_l = status.lower()
    if snapshot_branch == 'AIOPT4':
        return 'Archived snapshot; no current AIOPT4 authority'
    if filename == 'SPEC-031-recvmmsg-batched-receive.md':
        return 'Implemented in source; SPEC Draft/current recvfrom baseline stale; no benchmark acceptance implied'
    if snapshot_branch == 'AIOPT5' and filename == 'SPEC-027-pgcs-multithreading.md':
        return 'AIOPT5-linked; phases OPEN/PARTIAL and work paused; no execution authorization'
    if snapshot_branch == 'AIOPT3' and declared_branch == 'AIOPT3':
        return 'AIOPT3-origin; historical line; status/gates apply to this snapshot only'
    if declared_branch and declared_branch != '(none)' and declared_branch != snapshot_branch:
        return f'{declared_branch}-origin copy; not authority for {snapshot_branch}'
    if declared_branch == snapshot_branch:
        return f'{snapshot_branch}-scoped; honor declared status and gates only'
    if 'supersed' in status_l:
        return 'Superseded per header; preserve as historical record'
    if 'deferred' in status_l or 'abandoned' in status_l:
        return 'Deferred/abandoned per header; not active authority'
    if 'draft' in status_l or 'proposal' in status_l:
        return 'Draft/proposal; not implementation authority; branch unspecified'
    return 'Indeterminate: no Branch header establishes applicability'


def scope_paths(text: str) -> list[tuple[int, str, str]]:
    """Collect literal path strings only from explicit file/scope sections or tables."""
    lines = text.splitlines()
    explicit_depth = None
    paths: dict[tuple[int, str], str] = {}
    section_re = re.compile(r'\b(files?|file paths?|affected files?|files? affected|files? to (?:change|modify|update)|scope|allowlist|ficheiros?|arquivos?)\b', re.I)
    for i, line in enumerate(lines):
        heading = re.match(r'^(#{1,6})\s+(.+)', line)
        if heading:
            depth = len(heading.group(1))
            if explicit_depth is not None and depth <= explicit_depth:
                explicit_depth = None
            if section_re.search(heading.group(2)):
                explicit_depth = depth
        in_explicit_section = explicit_depth is not None
        is_table = '|' in line
        table_header = is_table and bool(re.search(r'\b(file|path|affected|change|scope|ficheiro|arquivo)\b', line, re.I))
        if not (in_explicit_section or table_header):
            continue
        candidates = []
        code_cells = CODE.findall(line)
        for cell in code_cells:
            value = cell.strip()
            if value.startswith('.') and '/' not in value:
                continue
            if PATH_EXT.fullmatch(value.replace('*', 'x')):
                candidates.append(value)
        # Plain table text is a fallback only when the row has no code-formatted paths.
        if is_table and not code_cells:
            for match in PATH_EXT.finditer(line):
                val = match.group(0).strip('`.,;()[]')
                if val not in candidates:
                    candidates.append(val)
        for value in candidates:
            paths[(i + 1, value)] = line.strip()
    return [(ln, path, raw) for (ln, path), raw in sorted(paths.items())]


def commits_for(branch: str) -> list[tuple[str, str, str, list[str]]]:
    raw = git('log', '--format=%H%x1f%s%x1f%b%x1e', BRANCHES[branch])
    commits = []
    for record in raw.split('\x1e'):
        fields = record.strip('\n\r').split('\x1f', 2)
        if len(fields) != 3 or not re.fullmatch(r'[0-9a-f]{40}', fields[0]):
            continue
        sha, subject, body = fields
        files = git('diff-tree', '--no-commit-id', '--name-only', '-r', sha).splitlines()
        commits.append((sha, subject, body, files))
    return commits


def exact_token_for(filename: str) -> str:
    stem = Path(filename).stem
    m = SPEC_ID.match(stem)
    return m.group(1) if m else stem


def md_cell(value: str) -> str:
    return value.replace('|', '\\|').replace('\n', ' ')


def build_records() -> dict[str, str]:
    paths = {b: tree_paths(b) for b in BRANCHES}
    all_names = {Path(p).name for group in paths.values() for p in group}
    local_specs = {p.name: p for p in (ROOT / 'Specs').glob('SPEC-*.md') if p.name not in all_names}
    all_names = sorted(all_names | set(local_specs))
    specs_by_name = {name: {b: next((p for p in paths[b] if Path(p).name == name), None) for b in BRANCHES}
                     for name in all_names}
    commit_cache = {b: commits_for(b) for b in BRANCHES}
    # Identify duplicate numeric IDs; only exact sub-ID references can be attributed to a variant.
    id_to_names: dict[str, list[str]] = defaultdict(list)
    for name in all_names:
        m = SPEC_ID.match(Path(name).stem)
        if m:
            id_to_names[m.group(1)].append(name)

    records = {}
    for name, by_branch in specs_by_name.items():
        exact_id = exact_token_for(name)
        duplicate = len(id_to_names[exact_id]) > 1
        sections: dict[str, list[tuple[int, str, str]]] = {}
        info = {}
        commit_evidence: dict[str, list[tuple[str, str, list[str], str]]] = {}
        ambiguous: dict[str, list[tuple[str, str]]] = {}
        for branch, path in by_branch.items():
            if not path:
                continue
            text = get_text(branch, path)
            info[branch] = {
                'path': path,
                'title': title(text),
                'branch': meta(text, 'Branch') or '(none)',
                'status': meta(text, 'Status') or '(not declared in header)',
                'version': meta(text, 'Version') or '(not declared)',
                'baseline': meta(text, 'Baseline commit') or '(not declared)',
                'implementation': meta(text, 'Implementation commit') or '(not declared)',
            }
            sections[branch] = scope_paths(text)
            evidence, uncertain = [], []
            for sha, subject, body, files in commit_cache[branch]:
                blob = (subject + '\n' + body)
                basis = []
                if path in files:
                    basis.append('the SPEC file changed in this commit')
                # Full numeric ID is unambiguous only when the ID is unique in the inventory.
                if duplicate:
                    variant_token = Path(name).stem
                    matches = bool(re.search(r'(?<![A-Za-z0-9])' + re.escape(variant_token) + r'(?![A-Za-z0-9])', blob, re.I))
                    generic = bool(re.search(r'(?<![A-Za-z0-9])' + re.escape(exact_id) + r'(?![A-Za-z0-9])', blob, re.I))
                    if matches:
                        basis.append('commit subject/body names this exact SPEC variant')
                    elif generic and not basis:
                        uncertain.append((sha, subject))
                elif re.search(r'(?<![A-Za-z0-9])' + re.escape(exact_id) + r'(?![0-9])', blob, re.I):
                    basis.append('commit subject/body names this SPEC identifier')
                if basis:
                    evidence.append((sha, subject, files, '; '.join(basis)))
            commit_evidence[branch] = evidence
            ambiguous[branch] = uncertain

        local_info = None
        if name in local_specs:
            local_path = local_specs[name]
            local_text = local_path.read_text(encoding='utf-8', errors='replace')
            local_rel = local_path.relative_to(ROOT).as_posix()
            local_state = git('status', '--short', '--', local_rel) or '(clean/tracked)'
            local_info = {
                'path': local_rel,
                'title': title(local_text),
                'branch': meta(local_text, 'Branch') or '(none)',
                'status': meta(local_text, 'Status') or '(not declared in header)',
                'baseline': meta(local_text, 'Baseline commit') or '(not declared)',
                'implementation': meta(local_text, 'Implementation commit') or '(not declared)',
                'git_state': local_state,
                'sha256': __import__('hashlib').sha256(local_path.read_bytes()).hexdigest(),
            }
            sections['Local working tree'] = scope_paths(local_text)

        out = [f'# Affected-file record: `{name}`', '', f'SPEC title: {next(iter(info.values()))["title"] if info else (local_info["title"] if local_info else "(not present in pinned trees)")}',
               '', '## Snapshot coverage and applicability', '',
               '| Snapshot | Pinned HEAD | Present | Declared Branch | Version | Status | Baseline | Implementation commit | Applicability |',
               '|---|---|---|---|---|---|---|---|---|']
        for branch, sha in BRANCHES.items():
            d = info.get(branch)
            declared = md_cell(d['branch']) if d else '—'
            version = md_cell(d['version']) if d else '—'
            status = md_cell(d['status']) if d else '—'
            baseline = md_cell(d['baseline']) if d else '—'
            implementation = md_cell(d['implementation']) if d else '—'
            scope = md_cell(applicability(branch, d['branch'], d['status'], name)) if d else '—'
            out.append(f'| `{branch}` | `{sha}` | {"yes" if d else "no"} | {declared} | {version} | {status} | {baseline} | {implementation} | {scope} |')
        if local_info:
            local_scope = 'Local-only working copy; no branch authority'
            out.append(f'| `Local working tree` | SHA-256 `{local_info["sha256"]}` | yes | {md_cell(local_info["branch"])} | (working copy) | {md_cell(local_info["status"])} | {md_cell(local_info["baseline"])} | {md_cell(local_info["implementation"])} | {local_scope} |')
            out += ['', f'Local-only source: `{local_info["path"]}`; Git state: `{local_info["git_state"]}`. This content is not part of the pinned branch snapshots.']
        out += ['', '## Files named in explicit file/scope sections', '',
                'These are literal file/path strings found in file-, scope-, or allowlist-related headings/tables. They represent declared or proposed scope, not proof of a change.']
        any_paths = False
        for branch in BRANCHES:
            if branch not in sections: continue
            entries = sections[branch]
            out += ['', f'### `{branch}` — source `{info[branch]["path"]}`']
            if not entries:
                out.append('- No literal file paths were detected in the recognized scope/file sections.')
            else:
                any_paths = True
                for line_no, path, _raw in entries:
                    kind = 'pattern' if '*' in path else 'path'
                    out.append(f'- {kind}: `{path}` — source SPEC line {line_no}.')
        if local_info:
            entries = sections['Local working tree']
            out += ['', f'### `Local working tree` — source `{local_info["path"]}`']
            if entries:
                any_paths = True
                out.extend(f'- {"pattern" if "*" in path else "path"}: `{path}` — source SPEC line {line_no}.'
                           for line_no, path, _raw in entries)
            else:
                out.append('- No literal file paths were detected in the recognized scope/file sections.')
        if not any_paths:
            out += ['', 'No explicit file paths found in the recognized sections for this SPEC. This is an inventory gap, not evidence that no files were affected.']
        out += ['', '## Files changed in linked Git commits', '',
                'Paths below come from commits that either changed this SPEC file itself or explicitly named its identifier in the subject/body. This is evidence of co-change or a declared commit link, not proof that every path was caused solely by this SPEC.']
        any_commit_files = False
        for branch in BRANCHES:
            if branch not in info: continue
            out += ['', f'### `{branch}`']
            entries = commit_evidence[branch]
            if not entries:
                out.append('- No commit with an unambiguous matching SPEC identifier was found in this branch history.')
            for sha, subject, files, basis in entries:
                out.append(f'- Commit `{sha}` — {subject} (link basis: {basis})')
                if files:
                    any_commit_files = True
                    out.extend(f'  - `{f}`' for f in files)
                else:
                    out.append('  - (no file paths reported by the commit diff)')
            unresolved = ambiguous[branch]
            if unresolved:
                out.append('- Ambiguous generic-ID commits were not assigned to this SPEC variant:')
                out.extend(f'  - `{sha}` — {subject}' for sha, subject in unresolved)
        if not any_commit_files:
            out += ['', 'No changed path was established from an unambiguous SPEC-linked commit in these snapshots.']
        out += ['', '## Interpretation and limits', '',
                '- A declared path, a path present in the tree, a source citation, and a path changed in a SPEC-linked commit are different evidence types; this record keeps declared scope separate from observed commit diffs.',
                '- `git ls-tree`/`git show` at the pinned HEAD can establish presence and exact contents, but not that a SPEC caused a change or that its acceptance criteria passed.',
                '- Commit-message linkage is incomplete by nature. Absence of a linked commit does not prove that no files were affected; ambiguous duplicate SPEC IDs are left unattributed.',
                '- Branch-specific absence means the SPEC file is not in that pinned branch tree; it does not imply the branch never contained it historically.',
                '', 'Generated from pinned Git objects; do not treat this sidecar as implementation authorization or acceptance evidence.', '']
        records[name] = '\n'.join(out)
    return records


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument('--write', action='store_true', help='write sidecars into Specs/IMPACT')
    parser.add_argument('--ref', action='append', default=[], metavar='NAME=REV',
                        help='override a frozen branch input (repeatable), e.g. --ref AIOPT5=AIOPT5')
    args = parser.parse_args()
    for item in args.ref:
        if '=' not in item:
            raise SystemExit(f'FAIL: --ref must be NAME=REV, got {item!r}')
        name, ref = item.split('=', 1)
        if not name or not ref:
            raise SystemExit(f'FAIL: --ref must be NAME=REV, got {item!r}')
        BRANCHES[name] = git('rev-parse', ref)
    for branch, expected in BRANCHES.items():
        actual = git('rev-parse', expected)
        if actual != expected:
            raise SystemExit(f'FAIL: {branch} pin is not an exact commit: {expected}')
    records = build_records()
    target = ROOT / 'Specs' / 'IMPACT'
    if args.write:
        target.mkdir(parents=True, exist_ok=True)
        writes, unchanged = [], 0
        for name, content in records.items():
            dest = target / f'{name}.affected-files.md'
            if not dest.exists():
                writes.append((dest, content))
                continue
            old = dest.read_text(encoding='utf-8')
            if old == content:
                unchanged += 1
                continue
            same_pins = all(f'| `{branch}` | `{sha}` |' in old for branch, sha in BRANCHES.items())
            if 'Generated from pinned Git objects' not in old or not same_pins:
                raise SystemExit(f'FAIL: refusing to replace non-generated or differently pinned sidecar {dest}')
            writes.append((dest, content))
        for dest, content in writes:
            dest.write_text(content, encoding='utf-8')
        print(f'WROTE/REFRESHED {len(writes)} sidecars; unchanged={unchanged}; target={target}')
    else:
        print(f'DRY RUN: {len(records)} sidecars; target={target}; no files written')
        for name in list(records)[:3]:
            print(f'--- {name} ---')
            print(records[name][:1600])
            print('...')

if __name__ == '__main__':
    main()
