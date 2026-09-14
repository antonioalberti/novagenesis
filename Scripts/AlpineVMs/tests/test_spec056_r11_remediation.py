"""SPEC-056 R11 fail-closed publication and rollback regressions."""
from __future__ import annotations

import json
import os
import signal
import subprocess
import sys
import time
from pathlib import Path
from unittest.mock import patch

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import local_provenance
import ng_remote_executor as executor


def test_successful_secret_rewrite_is_a_publication_blocker(tmp_path: Path):
    path = tmp_path / "roles" / "stdout.log"
    path.parent.mkdir()
    path.write_text('SOURCE_TOKEN="r11-secret"\n', encoding="utf-8")

    report = local_provenance.sanitize_evidence_tree(tmp_path, {"r11-secret"})

    assert report["ok"] is False
    assert report["rewritten"] == ["roles/stdout.log"]
    assert any("rewritten" in item for item in report["blockers"])
    assert "r11-secret" not in path.read_text(encoding="utf-8")


def test_secret_filename_dotenv_url_and_argv_are_detected(tmp_path: Path):
    dotenv = tmp_path / ".env"
    dotenv.write_text("ordinary-looking but protected", encoding="utf-8")
    log = tmp_path / "roles" / "stdout.log"
    log.parent.mkdir()
    log.write_text(
        'curl https://user:r11-url-secret@example.invalid/?token=r11-query-secret\n'
        '["--password", "r11-argv-secret"]\n',
        encoding="utf-8",
    )

    report = local_provenance.sanitize_evidence_tree(
        tmp_path,
        {"r11-url-secret", "r11-query-secret", "r11-argv-secret"},
    )

    assert report["ok"] is False
    assert any("filename" in item for item in report["blockers"])
    assert any("rewritten" in item for item in report["blockers"])
    assert not dotenv.exists()
    assert "r11-url-secret" not in log.read_text(encoding="utf-8")
    assert "r11-query-secret" not in log.read_text(encoding="utf-8")
    assert "r11-argv-secret" not in log.read_text(encoding="utf-8")


def test_execution_inputs_report_protected_blockers_before_sanitisation():
    blockers = local_provenance._execution_input_blockers(
        {
            "argv": ["--password", "r11-argv-secret"],
            "url": "https://user:r11-url-secret@example.invalid/?token=r11-query-secret",
            "dotenv": "/staging/.env",
        }
    )

    assert any("argv secret" in item for item in blockers)
    assert any("execution text" in item for item in blockers)
    assert any("filename" in item for item in blockers)


def test_publication_boundary_does_not_seal_protected_bundle(tmp_path: Path):
    result = tmp_path / "result.json"
    result.write_text(
        json.dumps(
            {
                "schema_version": 2,
                "runtime_result": "PASS",
                "teardown_result": "PASS",
                "evidence_result": "COMPLETE",
                "local_acceptance_eligible": True,
                "acceptance_blockers": [],
                "exit_code": 0,
            }
        ),
        encoding="utf-8",
    )
    (tmp_path / "roles").mkdir()
    (tmp_path / "roles" / "stdout.log").write_text('TOKEN="r11-publish-secret"\n', encoding="utf-8")

    with pytest.raises(OSError, match="evidence publication aborted"):
        executor.write_evidence_manifest(tmp_path, {"mode": "local", "trial_id": "r11"})

    assert not (tmp_path / "manifest.json").exists()
    assert not (tmp_path / "terminal-seal.json").exists()


def test_options_boolean_id_and_synthetic_recipe_runtime_are_rejected():
    options = {"id": False, "variant": "normal", "jobs": 1, "configure_only": False}
    with pytest.raises(ValueError, match="options.*id"):
        local_provenance._validate_build_options(options)


def test_descendant_reconciliation_repeats_scans_and_keeps_ledger():
    tracked = {}
    late = {"pid": 222, "ppid": 1, "pgid": 999, "starttime": "late", "comm": "child", "executable": "/bin/child"}
    scans = iter(
        [
            {"descendants": [], "complete": True},
            {"descendants": [late], "complete": True},
            {"descendants": [], "complete": True},
        ]
    )
    with (
        patch.object(executor, "local_process_descendants", side_effect=lambda _pid: next(scans)),
        patch.object(executor, "local_stop_tracked_descendants", return_value=(True, [])),
    ):
        report = executor.local_descendant_reconciliation(123, tracked, 123, scans=3)

    assert report["ok"] is True
    assert report["scan_count"] == 3
    assert report["residual"] == []
    assert report["unknown"] == []
    assert tracked[222] == late


def test_actual_reparented_descendant_is_drained_after_leader_exit():
    code = "import os, time; child = os.fork();\nif child: time.sleep(0.15)\nelse: os.setsid(); time.sleep(10)"
    leader = subprocess.Popen([sys.executable, "-c", code], start_new_session=True)
    tracked: dict[int, dict[str, object]] = {}
    try:
        time.sleep(0.04)
        observed = executor.local_process_descendants(leader.pid)
        tracked.update({item["pid"]: item for item in observed["descendants"]})
        assert tracked
        assert leader.wait(timeout=3) == 0
        report = executor.local_descendant_reconciliation(leader.pid, tracked, leader.pid, scans=3)
        assert report["ok"] is True
        assert report["residual"] == []
        assert report["unknown"] == []
    finally:
        for pid in list(tracked):
            try:
                os.kill(pid, signal.SIGKILL)
            except ProcessLookupError:
                pass


def test_descendant_reconciliation_marks_scan_uncertainty_non_success():
    with patch.object(
        executor,
        "local_process_descendants",
        return_value={"descendants": [], "complete": False, "error": "late scan denied"},
    ), patch.object(executor, "local_stop_tracked_descendants", return_value=(True, [])):
        report = executor.local_descendant_reconciliation(123, {}, 123, scans=2)

    assert report["ok"] is False
    assert report["unknown"]
    assert "late scan denied" in report["unknown"][0]["error"]
