#ifndef _NGAL_STAGE_TELEMETRY_H
#define _NGAL_STAGE_TELEMETRY_H

namespace NGALStageTelemetry
{
inline bool IsSysVShmAttachFailure(const void* address) noexcept
{
  return address == reinterpret_cast<const void*>(-1);
}
}

#ifdef SPEC027_STAGE_DIAGNOSTICS

#include <array>
#include <atomic>
#include <cerrno>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <sstream>
#include <string>
#include <utility>

namespace NGALStageTelemetry
{
enum class Counter : std::size_t
{
  RawReceiveCalls,
  RawFrames,
  RawBytes,
  NgFrames,
  NgBytes,
  NonNgFrames,
  RxTruncated,
  RxTooShort,
  SarReceiveCalls,
  SarFragmentsAccepted,
  SarRejectedFrames,
  SarCompletions,
  NetworkEnqueued,
  NetworkEnqueuedBytes,
  NetworkDequeued,
  NetworkDequeuedBytes,
  NetworkRejected,
  SampleTableOverflows,
  NetworkNewMessageFailures,
  NetworkDeserializeFailures,
  InputAdmissions,
  InputRejected,
  InputDispatches,
  InputCompletions,
  OutputAdmissions,
  OutputDequeued,
  OutputRetries,
  OutputCompletions,
  ShmReadPolls,
  ShmReadSegmentsProbed,
  ShmReadSemaphoreBusy,
  ShmReadSemaphoreErrors,
  ShmReadFlagReady,
  ShmReadMessages,
  ShmReadBytes,
  ShmReadDecodeErrors,
  ShmReadNewMessageFailures,
  ShmReadAttachErrors,
  ShmReadOpenErrors,
  ShmReadPostErrors,
  ShmReadSegmentErrors,
  ShmReadDetachErrors,
  ShmWriteCalls,
  ShmWriteSegmentsProbed,
  ShmWriteSemaphoreBusy,
  ShmWriteSemaphoreErrors,
  ShmWriteFlagBusy,
  ShmWriteMessages,
  ShmWriteBytes,
  ShmWriteFailures,
  ShmWriteOpenErrors,
  ShmWriteAttachErrors,
  ShmWritePostErrors,
  ShmWriteLookupErrors,
  ShmWriteCapacityErrors,
  ShmWriteDetachErrors,
  ShmReadSemaphoreAcquisitions,
  ShmWriteSemaphoreAcquisitions,
  Count
};

enum class Gauge : std::size_t
{
  ReassemblyAssemblies,
  ReassemblyBytes,
  NetworkQueueEntries,
  NetworkQueueBytes,
  InputQueueEntries,
  OutputQueueEntries,
  Count
};

enum class Timing : std::size_t
{
  RawReceiveCall,
  RawFrameValidation,
  SarReceiveFragment,
  NetworkProducerLockWait,
  NetworkProducerLockHold,
  NetworkConsumerLockWait,
  NetworkConsumerLockHold,
  NetworkResidence,
  NetworkCopy,
  GatewayMessageCreate,
  GatewayDeserialize,
  InputQueueLockWait,
  InputQueueLockHold,
  InputResidence,
  SchedulerLateness,
  ActionService,
  ShmReadSemaphoreWait,
  ShmReadOperation,
  ShmWriteSemaphoreWait,
  ShmWriteOperation,
  Count
};

constexpr std::size_t CounterCount = static_cast<std::size_t>(Counter::Count);
constexpr std::size_t GaugeCount = static_cast<std::size_t>(Gauge::Count);
constexpr std::size_t TimingCount = static_cast<std::size_t>(Timing::Count);
constexpr std::size_t HistogramBucketCount = 64;
constexpr std::uint32_t SampleEvery = 128;

// Fixed-capacity side table for sampled queue timestamps. It performs no
// per-event allocation; callers serialize access with the owning queue mutex.
template <std::size_t Capacity>
class FixedSampleTable
{
  static_assert(Capacity > 0, "sample table must have non-zero capacity");

  struct Slot
  {
    const void* key = nullptr;
    std::chrono::steady_clock::time_point timestamp{};
    unsigned char state = 0; // 0 empty, 1 occupied, 2 tombstone
  };

public:
  bool TryRecord(const void* key,
                 std::chrono::steady_clock::time_point timestamp) noexcept
  {
    if (key == nullptr)
      return false;

    std::size_t firstTombstone = Capacity;
    const std::size_t start = Hash(key) % Capacity;
    for (std::size_t probe = 0; probe < Capacity; ++probe)
    {
      const std::size_t index = (start + probe) % Capacity;
      Slot& slot = slots[index];
      if (slot.state == 1 && slot.key == key)
      {
        slot.timestamp = timestamp;
        return true;
      }
      if (slot.state == 2 && firstTombstone == Capacity)
        firstTombstone = index;
      if (slot.state == 0)
      {
        const std::size_t target =
            firstTombstone == Capacity ? index : firstTombstone;
        slots[target] = Slot{key, timestamp, 1};
        ++size;
        return true;
      }
    }
    if (firstTombstone != Capacity)
    {
      slots[firstTombstone] = Slot{key, timestamp, 1};
      ++size;
      return true;
    }
    return false;
  }

  bool Take(const void* key,
            std::chrono::steady_clock::time_point& timestamp) noexcept
  {
    if (key == nullptr)
      return false;
    const std::size_t start = Hash(key) % Capacity;
    for (std::size_t probe = 0; probe < Capacity; ++probe)
    {
      Slot& slot = slots[(start + probe) % Capacity];
      if (slot.state == 0)
        return false;
      if (slot.state == 1 && slot.key == key)
      {
        timestamp = slot.timestamp;
        slot.key = nullptr;
        slot.state = 2;
        --size;
        return true;
      }
    }
    return false;
  }

  std::size_t Size() const noexcept { return size; }

private:
  static std::size_t Hash(const void* key) noexcept
  {
    std::uintptr_t value = reinterpret_cast<std::uintptr_t>(key);
    value >>= 4;
    value ^= value >> 17;
    value *= static_cast<std::uintptr_t>(0xed5ad4bbU);
    value ^= value >> 11;
    return static_cast<std::size_t>(value);
  }

  std::array<Slot, Capacity> slots{};
  std::size_t size = 0;
};

struct HistogramSnapshot
{
  std::uint64_t samples = 0;
  std::array<std::uint64_t, HistogramBucketCount> buckets{};

  std::pair<std::uint64_t, std::uint64_t> QuantileBoundsNanoseconds(
      unsigned int percentile) const noexcept
  {
    if (samples == 0)
      return {0, 0};
    if (percentile < 1)
      percentile = 1;
    if (percentile > 100)
      percentile = 100;

    const std::uint64_t target = (samples / 100) * percentile +
        ((samples % 100) * percentile + 99) / 100;
    std::uint64_t cumulative = 0;
    for (std::size_t i = 0; i < buckets.size(); ++i)
    {
      cumulative += buckets[i];
      if (cumulative >= target)
      {
        if (i >= 63)
          return {static_cast<std::uint64_t>(1) << 63,
                  std::numeric_limits<std::uint64_t>::max()};
        const std::uint64_t lower = i == 0 ? 0 :
            (static_cast<std::uint64_t>(1) << i);
        const std::uint64_t upper = static_cast<std::uint64_t>(1) << (i + 1);
        return {lower, upper};
      }
    }
    return {static_cast<std::uint64_t>(1) << 63,
            std::numeric_limits<std::uint64_t>::max()};
  }

  std::uint64_t QuantileBoundNanoseconds(unsigned int percentile) const noexcept
  {
    return QuantileBoundsNanoseconds(percentile).second;
  }

  std::uint64_t P50Nanoseconds() const noexcept
  {
    return QuantileBoundNanoseconds(50);
  }
  std::uint64_t P95Nanoseconds() const noexcept
  {
    return QuantileBoundNanoseconds(95);
  }
  std::uint64_t P99Nanoseconds() const noexcept
  {
    return QuantileBoundNanoseconds(99);
  }
};

struct Snapshot
{
  std::array<std::uint64_t, CounterCount> counters{};
  std::array<std::int64_t, GaugeCount> gauges{};
  std::array<std::uint64_t, GaugeCount> highWater{};
  std::array<HistogramSnapshot, TimingCount> timings{};
};

inline std::string BuildRunId(
    const std::string& processIdentity, int pid,
    std::chrono::steady_clock::time_point started)
{
  const auto monotonicNanoseconds =
      std::chrono::duration_cast<std::chrono::nanoseconds>(
          started.time_since_epoch()).count();
  return "pgcs-" + std::to_string(pid) + "-" +
      std::to_string(monotonicNanoseconds) + "-" + processIdentity;
}

// Call after application producers/consumers have stopped. A drained result
// requires empty gauges and reconciled admission/dispatch/completion counters.
inline bool IsDrained(const Snapshot& snapshot) noexcept
{
  const auto gauge = [&snapshot](Gauge id) {
    return snapshot.gauges[static_cast<std::size_t>(id)];
  };
  const auto counter = [&snapshot](Counter id) {
    return snapshot.counters[static_cast<std::size_t>(id)];
  };

  return gauge(Gauge::ReassemblyAssemblies) == 0 &&
      gauge(Gauge::ReassemblyBytes) == 0 &&
      gauge(Gauge::NetworkQueueEntries) == 0 &&
      gauge(Gauge::NetworkQueueBytes) == 0 &&
      gauge(Gauge::InputQueueEntries) == 0 &&
      gauge(Gauge::OutputQueueEntries) == 0 &&
      counter(Counter::NetworkEnqueued) == counter(Counter::NetworkDequeued) &&
      counter(Counter::NetworkEnqueuedBytes) ==
          counter(Counter::NetworkDequeuedBytes) &&
      counter(Counter::SarReceiveCalls) ==
          counter(Counter::SarFragmentsAccepted) +
              counter(Counter::SarRejectedFrames) &&
      counter(Counter::InputAdmissions) == counter(Counter::InputDispatches) &&
      counter(Counter::InputDispatches) == counter(Counter::InputCompletions);
}

struct HistogramStorage
{
  std::atomic<std::uint64_t> samples{0};
  std::array<std::atomic<std::uint64_t>, HistogramBucketCount> buckets{};

  HistogramStorage() noexcept
  {
    for (std::size_t i = 0; i < buckets.size(); ++i)
      buckets[i].store(0, std::memory_order_relaxed);
  }
};

struct Storage
{
  std::array<std::atomic<std::uint64_t>, CounterCount> counters{};
  std::array<std::atomic<std::int64_t>, GaugeCount> gauges{};
  std::array<std::atomic<std::uint64_t>, GaugeCount> highWater{};
  std::array<HistogramStorage, TimingCount> timings{};

  Storage() noexcept
  {
    for (std::size_t i = 0; i < counters.size(); ++i)
      counters[i].store(0, std::memory_order_relaxed);
    for (std::size_t i = 0; i < gauges.size(); ++i)
    {
      gauges[i].store(0, std::memory_order_relaxed);
      highWater[i].store(0, std::memory_order_relaxed);
    }
  }
};

inline Storage Metrics;
inline thread_local std::array<std::uint32_t, TimingCount> SampleSequence{};

inline std::size_t Index(Counter counter) noexcept
{
  return static_cast<std::size_t>(counter);
}
inline std::size_t Index(Gauge gauge) noexcept
{
  return static_cast<std::size_t>(gauge);
}
inline std::size_t Index(Timing timing) noexcept
{
  return static_cast<std::size_t>(timing);
}

inline void Increment(Counter counter, std::uint64_t amount = 1) noexcept
{
  Metrics.counters[Index(counter)].fetch_add(amount, std::memory_order_relaxed);
}

inline void IncrementRelaxed(Counter counter, std::uint64_t amount = 1) noexcept
{
  Metrics.counters[Index(counter)].fetch_add(amount, std::memory_order_relaxed);
}

template <typename TryWait>
inline int RunSemaphoreTryWait(TryWait&& tryWait, Counter busyCounter,
                               Counter errorCounter) noexcept
{
  const int result = tryWait();
  if (result != 0)
  {
    const int savedErrno = errno;
    Increment(savedErrno == EAGAIN ? busyCounter : errorCounter);
    errno = savedErrno;
  }
  return result;
}

inline void UpdateHighWater(std::size_t index, std::uint64_t candidate) noexcept
{
  std::uint64_t previous = Metrics.highWater[index].load(std::memory_order_relaxed);
  while (previous < candidate &&
         !Metrics.highWater[index].compare_exchange_weak(
             previous, candidate, std::memory_order_relaxed,
             std::memory_order_relaxed))
  {
  }
}

inline void AddGauge(Gauge gauge, std::int64_t delta) noexcept
{
  const std::size_t index = Index(gauge);
  std::int64_t previous = Metrics.gauges[index].load(std::memory_order_relaxed);
  std::int64_t current = 0;
  for (;;)
  {
    if (delta < 0 && previous < -delta)
      return;
    current = previous + delta;
    if (Metrics.gauges[index].compare_exchange_weak(
            previous, current, std::memory_order_relaxed,
            std::memory_order_relaxed))
      break;
  }
  if (current > 0)
    UpdateHighWater(index, static_cast<std::uint64_t>(current));
}

inline void SetGauge(Gauge gauge, std::uint64_t value) noexcept
{
  const std::size_t index = Index(gauge);
  Metrics.gauges[index].store(static_cast<std::int64_t>(value),
                              std::memory_order_relaxed);
  if (value > 0)
    UpdateHighWater(index, value);
}

inline bool ShouldSample(Timing timing) noexcept
{
  const std::size_t index = Index(timing);
  const std::uint32_t sequence = SampleSequence[index]++;
  return (sequence % SampleEvery) == 0;
}

inline std::size_t BucketForNanoseconds(std::uint64_t nanoseconds) noexcept
{
  std::size_t bucket = 0;
  while (nanoseconds > 1 && bucket < HistogramBucketCount - 1)
  {
    nanoseconds >>= 1;
    ++bucket;
  }
  return bucket;
}

inline void ObserveNanoseconds(Timing timing, std::uint64_t nanoseconds) noexcept
{
  HistogramStorage& histogram = Metrics.timings[Index(timing)];
  histogram.buckets[BucketForNanoseconds(nanoseconds)].fetch_add(
      1, std::memory_order_relaxed);
  histogram.samples.fetch_add(1, std::memory_order_relaxed);
}

template <std::size_t Capacity, typename Clock = std::chrono::steady_clock>
class QueueTelemetry
{
public:
  QueueTelemetry(Counter enqueued, Counter dequeued, Gauge entries,
                 Timing residence, Counter enqueuedBytes = Counter::Count,
                 Counter dequeuedBytes = Counter::Count,
                 Gauge bytes = Gauge::Count) noexcept
      : Enqueued(enqueued), Dequeued(dequeued), Entries(entries),
        Residence(residence), EnqueuedBytes(enqueuedBytes),
        DequeuedBytes(dequeuedBytes), Bytes(bytes)
  {
  }

  bool Enqueue(const void* key, std::uint64_t byteCount = 0) noexcept
  {
    return EnqueueWithTimestamp(key, byteCount,
                                []() noexcept { return Clock::now(); });
  }

  bool EnqueueAt(const void* key, std::uint64_t byteCount,
                 std::chrono::steady_clock::time_point timestamp) noexcept
  {
    return EnqueueWithTimestamp(key, byteCount,
                                [timestamp]() noexcept { return timestamp; });
  }

  bool Dequeue(const void* key, std::uint64_t byteCount = 0) noexcept
  {
    return DequeueWithTimestamp(key, byteCount,
                                []() noexcept { return Clock::now(); });
  }

  bool DequeueAt(const void* key, std::uint64_t byteCount,
                 std::chrono::steady_clock::time_point timestamp) noexcept
  {
    return DequeueWithTimestamp(key, byteCount,
                                [timestamp]() noexcept { return timestamp; });
  }

private:
  template <typename TimestampProvider>
  bool EnqueueWithTimestamp(const void* key, std::uint64_t byteCount,
                            TimestampProvider&& timestampProvider) noexcept
  {
    Increment(Enqueued);
    AddGauge(Entries, 1);
    if (Bytes != Gauge::Count)
    {
      Increment(EnqueuedBytes, byteCount);
      AddGauge(Bytes, static_cast<std::int64_t>(byteCount));
    }
    if (!ShouldSample(Residence))
      return true;
    if (Samples.TryRecord(key, timestampProvider()))
      return true;
    Increment(Counter::SampleTableOverflows, 1);
    return false;
  }

  template <typename TimestampProvider>
  bool DequeueWithTimestamp(const void* key, std::uint64_t byteCount,
                            TimestampProvider&& timestampProvider) noexcept
  {
    Increment(Dequeued);
    AddGauge(Entries, -1);
    if (Bytes != Gauge::Count)
    {
      Increment(DequeuedBytes, byteCount);
      AddGauge(Bytes, -static_cast<std::int64_t>(byteCount));
    }

    std::chrono::steady_clock::time_point enqueuedAt{};
    if (!Samples.Take(key, enqueuedAt))
      return false;
    const auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(
        timestampProvider() - enqueuedAt).count();
    ObserveNanoseconds(Residence,
        elapsed > 0 ? static_cast<std::uint64_t>(elapsed) : 0);
    return true;
  }

  Counter Enqueued;
  Counter Dequeued;
  Gauge Entries;
  Timing Residence;
  Counter EnqueuedBytes;
  Counter DequeuedBytes;
  Gauge Bytes;
  FixedSampleTable<Capacity> Samples;
};

class OutcomeCounterGuard
{
public:
  OutcomeCounterGuard(Counter accepted, Counter rejected) noexcept
      : Accepted(accepted), Rejected(rejected), Pending(true)
  {
  }

  ~OutcomeCounterGuard()
  {
    if (Pending)
      Increment(Rejected);
  }

  OutcomeCounterGuard(const OutcomeCounterGuard&) = delete;
  OutcomeCounterGuard& operator=(const OutcomeCounterGuard&) = delete;

  void MarkAccepted() noexcept
  {
    if (!Pending)
      return;
    Increment(Accepted);
    Pending = false;
  }

private:
  Counter Accepted;
  Counter Rejected;
  bool Pending;
};

class SampledTimer
{
public:
  explicit SampledTimer(Timing timing) noexcept
      : TimingId(timing), Active(ShouldSample(timing)),
        Started(Active ? std::chrono::steady_clock::now()
                       : std::chrono::steady_clock::time_point())
  {
  }

  ~SampledTimer() { Stop(); }

  SampledTimer(const SampledTimer&) = delete;
  SampledTimer& operator=(const SampledTimer&) = delete;

  void Stop() noexcept
  {
    if (!Active)
      return;
    const auto elapsed = std::chrono::steady_clock::now() - Started;
    const auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
    ObserveNanoseconds(TimingId,
        nanoseconds > 0 ? static_cast<std::uint64_t>(nanoseconds) : 0);
    Active = false;
  }

private:
  Timing TimingId;
  bool Active;
  std::chrono::steady_clock::time_point Started;
};

inline std::uint64_t ReadCounter(Counter counter) noexcept
{
  return Metrics.counters[Index(counter)].load(std::memory_order_relaxed);
}
inline std::int64_t ReadGauge(Gauge gauge) noexcept
{
  return Metrics.gauges[Index(gauge)].load(std::memory_order_relaxed);
}
inline std::uint64_t ReadGaugeHighWater(Gauge gauge) noexcept
{
  return Metrics.highWater[Index(gauge)].load(std::memory_order_relaxed);
}

inline Snapshot Capture() noexcept
{
  Snapshot snapshot;
  for (std::size_t i = 0; i < CounterCount; ++i)
    snapshot.counters[i] = Metrics.counters[i].load(std::memory_order_relaxed);
  for (std::size_t i = 0; i < GaugeCount; ++i)
  {
    snapshot.gauges[i] = Metrics.gauges[i].load(std::memory_order_relaxed);
    snapshot.highWater[i] = Metrics.highWater[i].load(std::memory_order_relaxed);
  }
  for (std::size_t i = 0; i < TimingCount; ++i)
  {
    snapshot.timings[i].samples = Metrics.timings[i].samples.load(std::memory_order_relaxed);
    for (std::size_t j = 0; j < HistogramBucketCount; ++j)
      snapshot.timings[i].buckets[j] =
          Metrics.timings[i].buckets[j].load(std::memory_order_relaxed);
  }
  return snapshot;
}

inline const char* CounterName(Counter counter) noexcept
{
  static const char* const names[CounterCount] = {
      "raw_receive_calls", "raw_frames", "raw_bytes", "ng_frames", "ng_bytes",
      "non_ng_frames", "rx_truncated", "rx_too_short", "sar_receive_calls",
      "sar_fragments_accepted", "sar_rejected_frames", "sar_completions",
      "network_enqueued", "network_enqueued_bytes", "network_dequeued",
      "network_dequeued_bytes", "network_rejected", "sample_table_overflows",
      "network_new_message_failures",
      "network_deserialize_failures", "input_admissions", "input_rejected",
      "input_dispatches", "input_completions", "output_admissions", "output_dequeued",
      "output_retries", "output_completions", "shm_read_polls",
      "shm_read_segments_probed", "shm_read_semaphore_busy", "shm_read_semaphore_errors",
      "shm_read_flag_ready", "shm_read_messages", "shm_read_bytes",
      "shm_read_decode_errors", "shm_read_new_message_failures",
      "shm_read_attach_errors", "shm_read_open_errors", "shm_read_post_errors",
      "shm_read_segment_errors", "shm_read_detach_errors",
      "shm_write_calls", "shm_write_segments_probed", "shm_write_semaphore_busy",
      "shm_write_semaphore_errors", "shm_write_flag_busy", "shm_write_messages",
      "shm_write_bytes", "shm_write_failures", "shm_write_open_errors",
      "shm_write_attach_errors", "shm_write_post_errors", "shm_write_lookup_errors",
      "shm_write_capacity_errors", "shm_write_detach_errors",
      "shm_read_semaphore_acquisitions", "shm_write_semaphore_acquisitions"};
  const std::size_t index = Index(counter);
  return index < CounterCount ? names[index] : "invalid_counter";
}

inline const char* GaugeName(Gauge gauge) noexcept
{
  static const char* const names[GaugeCount] = {
      "sar_active_assemblies", "sar_active_bytes", "network_queue_entries",
      "network_queue_bytes", "input_queue_entries", "output_queue_entries"};
  const std::size_t index = Index(gauge);
  return index < GaugeCount ? names[index] : "invalid_gauge";
}

inline const char* TimingName(Timing timing) noexcept
{
  static const char* const names[TimingCount] = {
      "raw_receive_call", "raw_frame_validation", "sar_receive_fragment",
      "network_producer_lock_wait", "network_producer_lock_hold",
      "network_consumer_lock_wait", "network_consumer_lock_hold",
      "network_residence", "network_copy", "gateway_message_create",
      "gateway_deserialize", "input_queue_lock_wait", "input_queue_lock_hold",
      "input_residence", "scheduler_lateness", "action_service",
      "shm_read_semaphore_wait", "shm_read_operation",
      "shm_write_semaphore_wait", "shm_write_operation"};
  const std::size_t index = Index(timing);
  return index < TimingCount ? names[index] : "invalid_timing";
}

inline std::string FormatSnapshot(const Snapshot& snapshot,
                                 const std::string& run,
                                 int pid,
                                 std::int64_t elapsedSeconds,
                                 bool final,
                                 const char* drainState = "running")
{
  std::ostringstream line;
  line << (final ? "SPEC027_STAGE_FINAL" : "SPEC027_STAGE_STATS")
       << " run=" << run << " pid=" << pid
       << " elapsed_s=" << elapsedSeconds
       << " phase=" << (final ? "final" : "periodic")
       << " drain_state=" << (drainState ? drainState : "unknown");
  for (std::size_t i = 0; i < CounterCount; ++i)
    line << ' ' << CounterName(static_cast<Counter>(i)) << '=' << snapshot.counters[i];
  for (std::size_t i = 0; i < GaugeCount; ++i)
    line << ' ' << GaugeName(static_cast<Gauge>(i)) << '=' << snapshot.gauges[i]
         << ' ' << GaugeName(static_cast<Gauge>(i)) << "_high=" << snapshot.highWater[i];
  for (std::size_t i = 0; i < TimingCount; ++i)
  {
    const HistogramSnapshot& histogram = snapshot.timings[i];
    const Timing timing = static_cast<Timing>(i);
    const auto p50 = histogram.QuantileBoundsNanoseconds(50);
    const auto p95 = histogram.QuantileBoundsNanoseconds(95);
    const auto p99 = histogram.QuantileBoundsNanoseconds(99);
    line << ' ' << TimingName(timing) << "_samples=" << histogram.samples
         << ' ' << TimingName(timing) << "_p50_low_ns=" << p50.first
         << ' ' << TimingName(timing) << "_p50_high_ns=" << p50.second
         << ' ' << TimingName(timing) << "_p95_low_ns=" << p95.first
         << ' ' << TimingName(timing) << "_p95_high_ns=" << p95.second
         << ' ' << TimingName(timing) << "_p99_low_ns=" << p99.first
         << ' ' << TimingName(timing) << "_p99_high_ns=" << p99.second;
  }
  return line.str();
}

inline void ResetForTests() noexcept
{
  for (std::size_t i = 0; i < CounterCount; ++i)
    Metrics.counters[i].store(0, std::memory_order_relaxed);
  for (std::size_t i = 0; i < GaugeCount; ++i)
  {
    Metrics.gauges[i].store(0, std::memory_order_relaxed);
    Metrics.highWater[i].store(0, std::memory_order_relaxed);
  }
  for (std::size_t i = 0; i < TimingCount; ++i)
  {
    Metrics.timings[i].samples.store(0, std::memory_order_relaxed);
    for (std::size_t j = 0; j < HistogramBucketCount; ++j)
      Metrics.timings[i].buckets[j].store(0, std::memory_order_relaxed);
  }
}

inline void ResetSamplerForTests() noexcept
{
  SampleSequence.fill(0);
}
} // namespace NGALStageTelemetry

#define NGAL_STAGE_INCREMENT(counter, amount) \
  ::NGALStageTelemetry::Increment((counter), (amount))
#define NGAL_STAGE_GAUGE_ADD(gauge, delta) \
  ::NGALStageTelemetry::AddGauge((gauge), (delta))
#define NGAL_STAGE_GAUGE_SET(gauge, value) \
  ::NGALStageTelemetry::SetGauge((gauge), (value))
#define NGAL_STAGE_OBSERVE_NS(timing, nanoseconds) \
  ::NGALStageTelemetry::ObserveNanoseconds((timing), (nanoseconds))
#define NGAL_STAGE_SHOULD_SAMPLE(timing) \
  ::NGALStageTelemetry::ShouldSample((timing))
#define NGAL_STAGE_TIMER_START(name, timing) \
  ::NGALStageTelemetry::SampledTimer name((timing))
#define NGAL_STAGE_TIMER_STOP(name) (name).Stop()
#define NGAL_STAGE_SEM_TRYWAIT(semaphore, busyCounter, errorCounter) \
  ::NGALStageTelemetry::RunSemaphoreTryWait( \
      [&]() { return sem_trywait(semaphore); }, (busyCounter), (errorCounter))

#else

#define NGAL_STAGE_INCREMENT(counter, amount) do { } while (0)
#define NGAL_STAGE_GAUGE_ADD(gauge, delta) do { } while (0)
#define NGAL_STAGE_GAUGE_SET(gauge, value) do { } while (0)
#define NGAL_STAGE_OBSERVE_NS(timing, nanoseconds) do { } while (0)
#define NGAL_STAGE_SHOULD_SAMPLE(timing) false
#define NGAL_STAGE_TIMER_START(name, timing) do { } while (0)
#define NGAL_STAGE_TIMER_STOP(name) do { } while (0)
#define NGAL_STAGE_SEM_TRYWAIT(semaphore, busyCounter, errorCounter) \
  sem_trywait(semaphore)

#endif // SPEC027_STAGE_DIAGNOSTICS

#endif // _NGAL_STAGE_TELEMETRY_H
