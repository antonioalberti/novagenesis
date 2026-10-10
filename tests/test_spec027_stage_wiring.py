import pathlib
import re
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[1]


def function_body(path, start, end):
    text = (ROOT / path).read_text(encoding="utf-8")
    begin = text.index(start)
    finish = text.index(end, begin + len(start))
    return text[begin:finish]


def braced_body(text, marker):
    opening = text.index("{", text.index(marker))
    depth = 0
    for index in range(opening, len(text)):
        if text[index] == "{":
            depth += 1
        elif text[index] == "}":
            depth -= 1
            if depth == 0:
                return text[opening + 1:index], index + 1
    raise AssertionError(f"unterminated block after {marker}")


class StageTelemetryWiringTests(unittest.TestCase):
    def test_input_queue_rejections_are_counted(self):
        body = function_body(
            "Common/src/GW.cpp",
            "void GW::PushToInputQueue(",
            "void GW::PushToOutputQueue(",
        )
        self.assertIn("Counter::InputRejected", body)

    def test_shm_read_segment_and_detach_errors_are_counted(self):
        body = function_body(
            "Common/src/GW.cpp",
            "int GW::ReadFromSharedMemory3()",
            "int GW::WriteToSharedMemory3(",
        )
        self.assertIn("Counter::ShmReadSegmentErrors", body)
        self.assertIn("Counter::ShmReadDetachErrors", body)

    def test_shm_write_outcomes_are_counted_at_the_write_boundary(self):
        body = function_body(
            "Common/src/GW.cpp",
            "int GW::WriteToSharedMemory3(",
            "int GW::ReturnIPCSHMID(",
        )
        for counter in (
            "ShmWriteFlagBusy",
            "ShmWriteMessages",
            "ShmWriteBytes",
            "ShmWriteFailures",
            "ShmWriteOpenErrors",
            "ShmWritePostErrors",
            "ShmWriteLookupErrors",
            "ShmWriteCapacityErrors",
            "ShmWriteAttachErrors",
            "ShmWriteDetachErrors",
        ):
            with self.subTest(counter=counter):
                self.assertIn(f"Counter::{counter}", body)

    def test_shm_write_attach_failure_never_enters_attached_path(self):
        body = function_body(
            "Common/src/GW.cpp",
            "int GW::WriteToSharedMemory3(",
            "int GW::ReturnIPCSHMID(",
        )
        begin = body.index("shm_address = shmat(shmid, NULL, 0)")
        end = body.index("// Semaphore kept open", begin)
        attach = body[begin:end]
        condition = "if (!NGALStageTelemetry::IsSysVShmAttachFailure(shm_address))"
        self.assertIn(condition, attach)
        _, after_success = braced_body(attach, condition)
        failure_clause = re.sub(r"^\s*//[^\n]*\n", "", attach[after_success:], count=1).lstrip()
        self.assertTrue(failure_clause.startswith("else"))
        failure_body, _ = braced_body(failure_clause, "else")
        self.assertIn("Counter::ShmWriteAttachErrors", failure_body)
        self.assertIn('perror("writing SHM : shmat")', failure_body)
        self.assertIn("Status = ERROR;", failure_body)
        self.assertNotIn("sem_trywait", failure_body)
        self.assertNotIn("shmdt(", failure_body)
        self.assertNotIn("data[", failure_body)

    def test_run_identity_and_final_drain_are_wired(self):
        init = (ROOT / "PGCS/src/PGRunInitialization01.cpp").read_text()
        pg = (ROOT / "PGCS/src/PG.cpp").read_text()
        self.assertIn("BuildRunId(", init)
        self.assertIn("IsDrained(", pg)
        self.assertNotIn("StopStageTelemetryReporter(false)", pg)

    def test_performance_profile_suppresses_debug_queue_logging(self):
        gw = (ROOT / "Common/src/GW.cpp").read_text()
        self.assertIn(
            "#if defined(DEBUG_NETWORK_QUEUE) && !defined(SPEC027_PERF_PROFILE)",
            gw,
        )

    def test_heartbeat_keeps_the_existing_unconditional_emission(self):
        pg = function_body(
            "PGCS/src/PG.cpp",
            "void PG::HeartbeatLoop()",
            "void PG::EmitHeartbeat(",
        )
        self.assertIn('EmitHeartbeat("SPEC028_STATS");', pg)
        self.assertNotIn("if (StressEnabled)", pg)


if __name__ == "__main__":
    unittest.main()
