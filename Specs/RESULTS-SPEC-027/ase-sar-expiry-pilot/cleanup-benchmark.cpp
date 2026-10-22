#include "NGAL_SAR.h"

#include <chrono>
#include <cstdio>
#include <iostream>
#include <vector>

namespace
{
std::vector<unsigned char> FirstFragment(unsigned int message_number)
{
  std::vector<unsigned char> fragment(24, 0);
  NGAL_SAR::BuildHeaderSegmentationField(fragment.data(), message_number, 0);
  NGAL_SAR::BuildHeaderSizeField(fragment.data() + 8, 24);
  return fragment;
}
}

int main()
{
  if (std::freopen("/dev/null", "w", stderr) == nullptr)
    return 3;
  constexpr unsigned int buffer_count = 256;
  constexpr unsigned int sweeps = 1000000;
  NGAL_SAR sar;

  for (unsigned int id = 1; id <= buffer_count; ++id)
  {
    const std::vector<unsigned char> fragment = FirstFragment(id);
    char* completed = 0;
    long long completed_size = 0;
    if (sar.ReceiveFragment(const_cast<unsigned char*>(fragment.data()),
                            static_cast<unsigned int>(fragment.size()), 16,
                            completed, completed_size) != 1 ||
        completed != 0 || completed_size != 0)
      return 2;
  }

  const auto started = std::chrono::steady_clock::now();
  for (unsigned int i = 0; i < sweeps; ++i)
  {
#ifdef NGAL_SAR_BENCH_CURRENT
    sar.CleanupTimedOut();
#else
    const double threshold = std::chrono::duration<double>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    sar.CleanupTimedOut(threshold);
#endif
  }
  const auto elapsed = std::chrono::steady_clock::now() - started;
  const double ns_per_sweep =
      std::chrono::duration<double, std::nano>(elapsed).count() / sweeps;
  std::cout << ns_per_sweep << '\n';
  return 0;
}
