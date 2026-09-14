from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
RUN_SCRIPTS = sorted(ROOT.glob("run_*.sh"))


def test_all_run_launchers_propagate_child_exit_status_from_gdb():
    assert len(RUN_SCRIPTS) == 5
    for path in RUN_SCRIPTS:
        text = path.read_text()
        assert "gdb -batch" in text
        assert "-return-child-result" in text, path.name


def test_all_run_launchers_verify_a_iopt3_build_provenance_before_start():
    assert len(RUN_SCRIPTS) == 5
    for path in RUN_SCRIPTS:
        text = path.read_text()
        assert "AIOPT3" in text, path.name
        assert ".ng-build-receipt" in text, path.name
        assert "git symbolic-ref" in text, path.name
        assert "git rev-parse HEAD" in text, path.name


def test_build_script_pins_one_commit_for_both_guests():
    text = (ROOT / "pull-and-build-vms.sh").read_text()
    assert "git ls-remote" in text
    assert "EXPECTED_COMMIT" in text
    assert "git merge --ff-only" in text
    assert 'test "$(git rev-parse HEAD)" = "$EXPECTED_COMMIT"' in text


def test_build_script_requires_explicit_ssh_and_durable_evidence():
    text = (ROOT / "pull-and-build-vms.sh").read_text()
    assert ': "${NG_SSH_KEY:?' in text
    assert ': "${NG_SSH_USER:?' in text
    assert ': "${NG_EVIDENCE_PATH:?' in text
    assert "NG_EVIDENCE_PATH" in text
    assert "trap" in text
    assert "kill \"$PID101\"" in text
    assert "kill \"$PID102\"" in text
    assert "wait -n" in text
