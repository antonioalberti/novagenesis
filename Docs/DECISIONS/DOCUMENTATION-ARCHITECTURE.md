# NovaGenesis Documentation Architecture

**Version:** 1.0  
**Date:** 2026-07-16  
**Scope:** Repository-wide documentation organization for `/home/gandalf/workspace/novagenesis`

---

## 1. Top-Level Structure

```
/home/gandalf/workspace/novagenesis/
├── Specs/              # Specifications (SPECs) — design, implementation, fixes
├── Docs/               # Reference documentation — architecture, diagnostics, decisions
├── Issues/             # Issue tracking — open/closed issues with evidence
├── Scripts/            # Operational scripts (Alpine VMs, build, deploy)
├── IO/                 # Runtime I/O directories (gitignored)
├── Common/             # Shared C++ code (Process, Block, GW, Message, etc.)
├── PGCS/               # PGCS component
├── NRNCS/              # NRNCS component
├── ContentApp/         # ContentApp component
├── HTS/                # HTS component
├── ...                 # Embedded/IoT components are maintained externally
└── ... (other components)
```

---

## 2. Specs/ — Specifications

### Purpose
Living design documents for changes. Each SPEC describes a problem, proposed solution, implementation plan, and verification criteria.

### Naming Convention
```
SPEC-<NNN>-<kebab-case-short-description>.md
SPEC-<NNN>-<component>-<kebab-case>.md           # Component-specific
SPEC-<PREFIX>-<NNN>-<kebab-case>.md              # Special prefixes (MUSL, etc.)
```

### Status Values (in frontmatter)
| Status | Meaning |
|--------|---------|
| `Draft` | Initial sketch, not validated |
| `Proposal` | Design complete, awaiting approval/implementation |
| `In Progress` | Implementation started |
| `Implemented` | Code in HEAD, compiles, deployed |
| `Implemented — Pending Test` | Code in HEAD, E2E test pending |
| `Superseded` | Replaced by newer SPEC (reference given) |
| `Abandoned` | Never implemented, no replacement |

### Frontmatter Template
```markdown
# SPEC-<NNN>: <Title>

**Version:** v1.0  
**Date:** YYYY-MM-DD  
**Author:** <Name>  
**Status:** <Status>  
**Branch:** AIOPT3  
**Implementation commit:** <hash> (if applicable)  
**Related:** SPEC-XXX, SPEC-YYY
```

### Lifecycle
1. Create as `Draft` → 2. Refine to `Proposal` → 3. Implement (`In Progress`) → 4. Mark `Implemented` with commit hash → 5. If replaced, mark old `Superseded` with reference to new SPEC

### Cross-References
- Use **full filename** for SPEC-to-SPEC links: `SPEC-022-nrinfopayload01-separate-messages.md`
- Never reference non-existent SPECs (SPEC-015, SPEC-016 were dangling refs — removed)

---

## 3. Docs/ — Reference Documentation

### Structure
```
Docs/
├── ARCHITECTURE/       # System architecture, layer models, data flows
├── DIAGNOSTICS/        # Root cause analyses, investigation reports
├── DECISIONS/          # Decision registers, status trackers, revert logs
└── HISTORICAL/         # Obsolete docs kept for traceability
```

### Subdirectory Purposes

| Directory | Content | Example |
|-----------|---------|---------|
| `ARCHITECTURE/` | Stable architectural models, layer definitions, protocol formats | `NGAL-ARCHITECTURE.md`, `NG-INVERTED-PUB-SUB-MODEL.md` |
| `DIAGNOSTICS/` | Post-mortem analyses, root cause investigations | `NGAL-HASH-MISMATCH-ROOT-CAUSE.md`, `PGCS-RELAY-CONGESTION-BURST-SPLIT-2026-07-12.md` |
| `DECISIONS/` | Decision logs, status registers, audit trails | `SPEC-STATUS-REGISTER.md`, `REVERT.md` |
| `HISTORICAL/` | Superseded/obsolete docs moved here (never deleted) | — |

### Naming Convention
- Architecture: `UPPER-KEBAB-CASE.md` (e.g., `NGAL-ARCHITECTURE.md`)
- Diagnostics: `UPPER-KEBAB-CASE-YYYY-MM-DD.md` (date suffix for traceability)
- Decisions: `UPPER-KEBAB-CASE.md` or `STATUS-REGISTER.md`

### Source Extraction Rule
Docs in `ARCHITECTURE/` and `DIAGNOSTICS/` are **extracted from SPECs** (not duplicated). The SPEC remains the source of truth; the Doc is a curated, stable reference.

---

## 4. Issues/ — Issue Tracking

### Structure
```
Issues/
├── OPEN/       # Active issues under investigation
├── CLOSED/     # Resolved issues with resolution summary
└── TEMPLATE.md # Standard issue template
```

### Naming Convention
```
ISSUE-<NNN>-<kebab-case-short-description>.md
```

### Frontmatter
```markdown
# ISSUE-<NNN>: <Title>

**Date:** YYYY-MM-DD  
**Status:** Open / In Progress / Closed  
**Priority:** High / Medium / Low  
**Related:** SPEC-XXX, Component, File paths
```

### Required Sections
1. **Description** — Clear problem statement
2. **Evidence** — Logs, metrics, test results
3. **Root Cause Analysis** — If known
4. **Proposed Fix** — Or "TBD"
5. **Files Affected** — Table with change types
6. **Verification Plan** — How to confirm fix
7. **Resolution** — When closed: what was done, commit ref, test results

### Lifecycle
`Open` → `In Progress` → `Closed` (moved to `CLOSED/`)

### Closed Issues
- Kept permanently in `CLOSED/` for audit trail
- Include resolution summary and commit reference
- Cross-referenced from `DECISIONS/SPEC-STATUS-REGISTER.md`

---

## 5. Cross-Linking Rules

| From | To | Format |
|------|----|--------|
| SPEC → SPEC | Full filename | `SPEC-022-nrinfopayload01-separate-messages.md` |
| SPEC → Doc | Relative path | `../Docs/ARCHITECTURE/NGAL-ARCHITECTURE.md` |
| SPEC → Issue | Relative path | `../Issues/OPEN/ISSUE-001-...` |
| Doc → SPEC | Relative path | `../../Specs/SPEC-013-ngal-adaptation-layer.md` |
| Issue → SPEC | Relative path | `../../Specs/SPEC-022-...` |
| Issue → Doc | Relative path | `../../Docs/DIAGNOSTICS/...` |

**Never use absolute paths.** All links must be relative to survive repo moves.

---

## 6. Language & Style

- **All documentation in English** (UK spelling preferred)
- **Status values in English**: Draft, Proposal, In Progress, Implemented, Superseded, Abandoned
- **Dates**: ISO 8601 (YYYY-MM-DD)
- **Branches**: Always `AIOPT3` (current) — no `AIOPT2` references
- **Commits**: Short hash (7 chars) or full hash

---

## 7. Maintenance Responsibilities

| Role | Responsibility |
|------|----------------|
| SPEC author | Keep SPEC status current; mark Superseded when replaced |
| Doc extractor | Create Doc from SPEC when SPEC reaches `Implemented` |
| Issue owner | Update Issue status; move to `CLOSED/` on resolution |
| Reviewer | Verify cross-links valid; check no dangling SPEC-015/016 refs |

---

## 8. Validation Checklist (run before commit)

- [ ] No `AIOPT2` references anywhere
- [ ] No `SPEC-015` or `SPEC-016` references
- [ ] All SPECs have valid status from allowed list
- [ ] All cross-links use relative paths and resolve
- [ ] New SPECs have frontmatter with Version, Date, Author, Status, Branch
- [ ] New Issues have frontmatter with Date, Status, Priority, Related
- [ ] Docs in `ARCHITECTURE/` and `DIAGNOSTICS/` have source SPEC noted
- [ ] Closed issues moved to `CLOSED/` with resolution

---

## 9. Tooling (Future)

Planned scripts:
- `Scripts/Simple/validate-specs.sh` — checks SPEC frontmatter, status, cross-refs
- `Scripts/Simple/validate-docs.sh` — checks Doc structure, links
- `Scripts/Simple/validate-issues.sh` — checks Issue structure, OPEN/CLOSED placement

---

*This document defines the documentation architecture for NovaGenesis. Update when structure evolves.*