#include "NGAL_SAR.h"

#include <chrono>
#include <ctime>
#include <iostream>
#include <vector>

namespace
{
std::vector<unsigned char> MakeFragment(unsigned int sequence_number,
                                        unsigned int payload_size)
{
  const unsigned int header_size = sequence_number == 0 ? 16 : 8;
  std::vector<unsigned char> fragment(header_size + payload_size, 0);
  NGAL_SAR::BuildHeaderSegmentationField(fragment.data(), 7, sequence_number);
  if (sequence_number == 0)
    NGAL_SAR::BuildHeaderSizeField(fragment.data() + 8, 24);
  return fragment;
}
}

int main()
{
  const double monotonic_seconds = std::chrono::duration<double>(
      std::chrono::steady_clock::now().time_since_epoch()).count();
  const double wall_seconds = static_cast<double>(std::time(0));
  if (wall_seconds <= monotonic_seconds + 31.0)
    return 77;

  NGAL_SAR sar;
  char* completed = 0;
  long long completed_size = 0;
  const std::vector<unsigned char> first = MakeFragment(0, 8);
  if (sar.ReceiveFragment(const_cast<unsigned char*>(first.data()),
                          static_cast<unsigned int>(first.size()), 16,
                          completed, completed_size) != 1)
    return 2;

  sar.CleanupTimedOut(monotonic_seconds + 31.0);
  const std::vector<unsigned char> second = MakeFragment(1, 16);
  const int status = sar.ReceiveFragment(
      const_cast<unsigned char*>(second.data()),
      static_cast<unsigned int>(second.size()), 16, completed, completed_size);
  delete[] completed;
  if (status == 0)
  {
    std::cerr << "RED: wall-clock timestamp survived monotonic cleanup threshold"
              << std::endl;
    return 1;
  }
  std::cout << "PASS: expired SAR did not complete" << std::endl;
  return 0;
}
