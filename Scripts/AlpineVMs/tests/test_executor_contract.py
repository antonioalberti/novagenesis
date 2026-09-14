import json
import os
import shutil
import subprocess
import sys
import tempfile
import time
import unittest
from pathlib import Path
from argparse import Namespace
from types import SimpleNamespace
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import ng_remote_executor as executor
from ng_remote_executor import (
    ConfigError,
    classify_result,
    expand_argv,
    load_config_from_env,
    validate_plan,
    canonical_scenario,
    load_local_config,
    run_local_trial,
    validate_trial_id,
    local_file_oracle,
    latch_oracle_result,
    marker_requirements_satisfied,
    marker_runtime_result,
    preserve_runtime_failure,
    preserve_local_artifacts,
    log_quota_exceeded,
    local_acceptance_eligibility,
    local_group_members,
    local_group_members_status,
    attribute_new_ipc,
    local_remove_new_ipc,
    local_ipc_ids,
    local_ipc_details,
    local_process_descendants,
    local_stop_tracked_descendants,
    local_stop_process_group,
    process_starttime,
)


class ExecutorContractTests(unittest.TestCase):
    def test_missing_required_environment_is_rejected(self):
        with tempfile.TemporaryDirectory() as td:
            old = dict(os.environ)
            try:
                for key in [
                    "NG_SSH_KEY", "NG_SSH_USER", "SOURCE_VM_IP", "REPO_VM_IP",
                    "SOURCE_VM_MAC", "REPO_VM_MAC", "SOURCE_VM_IFACE", "REPO_VM_IFACE", "NG_REPO_PATH", "NG_BUILD_PATH",
                    "NG_EVIDENCE_PATH", "NG_REMOTE_EVIDENCE_PATH",
                    "NG_SSH_KNOWN_HOSTS",
                ]:
                    os.environ.pop(key, None)
                with self.assertRaises(ConfigError):
                    load_config_from_env()
            finally:
                os.environ.clear()
                os.environ.update(old)

    def test_argv_expansion_preserves_argument_boundaries(self):
        result = expand_argv(["tool", "--label", "${TRIAL_ID}", "literal value"], {"TRIAL_ID": "abc-123"})
        self.assertEqual(result, ["tool", "--label", "abc-123", "literal value"])

    def test_plan_rejects_shell_command_string(self):
        plan = {"schema_version": 1, "roles": [{"name": "x", "command": "echo unsafe"}]}
        with self.assertRaises(ValueError):
            validate_plan(plan)

    def test_plan_rejects_unimplemented_file_oracle(self):
        plan = {"schema_version": 1, "roles": [{"name": "x", "command": ["/bin/true"]}], "runtime_oracle": {"type": "files", "required": []}}
        with self.assertRaises(ValueError):
            validate_plan(plan)

    def test_complete_environment_is_accepted(self):
        env = {
            "NG_SSH_KEY": "/tmp/key", "NG_SSH_USER": "root", "SOURCE_VM_IP": "source", "REPO_VM_IP": "repo",
            "SOURCE_VM_MAC": "08:00:27:65:00:08", "REPO_VM_MAC": "08:00:27:79:bb:15", "SOURCE_VM_IFACE": "eth0", "REPO_VM_IFACE": "eth0",
            "NG_REPO_PATH": "/opt/ng", "NG_BUILD_PATH": "/opt/ng/build-final", "NG_EVIDENCE_PATH": "/var/evidence", "NG_REMOTE_EVIDENCE_PATH": "/var/ng", "NG_SSH_KNOWN_HOSTS": "/tmp/known_hosts",
        }
        result = load_config_from_env(env)
        self.assertEqual(result["NG_BUILD_PATH"], "/opt/ng/build-final")

    def test_guest_preflight_generated_program_is_valid_python(self):
        config = {
            "SOURCE_VM_IP": "source",
            "REPO_VM_IP": "repo",
            "SOURCE_VM_IFACE": "eth0",
            "REPO_VM_IFACE": "eth0",
            "SOURCE_VM_MAC": "08:00:27:65:00:08",
            "REPO_VM_MAC": "08:00:27:79:bb:15",
            "NG_REPO_PATH": "/opt/ng",
            "NG_REMOTE_EVIDENCE_PATH": "/var/ng",
        }
        roles = [{"name": "PGCS-Source", "argv": ["/opt/ng/build/PGCS"], "cwd": "/opt/ng/build"}]
        response = subprocess.CompletedProcess([], 0, '{"ok": true}\n', "")
        with patch.object(executor, "remote_shell", return_value=response) as remote:
            executor.guest_preflight(config, "source", roles)
        generated = remote.call_args.args[2][2]
        compile(generated, "<guest-preflight>", "exec")

    def test_local_mode_has_a_distinct_explicit_configuration(self):
        root = Path(tempfile.mkdtemp(prefix="ng-elc-local-", dir=Path.home()))
        try:
            env = {
                "NG_LOCAL_REPO_PATH": str(root),
                "NG_LOCAL_BUILD_PATH": str(root / "build"),
                "NG_LOCAL_IO_PATH": str(root / "io"),
                "NG_LOCAL_EVIDENCE_PATH": str(root / "evidence"),
            }
            result = load_local_config(env)
            self.assertEqual(result["NG_LOCAL_IO_PATH"], str(root / "io"))
        finally:
            shutil.rmtree(root, ignore_errors=True)

    def test_local_plan_accepts_local_roles_and_file_oracle(self):
        plan = {
            "schema_version": 1,
            "roles": [{
                "name": "Source", "vm": "local", "command": ["/bin/true"],
                "readiness": [{"id": "ready", "pattern": "READY"}],
            }],
            "timeouts": {"readiness": 1, "observation": 1, "total": 3},
            "runtime_oracle": {"type": "files", "source": "/a", "repository": "/b", "pattern": "*.jpg", "expected_count": 1, "hash": "sha256"},
        }
        validate_plan(plan, mode="local")

    def test_local_scenario_name_is_canonical(self):
        self.assertEqual(canonical_scenario("local-intra-os"), "LOCAL")

    def test_local_trial_uses_same_bounded_result_contract(self):
        root = Path(tempfile.mkdtemp(prefix="ng-elc-local-trial-", dir=Path.home()))
        try:
            plan_path = root / "plan.json"
            code = "import time; print('READY', flush=True); time.sleep(0.4)"
            roles = [{"name": name, "vm": "local", "command": ["python3", "-c", code], "cwd": str(root), "readiness": [{"id": "ready", "pattern": "READY"}]} for name in ["PGCS", "NRNCS", "Repository", "Source"]]
            plan_path.write_text(json.dumps({
                "schema_version": 1, "roles": roles,
                "timeouts": {"readiness": 2, "observation": 0.1, "total": 5},
                "diagnostic_only": True,
            }), encoding="utf-8")
            env = {
                "NG_LOCAL_REPO_PATH": str(root), "NG_LOCAL_BUILD_PATH": str(root),
                "NG_LOCAL_IO_PATH": str(root / "io"), "NG_LOCAL_EVIDENCE_PATH": str(root / "evidence"),
            }
            args = Namespace(plan=str(plan_path), scenario="local-intra-os", debug_profile="obs-normal", trial="test-local", env=env)
            rc = run_local_trial(args)
            result = json.loads((root / "evidence" / "test-local" / "result.json").read_text())
            self.assertEqual(rc, 11)
            self.assertEqual(result["runtime_result"], "INCONCLUSIVE")
            self.assertEqual(result["teardown_result"], "PASS")
            self.assertEqual(result["evidence_result"], "COMPLETE")
            provenance = json.loads((root / "evidence" / "test-local" / "provenance.json").read_text())
            self.assertEqual(provenance["mode"], "local")
            self.assertEqual(set(provenance["binaries"]), {"PGCS", "NRNCS", "Repository", "Source"})
        finally:
            shutil.rmtree(root, ignore_errors=True)

    def test_teardown_failure_takes_precedence(self):
        self.assertEqual(classify_result("PASS", "FAIL", "COMPLETE"), 20)
        self.assertEqual(classify_result("PASS", "PASS", "COMPLETE"), 0)
        self.assertEqual(classify_result("INCONCLUSIVE", "PASS", "COMPLETE"), 11)
    def test_trial_id_is_one_safe_path_component(self):
        for trial_id in ["ok-trial_1.2", "A0"]:
            self.assertEqual(validate_trial_id(trial_id), trial_id)
        for trial_id in ["", ".", "..", "../escape", "/absolute", "nested/trial", "nested\\\\trial"]:
            with self.subTest(trial_id=trial_id):
                with self.assertRaises(ConfigError):
                    validate_trial_id(trial_id)
    def test_file_oracle_preserves_reproducible_hash_maps(self):
        root = Path(tempfile.mkdtemp(prefix="ng-elc-oracle-", dir=Path.home()))
        try:
            source = root / "source"
            repository = root / "repository"
            source.mkdir()
            repository.mkdir()
            for name, data in [("a.jpg", b"A"), ("b.jpg", b"BB")]:
                (source / name).write_bytes(data)
                (repository / name).write_bytes(data)
            result = local_file_oracle({
                "source": str(source), "repository": str(repository),
                "pattern": "*.jpg", "expected_count": 2, "hash": "sha256",
            }, {})
            self.assertEqual(result["result"], "PASS")
            self.assertEqual(result["source_map"], result["repository_map"])
            self.assertEqual(result["source_map"]["a.jpg"]["size"], 1)
            self.assertEqual(len(result["source_map"]["a.jpg"]["sha256"]), 64)
        finally:
            shutil.rmtree(root, ignore_errors=True)
    def test_oracle_failure_is_sticky(self):
        self.assertEqual(latch_oracle_result("INCONCLUSIVE", "FAIL"), "FAIL")
        self.assertEqual(latch_oracle_result("FAIL", "PASS"), "FAIL")
        self.assertEqual(latch_oracle_result("INCONCLUSIVE", "PASS"), "PASS")

    def test_marker_finalization_preserves_process_failure(self):
        self.assertEqual(marker_runtime_result("FAIL", {("PGCS", "done")}, [{"role": "PGCS", "id": "done"}]), "FAIL")
        self.assertEqual(marker_runtime_result("INCONCLUSIVE", {("PGCS", "done")}, [{"role": "PGCS", "id": "done"}]), "PASS")

        required = [
            {"role": "PGCS", "id": "ready", "pattern": "READY"},
            {"role": "NRNCS", "id": "ready", "pattern": "READY"},
        ]
        self.assertFalse(marker_requirements_satisfied({("PGCS", "ready")}, required))
        self.assertFalse(marker_requirements_satisfied({("PGCS", "ready"), ("PGCS", "ready")}, required))
        self.assertTrue(marker_requirements_satisfied({("PGCS", "ready"), ("NRNCS", "ready")}, required))

    def test_runtime_exception_preserves_prior_failure(self):
        self.assertEqual(preserve_runtime_failure("FAIL"), "FAIL")
        self.assertEqual(preserve_runtime_failure("PASS"), "INCONCLUSIVE")
    def test_preserve_local_artifacts_creates_independent_rehashable_copies(self):
        root = Path(tempfile.mkdtemp(prefix="ng-elc-preserve-", dir=Path.home()))
        try:
            source = root / "source"
            repository = root / "repository"
            evidence = root / "evidence"
            source.mkdir()
            repository.mkdir()
            (source / "photo.jpg").write_bytes(b"payload")
            (repository / "photo.jpg").write_bytes(b"payload")
            result = preserve_local_artifacts({
                "source": str(source), "repository": str(repository),
                "pattern": "*.jpg", "expected_count": 1, "hash": "sha256",
            }, {}, evidence)
            self.assertTrue(result["preservation_verified"])
            self.assertTrue((evidence / "artifacts" / "source" / "photo.jpg").is_file())
            self.assertTrue((evidence / "artifacts" / "repository" / "photo.jpg").is_file())
            self.assertEqual(result["source_map"], result["preserved_source_map"])
            self.assertEqual(result["repository_map"], result["preserved_repository_map"])
        finally:
            shutil.rmtree(root, ignore_errors=True)
    def test_local_trial_seals_oracle_and_artifact_copies(self):
        root = Path(tempfile.mkdtemp(prefix="ng-elc-bundle-", dir=Path.home()))
        try:
            build = root / "build"
            io = root / "io"
            source = io / "Source1"
            repository = io / "Repository1"
            evidence = root / "evidence"
            build.mkdir()
            build_manifest = root / "build-manifest.json"
            build_manifest.write_text("{}", encoding="utf-8")
            source.mkdir(parents=True)
            repository.mkdir(parents=True)
            (source / "a.jpg").write_bytes(b"payload")
            (repository / "a.jpg").write_bytes(b"payload")
            plan_path = root / "plan.json"
            code = "import time; print('READY', flush=True); time.sleep(5)"
            roles = [{"name": name, "vm": "local", "command": ["python3", "-c", code], "cwd": str(build), "readiness": [{"id": "ready", "pattern": "READY"}]} for name in ["PGCS", "NRNCS", "Repository", "Source"]]
            plan_path.write_text(json.dumps({
                "schema_version": 1, "roles": roles,
                "timeouts": {"readiness": 2, "observation": 1, "total": 5},
                "runtime_oracle": {"type": "files", "source": str(source), "repository": str(repository), "pattern": "*.jpg", "expected_count": 1, "hash": "sha256"},
            }), encoding="utf-8")
            env = {"NG_LOCAL_REPO_PATH": str(root), "NG_LOCAL_BUILD_PATH": str(build), "NG_LOCAL_IO_PATH": str(io), "NG_LOCAL_EVIDENCE_PATH": str(evidence), "NG_LOCAL_BUILD_MANIFEST": str(build_manifest)}
            args = Namespace(plan=str(plan_path), scenario="local-intra-os", debug_profile="obs-normal", trial="bundle-trial", env=env)
            self.assertEqual(run_local_trial(args), 11)
            trial = evidence / "bundle-trial"
            sealed_result = json.loads((trial / "result.json").read_text())
            self.assertFalse(sealed_result["local_acceptance_eligible"])
            self.assertIn("dirty source tree", sealed_result["acceptance_blockers"])
            oracle = json.loads((trial / "oracle.json").read_text())
            self.assertTrue(oracle["preservation_verified"])
            self.assertEqual(oracle["source_map"], oracle["preserved_source_map"])
            self.assertTrue((trial / "artifacts" / "source" / "a.jpg").is_file())
            self.assertTrue((trial / "artifacts" / "repository" / "a.jpg").is_file())
        finally:
            shutil.rmtree(root, ignore_errors=True)
    def test_log_quota_is_checked_before_reading(self):
        root = Path(tempfile.mkdtemp(prefix="ng-elc-log-", dir=Path.home()))
        try:
            small = root / "small.log"
            large = root / "large.log"
            small.write_text("ok", encoding="utf-8")
            large.write_text("0123456789", encoding="utf-8")
            self.assertFalse(log_quota_exceeded([small], 2))
            self.assertTrue(log_quota_exceeded([large], 5))
        finally:
            shutil.rmtree(root, ignore_errors=True)
    def test_acceptance_eligibility_requires_complete_provenance(self):
        blocked = local_acceptance_eligibility({
            "git_clean": False,
            "build_linkage": False,
            "controller_identity": False,
            "plan_snapshot": False,
        })
        self.assertFalse(blocked["eligible"])
        self.assertIn("dirty source tree", blocked["blockers"])
        accepted = local_acceptance_eligibility({
            "git_clean": True,
            "build_linkage": True,
            "controller_identity": True,
            "plan_snapshot": True,
        })
        self.assertTrue(accepted["eligible"])
        self.assertEqual(accepted["blockers"], [])
    def test_proc_inventory_enumeration_failure_is_incomplete(self):
        with patch("ng_remote_executor.Path.iterdir", side_effect=PermissionError("denied")):
            group = local_group_members_status(1234)
            descendants = local_process_descendants(1234)
        self.assertFalse(group["complete"])
        self.assertFalse(descendants["complete"])

    def test_process_group_members_reports_current_process(self):
        pid = os.getpid()
        pgid = os.getpgid(pid)
        self.assertIn(pid, {item["pid"] for item in local_group_members(pgid)})
    def test_process_group_members_catches_surviving_same_group_child(self):
        code = "import os, time; child = os.fork(); os._exit(0) if child else time.sleep(5)"
        leader = subprocess.Popen([sys.executable, "-c", code], start_new_session=True)
        pgid = leader.pid
        try:
            self.assertEqual(leader.wait(timeout=3), 0)
            members = []
            deadline = time.monotonic() + 2
            while time.monotonic() < deadline:
                members = local_group_members(pgid)
                if members:
                    break
                time.sleep(0.05)
            self.assertTrue(members)
            self.assertTrue(all(item["pgid"] == pgid for item in members))
            self.assertTrue(any(item["pid"] != leader.pid for item in members))
        finally:
            try:
                os.killpg(pgid, 9)
            except ProcessLookupError:
                pass

    def test_descendant_kill_error_is_not_success(self):
        tracked = {1234: {"pid": 1234, "pgid": 77, "starttime": "42"}}
        record = {"status": "ok", "state": "S", "pgid": 77, "starttime": "42"}
        clock = iter([0, 3])
        with patch("ng_remote_executor.read_proc_stat", side_effect=[record, record, record]), patch("ng_remote_executor.time.monotonic", side_effect=lambda: next(clock)), patch("ng_remote_executor.os.kill", side_effect=[None, PermissionError("denied")]) as kill:
            stopped, residual = local_stop_tracked_descendants(tracked)
        self.assertFalse(stopped)
        self.assertEqual(residual[0]["reason"], "executable-identity-unavailable")
        self.assertFalse(kill.called)

    def test_exited_leader_with_surviving_group_member_is_not_success(self):
        code = "import os, time; child = os.fork(); os._exit(0) if child else time.sleep(10)"
        leader = subprocess.Popen([sys.executable, "-c", code], start_new_session=True)
        pgid = leader.pid
        expected_starttime = process_starttime(leader.pid)
        try:
            self.assertEqual(leader.wait(timeout=3), 0)
            result = local_stop_process_group(leader, pgid, expected_starttime)
            self.assertFalse(result["ok"])
            self.assertEqual(result["result"], "group-member-residual")
            self.assertTrue(result["residual"])
            self.assertTrue(any(item["pid"] != leader.pid for item in result["residual"]))
        finally:
            try:
                os.killpg(pgid, 9)
            except ProcessLookupError:
                pass

    def test_detached_tracked_descendant_retains_pgid_identity_after_leader_exit(self):
        code = "import os, time; child = os.fork();\nif child: time.sleep(0.2)\nelse: os.setsid(); time.sleep(10)"
        leader = subprocess.Popen([sys.executable, "-c", code], start_new_session=True)
        pgid = leader.pid
        try:
            time.sleep(0.05)
            scan = local_process_descendants(leader.pid)
            tracked = {item["pid"]: item for item in scan["descendants"]}
            self.assertTrue(tracked)
            self.assertTrue(all(item.get("pgid") for item in tracked.values()))
            self.assertEqual(leader.wait(timeout=3), 0)
            stopped, residual = local_stop_tracked_descendants(tracked)
            self.assertTrue(stopped)
            self.assertEqual(residual, [])
        finally:
            try:
                os.killpg(pgid, 9)
            except ProcessLookupError:
                pass
            try:
                leader.wait(timeout=3)
            except subprocess.TimeoutExpired:
                leader.kill()
                leader.wait(timeout=3)

    def test_group_kill_rechecks_identity_after_term_timeout(self):
        class HungProcess:
            pid = 1234
            returncode = None
            def poll(self):
                return None
            def wait(self, timeout):
                raise subprocess.TimeoutExpired("fake", timeout)
        records = [
            {"status": "ok", "state": "S", "pgid": 77, "starttime": "42"},
            {"status": "ok", "state": "S", "pgid": 88, "starttime": "99"},
        ]
        with patch("ng_remote_executor.read_proc_stat", side_effect=records), patch("ng_remote_executor.os.killpg") as killpg:
            result = local_stop_process_group(HungProcess(), 77, "42")
        self.assertFalse(result["ok"])
        self.assertEqual(result["result"], "identity-mismatch-before-kill")
        killpg.assert_called_once_with(77, 15)

    def test_group_signal_error_is_reported_as_failure(self):
        class LiveProcess:
            pid = 1234
            returncode = None
            def poll(self):
                return None
        record = {"status": "ok", "state": "S", "pgid": 77, "starttime": "42"}
        with patch("ng_remote_executor.read_proc_stat", return_value=record), patch("ng_remote_executor.os.killpg", side_effect=OSError("denied")):
            result = local_stop_process_group(LiveProcess(), 77, "42")
        self.assertFalse(result["ok"])
        self.assertEqual(result["result"], "term-error")

    def test_descendant_signal_error_is_not_success(self):
        tracked = {1234: {"pid": 1234, "pgid": 77, "starttime": "42"}}
        record = {"status": "ok", "state": "S", "pgid": 77, "starttime": "42"}
        with patch("ng_remote_executor.read_proc_stat", return_value=record), patch("ng_remote_executor.os.kill", side_effect=PermissionError("denied")) as kill:
            stopped, residual = local_stop_tracked_descendants(tracked)
        self.assertFalse(stopped)
        self.assertEqual(residual[0]["reason"], "executable-identity-unavailable")
        self.assertFalse(kill.called)

    def test_descendant_identity_unknown_after_term_is_not_success(self):
        tracked = {1234: {"pid": 1234, "pgid": 77, "starttime": "42"}}
        records = [
            {"status": "ok", "state": "S", "pgid": 77, "starttime": "42"},
            {"status": "unknown", "error": "permission denied"},
        ]
        with patch("ng_remote_executor.read_proc_stat", side_effect=records), patch("ng_remote_executor.os.kill") as kill:
            stopped, residual = local_stop_tracked_descendants(tracked)
        self.assertFalse(stopped)
        self.assertEqual(residual[0]["reason"], "executable-identity-unavailable")
        kill.assert_not_called()

    def test_detached_descendant_is_tracked_and_stopped_by_identity(self):
        code = "import os, time; child = os.fork();\nif child: time.sleep(2)\nelse: os.setsid(); time.sleep(5)"
        leader = subprocess.Popen([sys.executable, "-c", code], start_new_session=True)
        pgid = leader.pid
        try:
            time.sleep(0.1)
            scan = local_process_descendants(leader.pid)
            tracked = {item["pid"]: item for item in scan["descendants"]}
            self.assertTrue(scan["complete"])
            self.assertTrue(tracked)
            stopped, residual = local_stop_tracked_descendants(tracked)
            self.assertTrue(stopped)
            self.assertEqual(residual, [])
        finally:
            try:
                os.killpg(pgid, 9)
            except ProcessLookupError:
                pass
            leader.wait(timeout=3)

    def test_ipc_attribution_rejects_unrelated_or_ambiguous_objects(self):
        before = {"shm": {"10"}, "semaphores": set()}
        after = {"shm": {"10", "11", "12"}, "semaphores": set()}
        details = {"shm": {"11": {"creator_pid": 1234}, "12": {"creator_pid": 9999}}, "semaphores": {}}
        result = attribute_new_ipc(before, after, details, {1234})
        self.assertFalse(result["ok"])
        self.assertEqual(result["owned"], {"shm": ["11"], "semaphores": []})
        self.assertEqual(result["unattributed"], {"shm": ["12"], "semaphores": []})
        ambiguous = attribute_new_ipc(before, after, {"shm": None, "semaphores": None}, {1234})
        self.assertFalse(ambiguous["ok"])
        self.assertEqual(ambiguous["owned"], {"shm": [], "semaphores": []})
        self.assertEqual(ambiguous["reason"], "inventory-unavailable")
    def test_ipcrm_failure_has_persistent_reason(self):
        before = {"shm": set(), "semaphores": set()}
        snapshots = [
            {"shm": {"11"}, "semaphores": set()},
            {"shm": set(), "semaphores": set()},
        ]
        details = {"11": {"creator_pid": 1234}}
        failed_ipcrm = type("Run", (), {"returncode": 1, "stdout": "", "stderr": "denied"})()
        with patch("ng_remote_executor.local_ipc_snapshot", side_effect=snapshots), patch("ng_remote_executor.local_ipc_details", return_value=details), patch("ng_remote_executor.subprocess.run", return_value=failed_ipcrm):
            ok, new_ipc, reason = local_remove_new_ipc(before, {1234})
        self.assertFalse(ok)
        self.assertEqual(new_ipc["shm"], ["11"])
        self.assertEqual(reason, "ipcrm-failed")

    def test_ipc_inventory_unavailable_never_calls_ipcrm(self):
        before = {"shm": {"10"}, "semaphores": set()}
        after = {"shm": {"10", "11"}, "semaphores": set()}
        with (
            patch("ng_remote_executor.local_ipc_snapshot", return_value=after),
            patch("ng_remote_executor.local_ipc_details", return_value=None),
            patch("ng_remote_executor.subprocess.run") as run,
        ):
            ok, new_ipc, reason = local_remove_new_ipc(before, {1234})
        self.assertEqual(reason, "inventory-unavailable")
        self.assertFalse(ok)
        self.assertEqual(new_ipc["shm"], ["11"])
        self.assertFalse(any(call.args and call.args[0][0] == "ipcrm" for call in run.call_args_list))
    def test_ipc_ids_report_unavailable_for_failed_or_unsupported_inventory(self):
        failed = type("Run", (), {"returncode": 1, "stdout": "", "stderr": "ipcs failed"})()
        malformed = type("Run", (), {"returncode": 0, "stdout": "unsupported output\n", "stderr": ""})()
        header_malformed = type("Run", (), {"returncode": 0, "stdout": "key shmid owner perms\n0x1 malformed user\n", "stderr": ""})()
        owner = "tester"
        bad_key = type("Run", (), {"returncode": 0, "stdout": f"key shmid owner perms\n0xNOTHEX 7 {owner} 644 1\n", "stderr": ""})()
        bad_detail_field = type("Run", (), {"returncode": 0, "stdout": f"shmid owner cpid lpid\n7 {owner} 123 bad\n", "stderr": ""})()
        short_row = type("Run", (), {"returncode": 0, "stdout": f"key shmid owner perms bytes nattch status\n0x1 7 {owner}\n", "stderr": ""})()
        with patch("ng_remote_executor.subprocess.run", return_value=failed):
            self.assertIsNone(local_ipc_ids("shm"))
        with patch("ng_remote_executor.subprocess.run", return_value=malformed):
            self.assertIsNone(local_ipc_ids("shm"))
        with patch("ng_remote_executor.subprocess.run", return_value=header_malformed):
            self.assertIsNone(local_ipc_ids("shm"))
            self.assertIsNone(local_ipc_details("shm"))
        with patch("ng_remote_executor.pwd.getpwuid", return_value=SimpleNamespace(pw_name=owner)), patch("ng_remote_executor.subprocess.run", return_value=bad_key):
            self.assertIsNone(local_ipc_ids("shm"))
        with patch("ng_remote_executor.pwd.getpwuid", return_value=SimpleNamespace(pw_name=owner)), patch("ng_remote_executor.subprocess.run", return_value=bad_detail_field):
            self.assertIsNone(local_ipc_details("shm"))
        with patch("ng_remote_executor.pwd.getpwuid", return_value=SimpleNamespace(pw_name=owner)), patch("ng_remote_executor.subprocess.run", return_value=short_row):
            self.assertIsNone(local_ipc_ids("shm"))


if __name__ == "__main__":
    unittest.main()
