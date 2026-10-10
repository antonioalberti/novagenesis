#include "NGAL_StageTelemetry.h"

#include <cassert>
#include <chrono>
#include <cstdint>
#include <string>

std::uint64_t CountingClockCalls = 0;

struct CountingClock
{
  using time_point = std::chrono::steady_clock::time_point;

  static time_point now() noexcept
  {
    ++CountingClockCalls;
    return time_point{};
  }
};

int main()
{
#ifndef SPEC027_STAGE_DIAGNOSTICS
#error "Compile this test with SPEC027_STAGE_DIAGNOSTICS enabled"
#endif

  using namespace NGALStageTelemetry;

  const auto runStart = std::chrono::steady_clock::time_point(
      std::chrono::nanoseconds(123456789));
  const std::string runIdA = BuildRunId("stable-pgcs-identity", 42, runStart);
  const std::string runIdB = BuildRunId(
      "stable-pgcs-identity", 42, runStart + std::chrono::nanoseconds(1));
  assert(runIdA != runIdB);
  assert(runIdA.find("stable-pgcs-identity") != std::string::npos);

  Snapshot emptySnapshot{};
  assert(IsDrained(emptySnapshot));
  emptySnapshot.gauges[static_cast<std::size_t>(Gauge::NetworkQueueEntries)] = 1;
  assert(!IsDrained(emptySnapshot));
  emptySnapshot.gauges[static_cast<std::size_t>(Gauge::NetworkQueueEntries)] = 0;
  emptySnapshot.counters[static_cast<std::size_t>(Counter::InputAdmissions)] = 1;
  assert(!IsDrained(emptySnapshot));
  emptySnapshot.counters[static_cast<std::size_t>(Counter::InputDispatches)] = 1;
  assert(!IsDrained(emptySnapshot));
  emptySnapshot.counters[static_cast<std::size_t>(Counter::InputCompletions)] = 1;
  assert(IsDrained(emptySnapshot));
  emptySnapshot.counters[static_cast<std::size_t>(Counter::SarReceiveCalls)] = 1;
  assert(!IsDrained(emptySnapshot));
  emptySnapshot.counters[static_cast<std::size_t>(Counter::SarRejectedFrames)] = 1;
  assert(IsDrained(emptySnapshot));
  emptySnapshot.counters[static_cast<std::size_t>(Counter::NetworkEnqueued)] = 1;
  assert(!IsDrained(emptySnapshot));

  ResetForTests();
  Increment(Counter::RawFrames, 3);
  Increment(Counter::RawBytes, 900);
  assert(ReadCounter(Counter::RawFrames) == 3);
  assert(ReadCounter(Counter::RawBytes) == 900);
  Increment(Counter::SarCompletions, 2);
  assert(ReadCounter(Counter::SarCompletions) == 2);

  {
    OutcomeCounterGuard outcome(Counter::SarFragmentsAccepted,
                                Counter::SarRejectedFrames);
  }
  assert(ReadCounter(Counter::SarFragmentsAccepted) == 0);
  assert(ReadCounter(Counter::SarRejectedFrames) == 1);
  {
    OutcomeCounterGuard outcome(Counter::SarFragmentsAccepted,
                                Counter::SarRejectedFrames);
    outcome.MarkAccepted();
  }
  assert(ReadCounter(Counter::SarFragmentsAccepted) == 1);
  assert(ReadCounter(Counter::SarRejectedFrames) == 1);

  AddGauge(Gauge::NetworkQueueEntries, 3);
  AddGauge(Gauge::NetworkQueueBytes, 900);
  AddGauge(Gauge::NetworkQueueEntries, -1);
  AddGauge(Gauge::NetworkQueueBytes, -300);
  assert(ReadGauge(Gauge::NetworkQueueEntries) == 2);
  assert(ReadGauge(Gauge::NetworkQueueBytes) == 600);
  assert(ReadGaugeHighWater(Gauge::NetworkQueueEntries) == 3);
  assert(ReadGaugeHighWater(Gauge::NetworkQueueBytes) == 900);
  SetGauge(Gauge::ReassemblyAssemblies, 4);
  SetGauge(Gauge::ReassemblyAssemblies, 2);
  assert(ReadGauge(Gauge::ReassemblyAssemblies) == 2);
  assert(ReadGaugeHighWater(Gauge::ReassemblyAssemblies) == 4);

  ResetSamplerForTests();
  unsigned int selected = 0;
  for (unsigned int i = 0; i < 256; ++i)
  {
    if (ShouldSample(Timing::NetworkResidence))
      ++selected;
  }
  assert(selected == 2);

  ObserveNanoseconds(Timing::NetworkResidence, 100);
  ObserveNanoseconds(Timing::NetworkResidence, 200);
  const Snapshot snapshot = Capture();
  const HistogramSnapshot& residence =
      snapshot.timings[static_cast<unsigned int>(Timing::NetworkResidence)];
  assert(residence.samples == 2);
  assert(residence.P50Nanoseconds() >= 100);
  assert(residence.P50Nanoseconds() <= 200);
  assert(residence.P95Nanoseconds() >= 200);
  assert(residence.P99Nanoseconds() >= 200);

  FixedSampleTable<8> samples;
  const auto sampleTime = std::chrono::steady_clock::now();
  assert(samples.TryRecord(reinterpret_cast<void*>(0x1000), sampleTime));
  std::chrono::steady_clock::time_point recovered{};
  assert(samples.Take(reinterpret_cast<void*>(0x1000), recovered));
  assert(recovered == sampleTime);
  assert(samples.Size() == 0);

  FixedSampleTable<2> bounded;
  assert(bounded.TryRecord(reinterpret_cast<void*>(0x2000), sampleTime));
  assert(bounded.TryRecord(reinterpret_cast<void*>(0x3000), sampleTime));
  assert(!bounded.TryRecord(reinterpret_cast<void*>(0x4000), sampleTime));
  assert(bounded.Take(reinterpret_cast<void*>(0x2000), recovered));
  assert(bounded.TryRecord(reinterpret_cast<void*>(0x4000), sampleTime));

  ResetForTests();
  assert(IsSysVShmAttachFailure(reinterpret_cast<void*>(-1)));
  assert(!IsSysVShmAttachFailure(reinterpret_cast<void*>(0x1000)));

  ResetSamplerForTests();
  QueueTelemetry<8> networkQueue(
      Counter::NetworkEnqueued, Counter::NetworkDequeued,
      Gauge::NetworkQueueEntries, Timing::NetworkResidence,
      Counter::NetworkEnqueuedBytes, Counter::NetworkDequeuedBytes,
      Gauge::NetworkQueueBytes);
  int queuedItem = 0;
  const auto queuedAt = std::chrono::steady_clock::now();
  assert(networkQueue.EnqueueAt(&queuedItem, 256, queuedAt));
  assert(ReadCounter(Counter::NetworkEnqueued) == 1);
  assert(ReadCounter(Counter::NetworkEnqueuedBytes) == 256);
  assert(ReadGauge(Gauge::NetworkQueueEntries) == 1);
  assert(ReadGauge(Gauge::NetworkQueueBytes) == 256);
  networkQueue.DequeueAt(&queuedItem, 256, queuedAt + std::chrono::nanoseconds(500));
  assert(ReadCounter(Counter::NetworkDequeued) == 1);
  assert(ReadCounter(Counter::NetworkDequeuedBytes) == 256);
  assert(ReadGauge(Gauge::NetworkQueueEntries) == 0);
  assert(ReadGauge(Gauge::NetworkQueueBytes) == 0);
  assert(ReadGaugeHighWater(Gauge::NetworkQueueEntries) == 1);
  assert(ReadGaugeHighWater(Gauge::NetworkQueueBytes) == 256);
  const Snapshot queueSnapshot = Capture();
  const HistogramSnapshot& queueResidence =
      queueSnapshot.timings[static_cast<unsigned int>(Timing::NetworkResidence)];
  assert(queueResidence.samples == 1);
  assert(queueResidence.P95Nanoseconds() >= 500);

  ResetForTests();
  ResetSamplerForTests();
  CountingClockCalls = 0;
  QueueTelemetry<8, CountingClock> sampledQueue(
      Counter::NetworkEnqueued, Counter::NetworkDequeued,
      Gauge::NetworkQueueEntries, Timing::NetworkResidence);
  int sampledItem = 0;
  int unsampledItem = 0;
  assert(sampledQueue.Enqueue(&sampledItem));
  assert(CountingClockCalls == 1);
  for (int i = 0; i < 127; ++i)
    assert(sampledQueue.Enqueue(&unsampledItem));
  assert(CountingClockCalls == 1);
  assert(!sampledQueue.Dequeue(&unsampledItem));
  assert(CountingClockCalls == 1);
  assert(sampledQueue.Dequeue(&sampledItem));
  assert(CountingClockCalls == 2);

  ResetForTests();
  errno = EAGAIN;
  const int busyResult = RunSemaphoreTryWait(
      []() { return -1; }, Counter::ShmReadSemaphoreBusy,
      Counter::ShmReadSemaphoreErrors);
  assert(busyResult == -1);
  assert(ReadCounter(Counter::ShmReadSemaphoreBusy) == 1);
  assert(ReadCounter(Counter::ShmReadSemaphoreErrors) == 0);
  errno = EBADF;
  const int errorResult = RunSemaphoreTryWait(
      []() { return -1; }, Counter::ShmWriteSemaphoreBusy,
      Counter::ShmWriteSemaphoreErrors);
  assert(errorResult == -1);
  assert(ReadCounter(Counter::ShmWriteSemaphoreBusy) == 0);
  assert(ReadCounter(Counter::ShmWriteSemaphoreErrors) == 1);
  assert(errno == EBADF);

  ResetForTests();
  Increment(Counter::ShmReadSegmentErrors);
  Increment(Counter::ShmReadDetachErrors);
  Increment(Counter::ShmWriteLookupErrors);
  Increment(Counter::ShmWriteCapacityErrors);
  Increment(Counter::ShmWriteDetachErrors);
  const std::string ipcFailures =
      FormatSnapshot(Capture(), "run-x", 42, 10, false);
  assert(ipcFailures.find("shm_read_segment_errors=1") != std::string::npos);
  assert(ipcFailures.find("shm_read_detach_errors=1") != std::string::npos);
  assert(ipcFailures.find("shm_write_lookup_errors=1") != std::string::npos);
  assert(ipcFailures.find("shm_write_capacity_errors=1") != std::string::npos);
  assert(ipcFailures.find("shm_write_detach_errors=1") != std::string::npos);

  const std::string line = FormatSnapshot(snapshot, "run-x", 42, 10, false);
  assert(line.find("SPEC027_STAGE_STATS run=run-x pid=42 elapsed_s=10 phase=periodic") == 0);
  assert(line.find("raw_frames=3") != std::string::npos);
  assert(line.find("network_queue_entries=2") != std::string::npos);
  assert(line.find("network_residence_samples=2") != std::string::npos);
  assert(line.find("network_residence_p50_low_ns=") != std::string::npos);
  assert(line.find("network_residence_p95_low_ns=") != std::string::npos);
  assert(line.find("network_residence_p95_high_ns=") != std::string::npos);
  assert(line.find("shm_read_semaphore_wait_samples=") != std::string::npos);
  assert(line.find("shm_write_semaphore_wait_samples=") != std::string::npos);
  assert(line.find("shm_read_semaphore_errors=0") != std::string::npos);
  assert(line.find("shm_write_semaphore_errors=0") != std::string::npos);
  assert(line.find("shm_write_messages=0") != std::string::npos);

  const std::string finalLine = FormatSnapshot(snapshot, "run-x", 42, 10, true);
  assert(finalLine.find("SPEC027_STAGE_FINAL run=run-x pid=42 elapsed_s=10 phase=final") == 0);
  return 0;
}
