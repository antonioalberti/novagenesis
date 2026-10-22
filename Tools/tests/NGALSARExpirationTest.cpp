#include "NGAL_SAR.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <vector>

namespace
{
using Clock = NGAL_SAR::Clock;
using TimePoint = NGAL_SAR::TimePoint;

struct FakeClock
{
  static TimePoint Current;

  static TimePoint Now()
  {
    return Current;
  }
};

TimePoint FakeClock::Current;
const char* ScheduledCleanupFailure = 0;

TimePoint AtSeconds(long long seconds)
{
  return TimePoint(std::chrono::seconds(seconds));
}

std::vector<unsigned char> MakeFragment(unsigned int message_number,
                                        unsigned int sequence_number,
                                        long long message_size,
                                        unsigned int payload_size)
{
  const unsigned int header_size = sequence_number == 0 ? 16 : 8;
  std::vector<unsigned char> fragment(header_size + payload_size, 0);
  NGAL_SAR::BuildHeaderSegmentationField(fragment.data(), message_number,
                                         sequence_number);
  if (sequence_number == 0)
    NGAL_SAR::BuildHeaderSizeField(fragment.data() + 8, message_size);
  return fragment;
}

int Submit(NGAL_SAR& sar,
           const std::vector<unsigned char>& fragment,
           long long& completed_size)
{
  char* completed_buffer = 0;
  completed_size = 0;
  const int status = sar.ReceiveFragment(
      const_cast<unsigned char*>(fragment.data()),
      static_cast<unsigned int>(fragment.size()), 16,
      completed_buffer, completed_size);
  delete[] completed_buffer;
  return status;
}

bool StartPartial(NGAL_SAR& sar,
                  unsigned int message_number,
                  long long message_size)
{
  long long completed_size = 0;
  return Submit(sar, MakeFragment(message_number, 0, message_size, 8),
                completed_size) == 1 && completed_size == 0;
}

bool TestIdleExpiryBoundary()
{
  long long completed_size = 0;
  FakeClock::Current = AtSeconds(1000);
  NGAL_SAR before(&FakeClock::Now);
  if (!StartPartial(before, 7, 24))
    return false;
  FakeClock::Current += std::chrono::milliseconds(29999);
  before.CleanupTimedOut();
  if (Submit(before, MakeFragment(7, 1, 24, 16), completed_size) != 0 ||
      completed_size != 24)
    return false;

  FakeClock::Current = AtSeconds(2000);
  NGAL_SAR at_boundary(&FakeClock::Now);
  if (!StartPartial(at_boundary, 8, 24))
    return false;
  FakeClock::Current += std::chrono::seconds(30);
  at_boundary.CleanupTimedOut();
  return Submit(at_boundary, MakeFragment(8, 1, 24, 16), completed_size) == 1 &&
         completed_size == 0;
}

bool TestExpiryBeforeContinuation()
{
  long long completed_size = 0;
  FakeClock::Current = AtSeconds(3000);
  NGAL_SAR sar(&FakeClock::Now);
  if (!StartPartial(sar, 9, 24))
    return false;

  FakeClock::Current += std::chrono::milliseconds(30001);
  return Submit(sar, MakeFragment(9, 1, 24, 16), completed_size) == 1 &&
         completed_size == 0;
}

bool TestDuplicateAndRejectedFragmentsDoNotRefreshIdle()
{
  long long completed_size = 0;
  FakeClock::Current = AtSeconds(4000);
  NGAL_SAR sar(&FakeClock::Now);
  if (!StartPartial(sar, 10, 24))
    return false;

  const unsigned long long duplicates_before =
      NGAL_SAR::Stats().Values[NGAL_SAR::DuplicateSN].load(
          std::memory_order_relaxed);
  FakeClock::Current += std::chrono::seconds(29);
  Submit(sar, MakeFragment(10, 0, 24, 8), completed_size);
  const unsigned long long duplicates_after =
      NGAL_SAR::Stats().Values[NGAL_SAR::DuplicateSN].load(
          std::memory_order_relaxed);
  if (duplicates_after != duplicates_before + 1)
    return false;

  Submit(sar, MakeFragment(10, 1, 24, 1), completed_size);
  FakeClock::Current += std::chrono::seconds(1);
  return Submit(sar, MakeFragment(10, 1, 24, 16), completed_size) == 1 &&
         completed_size == 0;
}

bool TestAcceptedProgressRefreshesIdle()
{
  long long completed_size = 0;
  FakeClock::Current = AtSeconds(5000);
  NGAL_SAR sar(&FakeClock::Now);
  if (!StartPartial(sar, 11, 40))
    return false;

  FakeClock::Current += std::chrono::seconds(29);
  if (Submit(sar, MakeFragment(11, 1, 40, 16), completed_size) != 1)
    return false;

  FakeClock::Current += std::chrono::seconds(29);
  sar.CleanupTimedOut();
  return Submit(sar, MakeFragment(11, 2, 40, 16), completed_size) == 0 &&
         completed_size == 40;
}

bool TestAbsoluteMaximumAge()
{
  const long long message_size = 104;
  long long completed_size = 0;
  FakeClock::Current = AtSeconds(6000);
  const TimePoint created = FakeClock::Current;
  NGAL_SAR sar(&FakeClock::Now);
  if (!StartPartial(sar, 12, message_size))
    return false;

  for (unsigned int sequence = 1; sequence <= 5; ++sequence)
  {
    FakeClock::Current = created + std::chrono::seconds(sequence * 29);
    if (Submit(sar, MakeFragment(12, sequence, message_size, 16),
               completed_size) != 1)
      return false;
  }

  const unsigned long long duplicates_before =
      NGAL_SAR::Stats().Values[NGAL_SAR::DuplicateSN].load(
          std::memory_order_relaxed);
  FakeClock::Current = created + std::chrono::milliseconds(149999);
  sar.CleanupTimedOut();
  Submit(sar, MakeFragment(12, 5, message_size, 16), completed_size);
  const unsigned long long duplicates_after =
      NGAL_SAR::Stats().Values[NGAL_SAR::DuplicateSN].load(
          std::memory_order_relaxed);
  if (duplicates_after != duplicates_before + 1)
    return false;

  FakeClock::Current = created + std::chrono::seconds(150);
  return Submit(sar, MakeFragment(12, 6, message_size, 16), completed_size) == 1 &&
         completed_size == 0;
}

bool TestAbsoluteMaximumAgeAfterBoundary()
{
  const long long message_size = 104;
  long long completed_size = 0;
  FakeClock::Current = AtSeconds(6500);
  const TimePoint created = FakeClock::Current;
  NGAL_SAR sar(&FakeClock::Now);
  if (!StartPartial(sar, 14, message_size))
    return false;

  for (unsigned int sequence = 1; sequence <= 5; ++sequence)
  {
    FakeClock::Current = created + std::chrono::seconds(sequence * 29);
    if (Submit(sar, MakeFragment(14, sequence, message_size, 16),
               completed_size) != 1)
      return false;
  }

  FakeClock::Current = created + std::chrono::milliseconds(150001);
  sar.CleanupTimedOut();
  return Submit(sar, MakeFragment(14, 6, message_size, 16), completed_size) == 1 &&
         completed_size == 0;
}

bool TestScheduledCleanupDuringContinuousTraffic()
{
  long long completed_size = 0;
  FakeClock::Current = AtSeconds(7000);
  NGAL_SAR sar(&FakeClock::Now);
  if (!StartPartial(sar, 13, 24))
    return false;

  const unsigned long long abandoned_before =
      NGAL_SAR::Stats().Values[NGAL_SAR::TimeoutAbandoned].load(
          std::memory_order_relaxed);
  unsigned int cleanup_calls = 0;
  for (unsigned int tick = 1; tick <= 60; ++tick)
  {
    FakeClock::Current += std::chrono::milliseconds(500);
    if (sar.MaybeCleanup())
      ++cleanup_calls;
    const unsigned int message_number = 100 + tick;
    if (Submit(sar, MakeFragment(message_number, 0, 8, 8),
               completed_size) != 0 || completed_size != 8)
      return false;
  }

  const unsigned long long abandoned_after =
      NGAL_SAR::Stats().Values[NGAL_SAR::TimeoutAbandoned].load(
          std::memory_order_relaxed);
  // A shorter schedule is compliant; assert the one-second upper bound only.
  return cleanup_calls >= 30 && abandoned_after == abandoned_before + 1;
}

bool TestScheduledCleanupWithUnalignedCalls()
{
  FakeClock::Current = AtSeconds(8000);
  const TimePoint schedule_start = FakeClock::Current;
  TimePoint last_sweep = schedule_start;
  NGAL_SAR sar(&FakeClock::Now);
  unsigned int cleanup_calls = 0;

  for (unsigned int tick = 1; tick <= 20; ++tick)
  {
    FakeClock::Current += std::chrono::milliseconds(600);
    if (!sar.MaybeCleanup())
      continue;

    if (FakeClock::Current - last_sweep > std::chrono::seconds(1))
    {
      ScheduledCleanupFailure =
          "scheduled cleanup interval exceeded one second at 600 ms call cadence";
      return false;
    }
    last_sweep = FakeClock::Current;
    ++cleanup_calls;
  }

  if (cleanup_calls < 2)
  {
    ScheduledCleanupFailure = "scheduled cleanup did not recur";
    return false;
  }
  return true;
}

bool TestScheduledCleanupWithIrregularCalls()
{
  const std::chrono::milliseconds counterexample_calls[] = {
      std::chrono::milliseconds(600),
      std::chrono::milliseconds(490),
      std::chrono::milliseconds(600)};

  FakeClock::Current = AtSeconds(9000);
  TimePoint last_sweep = FakeClock::Current;
  NGAL_SAR sar(&FakeClock::Now);
  unsigned int cleanup_calls = 0;
  for (const auto interval : counterexample_calls)
  {
    FakeClock::Current += interval;
    if (!sar.MaybeCleanup())
      continue;
    if (FakeClock::Current - last_sweep > std::chrono::seconds(1))
    {
      ScheduledCleanupFailure =
          "irregular 600/490/600 ms calls allowed a sweep gap over one second";
      return false;
    }
    last_sweep = FakeClock::Current;
    ++cleanup_calls;
  }
  if (cleanup_calls < 2)
  {
    ScheduledCleanupFailure = "irregular calls did not produce recurring sweeps";
    return false;
  }

  const std::chrono::milliseconds deadline_straddling_calls[] = {
      std::chrono::milliseconds(400),
      std::chrono::milliseconds(390),
      std::chrono::milliseconds(600)};
  FakeClock::Current = AtSeconds(9100);
  last_sweep = FakeClock::Current;
  NGAL_SAR straddling_sar(&FakeClock::Now);
  cleanup_calls = 0;
  for (const auto interval : deadline_straddling_calls)
  {
    FakeClock::Current += interval;
    if (!straddling_sar.MaybeCleanup())
      continue;
    if (FakeClock::Current - last_sweep > std::chrono::seconds(1))
    {
      ScheduledCleanupFailure =
          "deadline-straddling calls with a 600 ms maximum gap exceeded one second";
      return false;
    }
    last_sweep = FakeClock::Current;
    ++cleanup_calls;
  }
  if (cleanup_calls < 2)
  {
    ScheduledCleanupFailure = "deadline-straddling calls did not produce recurring sweeps";
    return false;
  }
  return true;
}
}

int main()
{
  std::ostringstream debug_output;
  std::streambuf* original_stderr = std::cerr.rdbuf(debug_output.rdbuf());
  const bool pass = TestIdleExpiryBoundary() &&
                    TestExpiryBeforeContinuation() &&
                    TestDuplicateAndRejectedFragmentsDoNotRefreshIdle() &&
                    TestAcceptedProgressRefreshesIdle() &&
                    TestAbsoluteMaximumAge() &&
                    TestAbsoluteMaximumAgeAfterBoundary() &&
                    TestScheduledCleanupDuringContinuousTraffic() &&
                    TestScheduledCleanupWithUnalignedCalls() &&
                    TestScheduledCleanupWithIrregularCalls();
  std::cerr.rdbuf(original_stderr);
  if (!pass)
  {
    if (ScheduledCleanupFailure != 0)
      std::cerr << "FAIL: " << ScheduledCleanupFailure << std::endl;
    else
      std::cerr << "FAIL: NGAL_SAR expiration contract" << std::endl;
    return 1;
  }
  std::cout << "PASS: SAR idle/max expiry, refresh semantics, continuation guard, "
               "and scheduled cleanup"
            << std::endl;
  return 0;
}
