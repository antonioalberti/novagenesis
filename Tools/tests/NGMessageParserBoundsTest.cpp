// SPEC-075 baseline-safe preservation controls V1 and V2.
#include "Message.h"

#include <cstring>
#include <iostream>
#include <string>

namespace {

bool CheckControl(const char* label, const std::string& wire,
                  const std::string& expected_payload) {
  Message message(0.0, 0, false);
  const int set_status = message.SetMessageFromCharArray(
      const_cast<char*>(wire.data()), static_cast<long long>(wire.size()));
  if (set_status != OK) {
    std::cerr << label << ": SetMessageFromCharArray did not return OK\n";
    return false;
  }

  const int parse_status =
      message.ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2();
  if (parse_status != OK) {
    std::cerr << label << ": parser did not return OK\n";
    return false;
  }

  unsigned int command_line_count = 0;
  if (message.GetNumberofCommandLines(command_line_count) != OK ||
      command_line_count != 1) {
    std::cerr << label << ": expected exactly one command line\n";
    return false;
  }

  CommandLine* command_line = nullptr;
  if (message.GetCommandLine(0, command_line) != OK || command_line == nullptr ||
      command_line->Name != "-x" || command_line->Alternative != "--y" ||
      command_line->Version != "0.1") {
    std::cerr << label << ": unexpected command line fields\n";
    return false;
  }

  if (!message.GetHasPayloadFlag()) {
    std::cerr << label << ": expected payload flag\n";
    return false;
  }

  long long payload_size = -1;
  if (message.GetPayloadSize(payload_size) != OK ||
      payload_size != static_cast<long long>(expected_payload.size())) {
    std::cerr << label << ": unexpected payload size\n";
    return false;
  }

  char* payload = nullptr;
  if (message.GetPayloadFromCharArray(payload) != OK || payload == nullptr ||
      std::memcmp(payload, expected_payload.data(), expected_payload.size()) !=
          0) {
    std::cerr << label << ": unexpected payload bytes\n";
    return false;
  }

  long long message_size = -1;
  char* message_bytes = nullptr;
  if (message.GetMessageSize(message_size) != OK ||
      message_size != static_cast<long long>(wire.size()) ||
      message.GetMessageFromCharArray(message_bytes) != OK ||
      message_bytes == nullptr ||
      std::memcmp(message_bytes, wire.data(), wire.size()) != 0) {
    std::cerr << label << ": serialized input changed\n";
    return false;
  }

  return true;
}

bool CheckPartial(const char* label, const std::string& wire,
                  const std::string& expected_payload) {
  Message message(0.0, 0, false);
  if (message.SetMessageFromCharArray(
          const_cast<char*>(wire.data()), static_cast<long long>(wire.size())) !=
      OK) {
    std::cerr << label << ": setter failed\n";
    return false;
  }
  if (message.ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2() !=
      OK) {
    std::cerr << label << ": parser did not return OK\n";
    return false;
  }
  unsigned int count = 0;
  CommandLine* command_line = nullptr;
  if (message.GetNumberofCommandLines(count) != OK || count != 1 ||
      message.GetCommandLine(0, command_line) != OK || command_line == nullptr ||
      command_line->Name != "-x" || command_line->Alternative != "--y" ||
      command_line->Version != "0.1") {
    std::cerr << label << ": unexpected command-line state\n";
    return false;
  }
  long long payload_size = -1;
  if (!message.GetHasPayloadFlag() || message.GetPayloadSize(payload_size) != OK ||
      payload_size != static_cast<long long>(expected_payload.size())) {
    std::cerr << label << ": unexpected payload metadata\n";
    return false;
  }
  char* payload = nullptr;
  const int payload_status = message.GetPayloadFromCharArray(payload);
  if (expected_payload.empty()) {
    if (payload_status != ERROR) {
      std::cerr << label << ": expected empty payload pointer status ERROR\n";
      return false;
    }
  } else if (payload_status != OK || payload == nullptr ||
             std::memcmp(payload, expected_payload.data(), expected_payload.size()) !=
                 0) {
    std::cerr << label << ": unexpected payload bytes\n";
    return false;
  }
  long long message_size = -1;
  char* message_bytes = nullptr;
  if (message.GetMessageSize(message_size) != OK ||
      message_size != static_cast<long long>(wire.size()) ||
      message.GetMessageFromCharArray(message_bytes) != OK || message_bytes == nullptr ||
      std::memcmp(message_bytes, wire.data(), wire.size()) != 0) {
    std::cerr << label << ": serialized input changed\n";
    return false;
  }
  return true;
}

}  // namespace

int main() {
  const std::string command = "ng -x --y 0.1 [ ]\n";
  const bool p1 = CheckPartial("P1", command + "n", "");
  const bool p2 = CheckPartial("P2", command + "ng", "g");
  const bool p3 = CheckPartial("P3", command + "ng ", "g ");
  const bool v1 = CheckControl("V1", command + "\npayload", "payload");
  const bool v2 = CheckControl("V2", command + "\npayload ng ", "payload ng ");
  if (!p1 || !p2 || !p3 || !v1 || !v2) {
    return 1;
  }
  std::cout << "PASS: SPEC-075 P1/P2/P3 and V1/V2 candidate contract\n";
  return 0;
}
