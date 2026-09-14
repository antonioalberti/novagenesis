import json
import subprocess
import sys
from pathlib import Path

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from ng_release_audit import (
    audit_release,
    extract_spec_refs,
    parse_frontmatter,
    parse_plan_gates,
    scan_ghost_spec_refs,
)


def test_parse_frontmatter_supports_canonical_task_fields():
    text = """---\nid: NG-050\ntitulo: Plano mestre\nstatus: em-andamento\nprioridade: alta\ntags:\n  - tarefa\n  - release\n---\n# Tarefa\n"""
    fields = parse_frontmatter(text)
    assert fields["id"] == "NG-050"
    assert fields["status"] == "em-andamento"
    assert fields["tags"] == ["tarefa", "release"]


def test_parse_plan_gates_extracts_gate_links_and_state():
    text = """## 4. Gate register\n\n| Gate | Objectivo | Tarefas | SPECs | Estado inicial |\n|---|---|---|---|---|\n| G1 | Hygiene | NG-038 | SPEC-038 | OPEN |\n| G2 | Runner | NG-049 | SPEC-046, 052, 053 | parcial |\n"""
    gates = parse_plan_gates(text)
    assert gates[0]["gate"] == "G1"
    assert gates[0]["tasks"] == ["NG-038"]
    assert gates[1]["specs"] == ["SPEC-046", "SPEC-052", "SPEC-053"]
    assert gates[1]["state"] == "parcial"


def test_extract_spec_refs_deduplicates_full_spec_names():
    text = "SPEC-044, SPEC-044-core-run-evaluate-invalid-pg-downcast.md and SPEC-045"
    assert extract_spec_refs(text) == ["SPEC-044", "SPEC-045"]


def test_audit_detects_missing_task_metadata_and_open_spec(tmp_path):
    repo = tmp_path / "repo"
    vault = tmp_path / "vault" / "Tarefas"
    specs = repo / "Specs"
    specs.mkdir(parents=True)
    vault.mkdir(parents=True)
    (repo / ".git").mkdir()
    subprocess.run(["git", "init", "-q"], cwd=repo, check=True)
    (repo / "Specs" / "RELEASE-MASTER-PLAN-v1.0.0.md").write_text(
        """# Plan\n\n## Gates\n\n| Gate | Objectivo | Tarefas | SPECs | Pré-requisito | Estado inicial |\n|---|---|---|---|---|---|\n| G1 | Correctness | NG-050 | SPEC-054 | — | OPEN |\n""",
        encoding="utf-8",
    )
    (specs / "SPEC-054-release-master-plan-and-audit.md").write_text(
        """# SPEC-054\n\n**Status:** In Progress\n**Branch:** AIOPT3\n**Implementation commit:** —\n\n- [ ] criterion\n""",
        encoding="utf-8",
    )
    (vault / "NG-050-release-master-control-v1-20260913.md").write_text(
        """---\ntask_id: NG-050\nstatus: em-andamento\n---\n# legacy\n""",
        encoding="utf-8",
    )
    result = audit_release(repo, specs / "RELEASE-MASTER-PLAN-v1.0.0.md", vault.parent)
    codes = {item["code"] for item in result["issues"]}
    assert result["verdict"] == "BLOCKED"
    assert "TASK_METADATA_DRIFT" in codes
    assert "SPEC_OPEN" in codes


def test_audit_reports_clean_readonly_status_when_no_vault_is_given(tmp_path):
    repo = tmp_path / "repo"
    specs = repo / "Specs"
    specs.mkdir(parents=True)
    subprocess.run(["git", "init", "-q"], cwd=repo, check=True)
    plan = specs / "RELEASE-MASTER-PLAN-v1.0.0.md"
    plan.write_text("# Plan\n\n## Gates\n\n| Gate | Objectivo | Tarefas | SPECs | Estado |\n|---|---|---|---|---|\n", encoding="utf-8")
    result = audit_release(repo, plan, None)
    assert result["read_only"] is True
    assert result["worktree"]["dirty"] is True
    json.dumps(result)


def test_reference_scan_ignores_audit_mentions_but_detects_real_reference(tmp_path):
    specs = tmp_path / "Specs"
    specs.mkdir()
    (specs / "PLAN-CONSOLIDATION-2026-07-16.md").write_text(
        "# Historical audit\nSPEC-015 does not exist; remove dangling references.\n",
        encoding="utf-8",
    )
    (specs / "SPEC-100-real.md").write_text(
        "# SPEC-100\n**Related:** SPEC-016\n",
        encoding="utf-8",
    )
    issues = scan_ghost_spec_refs(specs)
    assert [item["spec"] for item in issues] == ["SPEC-016"]
