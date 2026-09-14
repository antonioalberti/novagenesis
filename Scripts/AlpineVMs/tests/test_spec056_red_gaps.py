"""Focused SPEC-056 RED regressions for the remaining local-mode gaps.

These tests deliberately encode the desired fail-closed contract.  They use
only temporary directories and mocked subprocess/IPC observations; they do not
launch VMs, SSH, or destructive cleanup commands.
"""

from __future__ import annotations

import hashlib
import json
import sys
import tempfile
import unittest
from argparse import Namespace
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch


sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import ng_remote_executor as executor


OWNER = "spec056-test-owner"
SHM_HEADER = "key shmid owner perms bytes nattch status"
SEM_HEADER = "key semid owner perms nsems"


def completed(stdout: str) -> SimpleNamespace:
    return SimpleNamespace(returncode=0, stdout=stdout, stderr="")


class Spec056IpcParserRedTests(unittest.TestCase):
    """The complete IPC inventory is invalid when any row is invalid."""

    def assert_ipc_rejected(self, kind: str, output: str) -> None:
        with (
            patch.object(executor.pwd, "getpwuid", return_value=SimpleNamespace(pw_name=OWNER)),
            patch.object(executor.subprocess, "run", return_value=completed(output)),
        ):
            self.assertIsNone(executor.local_ipc_ids(kind))

    def test_ipc_parser_rejects_invalid_shm_permissions(self):
        output = f"{SHM_HEADER}\n0x1 7 {OWNER} bogus 4096 0 dest\n"
        self.assert_ipc_rejected("shm", output)

    def test_ipc_parser_rejects_invalid_shm_bytes(self):
        output = f"{SHM_HEADER}\n0x1 7 {OWNER} 600 not-bytes 0 dest\n"
        self.assert_ipc_rejected("shm", output)

    def test_ipc_parser_rejects_invalid_semaphore_permissions(self):
        output = f"{SEM_HEADER}\n0x1 8 {OWNER} invalid 3\n"
        self.assert_ipc_rejected("semaphores", output)

    def test_ipc_parser_rejects_invalid_semaphore_nsems(self):
        output = f"{SEM_HEADER}\n0x1 8 {OWNER} 600 not-nsems\n"
        self.assert_ipc_rejected("semaphores", output)

    def test_ipc_parser_rejects_mixed_valid_and_invalid_shm_rows(self):
        output = (
            f"{SHM_HEADER}\n"
            f"0x1 7 {OWNER} 600 4096 0 dest\n"
            f"0x2 9 {OWNER} 600 invalid-bytes 0 dest\n"
        )
        self.assert_ipc_rejected("shm", output)

    def test_ipc_parser_rejects_mixed_valid_and_invalid_semaphore_rows(self):
        output = (
            f"{SEM_HEADER}\n"
            f"0x1 8 {OWNER} 600 3\n"
            f"0x2 10 {OWNER} 600 invalid-nsems\n"
        )
        self.assert_ipc_rejected("semaphores", output)


class Spec056CleanupRedTests(unittest.TestCase):
    def test_local_remove_new_ipc_preserves_residual_reason_after_failed_removal(self):
        """A surviving post-removal object outranks the transient ipcrm error."""
        before = {"shm": set(), "semaphores": set()}
        after = {"shm": {"11"}, "semaphores": set()}
        failed_ipcrm = SimpleNamespace(returncode=1, stdout="", stderr="denied")

        with (
            patch.object(executor, "local_ipc_snapshot", side_effect=[after, after]),
            patch.object(
                executor,
                "local_ipc_details",
                side_effect=lambda kind: {"11": {"creator_pid": 1234}} if kind == "shm" else {},
            ),
            patch.object(executor.subprocess, "run", return_value=failed_ipcrm),
        ):
            ok, new_ipc, reason = executor.local_remove_new_ipc(before, {1234})

        self.assertFalse(ok)
        self.assertEqual(new_ipc, {"shm": ["11"], "semaphores": []})
        self.assertEqual(reason, "residual-ipc")


class Spec056ProvenanceRedTests(unittest.TestCase):
    def test_acceptance_rejects_an_empty_build_manifest(self):
        provenance = {
            "git_clean": True,
            "build_linkage": True,
            "controller_identity": True,
            "plan_snapshot": True,
            "build_manifest": {},
        }

        eligibility = executor.local_acceptance_eligibility(provenance)

        self.assertFalse(eligibility["eligible"])
        self.assertTrue(any("build manifest" in blocker for blocker in eligibility["blockers"]))


class Spec056EvidenceRedTests(unittest.TestCase):
    def test_evidence_manifest_uses_schema_v2(self):
        with tempfile.TemporaryDirectory(prefix="spec056-manifest-", dir=Path.home()) as td:
            evidence_dir = Path(td)
            (evidence_dir / "result.json").write_text("{}\n", encoding="utf-8")

            manifest_path = executor.write_evidence_manifest(
                evidence_dir,
                {"trial_id": "red-manifest", "mode": "local"},
            )
            manifest = json.loads(manifest_path.read_text(encoding="utf-8"))

        self.assertEqual(manifest["schema_version"], 2)

    def test_evidence_manifest_publishes_terminal_seal_record(self):
        with tempfile.TemporaryDirectory(prefix="spec056-seal-", dir=Path.home()) as td:
            evidence_dir = Path(td)
            (evidence_dir / "result.json").write_text("{}\n", encoding="utf-8")

            manifest_path = executor.write_evidence_manifest(
                evidence_dir,
                {"trial_id": "red-seal", "mode": "local"},
            )
            seal_path = evidence_dir / "terminal-seal.json"
            self.assertTrue(seal_path.is_file(), "a sealed bundle needs terminal-seal.json")
            seal = json.loads(seal_path.read_text(encoding="utf-8"))
            self.assertEqual(
                seal.get("manifest_sha256"),
                hashlib.sha256(manifest_path.read_bytes()).hexdigest(),
            )


class Spec056FixtureRedTests(unittest.TestCase):
    def test_local_fixture_cannot_pass_without_launch_events_after_preflight(self):
        """An empty-role fixture must not become an accepted local PASS."""
        with tempfile.TemporaryDirectory(prefix="spec056-fixture-", dir=Path.home()) as td:
            root = Path(td)
            build = root / "build"
            io = root / "io"
            source = io / "Source1"
            repository = io / "Repository1"
            evidence = root / "evidence"
            for path in (build, source, repository, evidence):
                path.mkdir(parents=True)

            plan_path = root / "plan.json"
            plan_path.write_text(
                json.dumps(
                    {
                        "schema_version": 1,
                        "roles": [],
                        "timeouts": {"readiness": 0.1, "observation": 0.01, "total": 1},
                        "runtime_oracle": {
                            "type": "files",
                            "source": str(source),
                            "repository": str(repository),
                            "pattern": "*.jpg",
                            "expected_count": 0,
                            "hash": "sha256",
                        },
                    }
                ),
                encoding="utf-8",
            )
            config = {
                "NG_LOCAL_REPO_PATH": str(root),
                "NG_LOCAL_BUILD_PATH": str(build),
                "NG_LOCAL_IO_PATH": str(io),
                "NG_LOCAL_EVIDENCE_PATH": str(evidence),
            }
            args = Namespace(
                plan=str(plan_path),
                scenario="L0",
                debug_profile="obs-normal",
                trial="no-launches",
                env=config,
            )
            complete_provenance = {
                "git_clean": True,
                "build_linkage": True,
                "controller_identity": True,
                "plan_snapshot": False,
            }
            empty_ipc = {"shm": set(), "semaphores": set()}

            with (
                patch.object(executor, "local_provenance", return_value=complete_provenance),
                patch.object(executor, "local_ipc_snapshot", return_value=empty_ipc),
                patch.object(executor.subprocess, "Popen") as popen,
            ):
                rc = executor.run_local_trial(args)

            trial_dir = evidence / "no-launches"
            events = [
                json.loads(line)
                for line in (trial_dir / "controller-events.jsonl").read_text(encoding="utf-8").splitlines()
                if line.strip()
            ]
            result = json.loads((trial_dir / "result.json").read_text(encoding="utf-8"))
            preflight = [event for event in events if event["event"] == "preflight"]
            launches = [event for event in events if event["event"] == "launch"]

        self.assertTrue(preflight and preflight[-1]["result"]["ok"])
        popen.assert_not_called()
        self.assertFalse(launches)
        self.assertNotEqual(result["runtime_result"], "PASS")
        self.assertNotEqual(rc, 0)


if __name__ == "__main__":
    unittest.main()
