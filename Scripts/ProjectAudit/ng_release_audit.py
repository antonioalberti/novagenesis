#!/usr/bin/env python3
"""Audit NovaGenesis release traceability without changing project state."""
from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path
from typing import Any

SPEC_REF_RE = re.compile(r"\bSPEC-(\d{3})(?:-[A-Za-z0-9][A-Za-z0-9_-]*)?\b")
TASK_REF_RE = re.compile(r"\bNG-(\d{3})\b")
FRONTMATTER_RE = re.compile(r"\A---\n(.*?)\n---\n", re.DOTALL)
RELEASE_BRANCH = "AIOPT3"
REQUIRED_TASK_FIELDS = (
    "id", "titulo", "area", "tipo", "status", "prioridade", "versao",
    "inicio", "responsavel", "stakeholders", "parent", "depends_on",
    "blocked_by", "tags",
)
BLOCKING_CODES = {
    "PLAN_MISSING", "PLAN_NO_GATES", "TASK_MISSING", "TASK_METADATA_DRIFT",
    "TASK_LINK_MISSING", "SPEC_MISSING", "SPEC_OPEN", "SPEC_BRANCH_DRIFT",
    "SPEC_IMPLEMENTATION_MISSING", "SPEC_IMPLEMENTATION_UNKNOWN",
    "SPEC_IMPLEMENTATION_NOT_ANCESTOR", "TASK_SPEC_DRIFT", "WORKTREE_DIRTY",
    "EVIDENCE_MISSING", "GHOST_SPEC_REF", "ACTIVE_AIOPT2_REF", "UNMAPPED_GATE",
}


def _scalar(value: str) -> Any:
    value = value.strip()
    if not value:
        return ""
    if value in {"null", "~"}:
        return ""
    if (value.startswith("\"") and value.endswith("\"")) or (
        value.startswith("'") and value.endswith("'")
    ):
        return value[1:-1]
    if value.startswith("[") and value.endswith("]"):
        inner = value[1:-1].strip()
        return [] if not inner else [_scalar(item) for item in inner.split(",")]
    return value


def parse_frontmatter(text: str) -> dict[str, Any]:
    match = FRONTMATTER_RE.match(text)
    if not match:
        return {}
    result: dict[str, Any] = {}
    current_list: str | None = None
    for raw in match.group(1).splitlines():
        if not raw.strip() or raw.lstrip().startswith("#"):
            continue
        list_item = re.match(r"^\s+-\s+(.+)$", raw)
        if list_item and current_list:
            if not isinstance(result.get(current_list), list):
                result[current_list] = []
            result[current_list].append(_scalar(list_item.group(1)))
            continue
        key_value = re.match(r"^([A-Za-z0-9_-]+):(?:\s*(.*))?$", raw)
        if not key_value:
            continue
        key, value = key_value.group(1), key_value.group(2) or ""
        parsed = _scalar(value)
        result[key] = parsed
        current_list = key if value == "" else None
    return result


def extract_spec_refs(text: str) -> list[str]:
    return sorted({f"SPEC-{number}" for number in SPEC_REF_RE.findall(text)}, key=lambda x: int(x[5:]))


def _refs(pattern: re.Pattern[str], text: str, prefix: str) -> list[str]:
    return sorted({f"{prefix}-{number}" for number in pattern.findall(text)}, key=lambda x: int(x[3:]))


def parse_plan_gates(text: str) -> list[dict[str, Any]]:
    section = re.split(r"(?im)^##(?:\s+\d+\.)?\s+.*\bGates?\b.*$", text, maxsplit=1)
    if len(section) != 2:
        return []
    lines = section[1].splitlines()
    header_index = next((i for i, line in enumerate(lines) if line.strip().startswith("|")), None)
    if header_index is None or header_index + 1 >= len(lines):
        return []
    headers = [cell.strip().lower() for cell in lines[header_index].strip().strip("|").split("|")]
    if not any("gate" == header for header in headers):
        return []
    rows: list[dict[str, Any]] = []
    for line in lines[header_index + 2:]:
        if not line.strip().startswith("|"):
            if rows:
                break
            continue
        cells = [cell.strip() for cell in line.strip().strip("|").split("|")]
        if len(cells) < len(headers):
            cells += [""] * (len(headers) - len(cells))
        row = dict(zip(headers, cells))
        gate = row.get("gate", "")
        if not gate:
            continue
        tasks_text = row.get("tarefas", row.get("tasks", ""))
        specs_text = row.get("specs", row.get("spec", ""))
        state = row.get("estado inicial", row.get("estado", row.get("state", "")))
        # Tables commonly abbreviate later SPECs as ", 052, 053".
        specs_text = re.sub(r"(?<=,)\s*(\d{3})\b", r" SPEC-\1", specs_text)
        rows.append({
            "gate": gate,
            "objective": row.get("objectivo", row.get("objective", "")),
            "tasks": _refs(TASK_REF_RE, tasks_text, "NG"),
            "specs": extract_spec_refs(specs_text),
            "prerequisite": row.get("pré-requisito", row.get("prerequisite", "")),
            "evidence": row.get("evidência", row.get("evidence", "")),
            "state": state,
        })
    return rows


def _git(repo: Path, *args: str) -> tuple[int, str, str]:
    result = subprocess.run(["git", *args], cwd=repo, text=True, capture_output=True)
    return result.returncode, result.stdout.strip(), result.stderr.strip()


def _commit_exists(repo: Path, commit: str) -> bool:
    return bool(commit and commit not in {"—", "-", "<absent>"} and _git(repo, "cat-file", "-e", f"{commit}^{{commit}}") [0] == 0)


def _is_ancestor(repo: Path, commit: str) -> bool:
    return _commit_exists(repo, commit) and _git(repo, "merge-base", "--is-ancestor", commit, "HEAD")[0] == 0


def _spec_files(specs_dir: Path, ref: str) -> list[Path]:
    number = ref[5:]
    return sorted(specs_dir.glob(f"SPEC-{number}-*.md"))


def _task_files(tasks_dir: Path) -> list[Path]:
    return sorted(tasks_dir.glob("NG-*.md")) if tasks_dir.exists() else []


def _task_index(tasks_dir: Path) -> dict[str, Path]:
    index: dict[str, Path] = {}
    for path in _task_files(tasks_dir):
        fields = parse_frontmatter(path.read_text(encoding="utf-8", errors="replace"))
        task_id = fields.get("id") or fields.get("task_id")
        if isinstance(task_id, str) and re.fullmatch(r"NG-\d{3}", task_id):
            index.setdefault(task_id, path)
        else:
            match = re.match(r"(NG-\d{3})-", path.name)
            if match:
                index.setdefault(match.group(1), path)
    return index


def _field_from_spec(text: str, label: str) -> str:
    match = re.search(rf"(?im)^\*\*{re.escape(label)}:\*\*\s*(.*?)\s*$", text)
    if not match:
        match = re.search(rf"(?im)^{re.escape(label)}:\s*(.*?)\s*$", text)
    return match.group(1).strip() if match else ""


def _issue(code: str, message: str, **extra: Any) -> dict[str, Any]:
    item = {"code": code, "message": message}
    item.update(extra)
    return item


def _is_audit_reference_line(line: str) -> bool:
    lowered = line.casefold()
    return any(marker in lowered for marker in (
        "ghost", "dangling", "does not exist", "não existe", "não existem",
        "remover referências", "0 referências", "historical", "histórico",
        "stale", "desactualizad", "grep", "drift conhecido",
    ))


def scan_ghost_spec_refs(specs_dir: Path) -> list[dict[str, Any]]:
    issues: list[dict[str, Any]] = []
    for path in specs_dir.glob("*.md"):
        if path.name.startswith("PLAN-CONSOLIDATION-"):
            continue
        for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
            if _is_audit_reference_line(line):
                continue
            for spec_ref in extract_spec_refs(line):
                if not _spec_files(specs_dir, spec_ref):
                    issues.append(_issue("GHOST_SPEC_REF", f"Referência {spec_ref} sem SPEC correspondente", spec=spec_ref, path=str(path)))
    return issues


def _active_aio_pt2_refs(repo: Path) -> list[str]:
    matches: list[str] = []
    for path in (repo / "Specs").glob("*.md"):
        if "HISTORICAL" in str(path).upper() or path.name.startswith("PLAN-CONSOLIDATION-"):
            continue
        lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
        if any("AIOPT2" in line and not _is_audit_reference_line(line) for line in lines):
            matches.append(str(path.relative_to(repo)))
    return matches


def audit_release(repo: Path, plan_path: Path, vault: Path | None) -> dict[str, Any]:
    repo = repo.resolve()
    plan_path = plan_path.resolve()
    issues: list[dict[str, Any]] = []
    gates: list[dict[str, Any]] = []
    plan_text = ""
    if not plan_path.exists():
        issues.append(_issue("PLAN_MISSING", f"Plano não encontrado: {plan_path}"))
    else:
        plan_text = plan_path.read_text(encoding="utf-8", errors="replace")
        gates = parse_plan_gates(plan_text)
        if not gates:
            issues.append(_issue("PLAN_NO_GATES", "Plano não contém uma tabela de gates reconhecível"))

    status_rc, status_text, _ = _git(repo, "status", "--porcelain")
    branch_rc, branch, _ = _git(repo, "branch", "--show-current")
    head_rc, head, _ = _git(repo, "rev-parse", "HEAD")
    worktree = {
        "dirty": bool(status_text),
        "paths": status_text.splitlines() if status_text else [],
        "branch": branch if branch_rc == 0 else "",
        "head": head if head_rc == 0 else "",
    }
    if worktree["dirty"]:
        issues.append(_issue("WORKTREE_DIRTY", "Árvore Git contém alterações não congeladas", paths=worktree["paths"]))
    if branch and branch != RELEASE_BRANCH:
        issues.append(_issue("SPEC_BRANCH_DRIFT", f"Branch candidata inesperada: {branch}"))

    tasks_dir: Path | None = None
    task_index: dict[str, Path] = {}
    if vault:
        vault = vault.resolve()
        tasks_dir = vault / "Tarefas" if (vault / "Tarefas").exists() else vault
        task_index = _task_index(tasks_dir)

    specs_dir = repo / "Specs"
    seen_specs: set[str] = set()
    for gate in gates:
        if not gate["tasks"] and not gate["specs"]:
            issues.append(_issue("UNMAPPED_GATE", f"{gate['gate']} não possui tarefa nem SPEC"))
        for task_id in gate["tasks"]:
            task_path = task_index.get(task_id)
            if vault and not task_path:
                issues.append(_issue("TASK_MISSING", f"{task_id} não encontrado no vault", task=task_id, gate=gate["gate"]))
                continue
            if not task_path:
                continue
            fields = parse_frontmatter(task_path.read_text(encoding="utf-8", errors="replace"))
            missing = [field for field in REQUIRED_TASK_FIELDS if field not in fields]
            if missing:
                issues.append(_issue("TASK_METADATA_DRIFT", f"{task_id} não segue o frontmatter canónico", task=task_id, missing=missing, path=str(task_path)))
            linked_specs = extract_spec_refs(task_path.read_text(encoding="utf-8", errors="replace"))
            if fields.get("status") in {"concluida", "cancelada"} and len(gate["tasks"]) == 1 and len(gate["specs"]) == 1:
                for spec_ref in gate["specs"]:
                    if spec_ref not in linked_specs:
                        issues.append(_issue("TASK_SPEC_DRIFT", f"{task_id} não referencia {spec_ref}", task=task_id, spec=spec_ref))
        for spec_ref in gate["specs"]:
            if spec_ref in seen_specs:
                continue
            seen_specs.add(spec_ref)
            files = _spec_files(specs_dir, spec_ref)
            if not files:
                issues.append(_issue("SPEC_MISSING", f"{spec_ref} não tem ficheiro correspondente", spec=spec_ref, gate=gate["gate"]))
                continue
            for spec_path in files:
                text = spec_path.read_text(encoding="utf-8", errors="replace")
                status = _field_from_spec(text, "Status")
                spec_branch = _field_from_spec(text, "Branch")
                implementation = _field_from_spec(text, "Implementation commit")
                open_criteria = len(re.findall(r"^\s*- \[ \]", text, re.MULTILINE))
                implemented_status = status.startswith("Implemented")
                if not implemented_status:
                    issues.append(_issue("SPEC_OPEN", f"{spec_path.name} está {status or 'sem status'}", spec=spec_ref, path=str(spec_path), status=status, open_criteria=open_criteria))
                if spec_branch and spec_branch != RELEASE_BRANCH:
                    issues.append(_issue("SPEC_BRANCH_DRIFT", f"{spec_path.name} aponta para {spec_branch}", spec=spec_ref, path=str(spec_path)))
                if implemented_status and not implementation:
                    issues.append(_issue("SPEC_IMPLEMENTATION_MISSING", f"{spec_path.name} não regista commit de implementação", spec=spec_ref, path=str(spec_path)))
                elif implementation:
                    commits = re.findall(r"[0-9a-fA-F]{7,40}", implementation)
                    if commits and not any(_commit_exists(repo, commit) for commit in commits):
                        issues.append(_issue("SPEC_IMPLEMENTATION_UNKNOWN", f"{spec_path.name} referencia commit inexistente", spec=spec_ref, implementation=implementation, path=str(spec_path)))
                    elif implemented_status and commits and not any(_is_ancestor(repo, commit) for commit in commits):
                        issues.append(_issue("SPEC_IMPLEMENTATION_NOT_ANCESTOR", f"{spec_path.name} não está demonstravelmente no candidato", spec=spec_ref, implementation=implementation, path=str(spec_path)))

    if vault:
        dashboard = tasks_dir / "Dashboard.md"
        if dashboard.exists():
            dashboard_text = dashboard.read_text(encoding="utf-8", errors="replace")
            for task_ref in sorted(set(TASK_REF_RE.findall(dashboard_text)), key=int):
                task_id = f"NG-{task_ref}"
                if task_id in {task for gate in gates for task in gate["tasks"]} and task_id not in task_index:
                    issues.append(_issue("TASK_LINK_MISSING", f"Dashboard referencia {task_id}, mas a nota não foi resolvida", task=task_id))

    issues.extend(scan_ghost_spec_refs(specs_dir))
    active_aio = _active_aio_pt2_refs(repo)
    if active_aio:
        issues.append(_issue("ACTIVE_AIOPT2_REF", "Referências AIOPT2 permanecem em documentos não históricos", paths=active_aio))

    # Evidence paths are checked only when an explicit Evidence column exists.
    for gate in gates:
        evidence = gate.get("evidence", "")
        for raw in re.findall(r"`([^`]+)`", evidence):
            evidence_path = repo / raw if not raw.startswith("/") else Path(raw)
            if not evidence_path.exists():
                issues.append(_issue("EVIDENCE_MISSING", f"Evidência ausente: {raw}", gate=gate["gate"], evidence=raw))

    blocking = [item for item in issues if item["code"] in BLOCKING_CODES]
    result = {
        "schema_version": 1,
        "read_only": True,
        "release": "v1.0.0",
        "branch": worktree["branch"],
        "head": worktree["head"],
        "worktree": worktree,
        "plan": str(plan_path),
        "vault": str(vault) if vault else None,
        "gates": gates,
        "issues": sorted(issues, key=lambda item: (item["code"], item.get("gate", ""), item.get("path", ""), item["message"])),
        "summary": {
            "gate_count": len(gates),
            "issue_count": len(issues),
            "blocking_issue_count": len(blocking),
        },
        "verdict": "PASS" if not blocking and gates else "BLOCKED",
    }
    return result


def render_human(result: dict[str, Any]) -> str:
    lines = [
        f"Release: {result['release']}",
        f"Veredicto: {result['verdict']}",
        f"Branch/HEAD: {result['branch'] or '<unknown>'} / {result['head'] or '<unknown>'}",
        f"Árvore dirty: {'sim' if result['worktree']['dirty'] else 'não'}",
        f"Gates: {result['summary']['gate_count']}; problemas: {result['summary']['issue_count']} "
        f"({result['summary']['blocking_issue_count']} bloqueantes)",
    ]
    for item in result["issues"]:
        lines.append(f"- [{item['code']}] {item['message']}")
    return "\n".join(lines)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repo", type=Path, default=Path.cwd())
    parser.add_argument("--plan", type=Path, default=Path("Specs/RELEASE-MASTER-PLAN-v1.0.0.md"))
    parser.add_argument("--vault", type=Path)
    parser.add_argument("--json", action="store_true", dest="as_json")
    args = parser.parse_args(argv)
    plan = args.plan if args.plan.is_absolute() else args.repo / args.plan
    result = audit_release(args.repo, plan, args.vault)
    print(json.dumps(result, ensure_ascii=False, indent=2, sort_keys=True) if args.as_json else render_human(result))
    return 0 if result["verdict"] == "PASS" else 2


if __name__ == "__main__":
    sys.exit(main())
