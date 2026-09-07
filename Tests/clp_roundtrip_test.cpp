
#include "CommandLineParser.h"
#include "CommandLine.h"
#include "Message.h"
#include <iostream>
#include <sstream>
using namespace std;

int failures = 0;
void expect(bool ok, const string& what) {
  if (!ok) { cout << "FAIL: " << what << endl; failures++; }
  else cout << "PASS: " << what << endl;
}

int main() {
  // 1. Round-trip legacy format with type token
  {
    string in = "ng -msg --cl 0.1 [ < 3 s A B C > < 1 s X > ]";
    CommandLine CL; int ec; size_t off;
    expect(CommandLineParser::Parse(in, CL, ec, off) == OK, "parse legacy with s");
    stringstream ss; ss << CL;
    string out = ss.str();
    // writer emits same tokens (with type s)
    expect(out.find("< 3 s A B C >") != string::npos, "round-trip args preserved: " + out);
  }
  // 2. New format without type token parses
  {
    string in = "ng -msg --cl 0.1 [ < 3 A B C > ]";
    CommandLine CL; int ec; size_t off;
    expect(CommandLineParser::Parse(in, CL, ec, off) == OK, "parse new format (no type)");
    string v; CL.GetArgumentElement(0, 2, v);
    expect(v == "C", "third element = C");
  }
  // 3. Cardinality mismatch rejected
  {
    string in = "ng -a --b 0.1 [ < 3 s A > ]";
    CommandLine CL; int ec; size_t off;
    expect(CommandLineParser::Parse(in, CL, ec, off) != OK, "cardinality mismatch rejected");
    expect(ec == CLP_ERR_ELEMENTS, "error = ELEMENTS");
  }
  // 4. Untouched out on error
  {
    CommandLine CL("n","-a","0.1");
    string in = "ng -x --y 0.1 [ < 1 s A";
    int ec; size_t off;
    expect(CommandLineParser::Parse(in, CL, ec, off) != OK, "unclosed rejected");
    expect(CL.Name == "n" && CL.Alternative == "-a", "out untouched on error");
  }
  // 5. Element with marker char rejected
  {
    string in = "ng -a --b 0.1 [ < 1 s A<B > ]";
    CommandLine CL; int ec; size_t off;
    expect(CommandLineParser::Parse(in, CL, ec, off) != OK, "marker char in element rejected");
  }
  // 6. h and i type tokens accepted as legacy syntax
  {
    string in = "ng -a --b 0.1 [ < 1 h HASH > < 1 i 42 > ]";
    CommandLine CL; int ec; size_t off;
    expect(CommandLineParser::Parse(in, CL, ec, off) == OK, "legacy h/i accepted");
  }
  // 7. NoA == 0 form (writer emits without brackets)
  {
    string in = "ng -a --b 0.1";
    CommandLine CL; int ec; size_t off;
    expect(CommandLineParser::Parse(in, CL, ec, off) == OK, "NoA==0 accepted");
  }
  // 8. NoA==0 with brackets also accepted (divergence P9 closed)
  {
    string in = "ng -a --b 0.1 [ ]";
    CommandLine CL; int ec; size_t off;
    expect(CommandLineParser::Parse(in, CL, ec, off) == OK, "empty brackets accepted");
  }
  // 9. Oversized cardinality rejected (P8)
  {
    string in = "ng -a --b 0.1 [ < 2000000000 s A > ]";
    CommandLine CL; int ec; size_t off;
    expect(CommandLineParser::Parse(in, CL, ec, off) != OK, "oversized count rejected");
  }
  // 10. Trailing garbage rejected
  {
    string in = "ng -a --b 0.1 [ < 1 s A > ] junk";
    CommandLine CL; int ec; size_t off;
    expect(CommandLineParser::Parse(in, CL, ec, off) != OK, "trailing garbage rejected");
  }
  // 11. operator>> adapter works + failbit
  {
    istringstream ss("ng -a --b 0.1 [ < 2 s X Y > ]");
    CommandLine CL;
    ss >> CL;
    expect(!ss.fail(), "operator>> ok");
    string v; CL.GetArgumentElement(0, 1, v);
    expect(v == "Y", "operator>> elements correct");
  }
  // 12. Message-level round trip: serialize CL into message array, reparse
  {
    Message M(1.0, 0, false);
    CommandLine* P = NULL;
    M.NewCommandLine(P);
    istringstream ss("ng -cl --x 0.1 [ < 2 s A B > ]");
    ss >> *P;
    M.ConvertMessageFromCommandLinesandPayloadCharArrayToCharArray();
    char* Buf = NULL;
    M.GetMessageFromCharArray(Buf);
    Message M2(1.0, 0, false);
    M2.SetMessageFromCharArray(Buf, M.MessageSize);
    M2.ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2();
    CommandLine* Q = NULL;
    M2.GetCommandLine(0, Q);
    string v; Q->GetArgumentElement(0, 0, v);
    expect(v == "A", "message-level round trip preserved");
  }
  cout << (failures == 0 ? "ALL TESTS PASSED" : "FAILURES") << endl;
  return failures == 0 ? 0 : 1;
}
