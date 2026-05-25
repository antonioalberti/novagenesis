// Teste completo de round-trip de serializacao de CommandLine
// Simula: operator<< -> ConvertCommandLineFromCharArray
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <tuple>

using namespace std;

// Simula o operator<< da CommandLine
string serializeCL(const string& name, const string& alt, const string& ver,
                   const vector<vector<string>>& args) {
    ostringstream os;
    os << "ng " << name << " " << alt << " " << ver;
    if (args.size() > 0) {
        os << " [";
        for (const auto& arg : args) {
            os << " < " << arg.size() << " s";
            for (const auto& e : arg) os << " " << e;
            os << " >";
        }
        os << " ]\n";
    } else {
        os << "\n";
    }
    return os.str();
}

struct ParsedCL {
    string Name, Alternative, Version;
    vector<vector<string>> Arguments;
    int Status = -1;
};

// Replica EXATAMENTE ConvertCommandLineFromCharArray
ParsedCL parseCL(const char* _CL, int _Size) {
    ParsedCL result;
    bool HasNG=false, HasCM=false, HasBegin=false, HasEnd=false;
    int AltMarkerPos=0;
    int WSPos[4096];
    int WSCounter=0;
    memset(WSPos, 0, sizeof(WSPos));

    if (_Size <= 9) return result;
    if (!(_CL[0]=='n' && _CL[1]=='g' && _CL[3]=='-')) return result;

    HasNG=true; HasCM=true;
    for (int i=0; i<(_Size-1); i++) {
        if (_CL[i]==' ') WSPos[WSCounter++]=i;
        if (_CL[i]=='-' && _CL[i+1]=='-') AltMarkerPos=i;
        if (_CL[i]=='[') HasBegin=true;
        if (_CL[i+1]==']') HasEnd=true;
        if (_CL[i]=='<') ;
        if (_CL[i]=='>') ;
    }

    if (!(HasNG && HasCM && HasBegin && HasEnd && AltMarkerPos>0 && WSCounter>3))
        return result;

    string* Words = new string[WSCounter + 1];

    for (int j=0; j<(WSCounter-1); j++) {
        int B=WSPos[j], E=WSPos[j+1];
        if (E>B && B>0)
            for (int k=B+1; k<E; k++)
                Words[j] += _CL[k];
    }

    for (int l=0; l<WSCounter+1; l++) {
        if (l==0) result.Name=Words[0];
        if (l==1) result.Alternative=Words[1];
        if (l==2) result.Version=Words[2];
        result.Status=0;

        if (Words[l]=="<") {
            if (l+1>WSCounter) continue;
            istringstream ss(Words[l+1]);
            int VS=0; ss>>VS;
            if (VS>0 && VS<4096) {
                vector<string> newArg;
                for (int m=0; m<VS; m++) {
                    int wi=l+3+m;
                    if (wi<=WSCounter) {
                        if (Words[wi]!="" && Words[wi]!=">" && Words[wi]!="<" &&
                            Words[wi]!="]" && Words[wi]!=" ") {
                            newArg.push_back(Words[wi]);
                        }
                    }
                }
                result.Arguments.push_back(newArg);
            }
        }
    }

    delete[] Words;
    return result;
}

int pass_count = 0, fail_count = 0;

void check(bool cond, const string& msg) {
    if (!cond) {
        cerr << "  FAIL: " << msg << endl;
        fail_count++;
    }
}

// Testa round-trip de UMA command line
void testSingle(const string& label, const string& name, const string& alt,
                const string& ver, const vector<vector<string>>& args) {
    string serialized = serializeCL(name, alt, ver, args);
    ParsedCL result = parseCL(serialized.c_str(), serialized.size());

    int before = fail_count;
    check(result.Status == 0, label + " - parsing failed");
    check(result.Name == name, label + " - Name '" + result.Name + "' != '" + name + "'");
    check(result.Alternative == alt, label + " - Alt mismatch");
    check(result.Version == ver, label + " - Ver mismatch");
    check(result.Arguments.size() == args.size(), label + " - Arg count " + to_string(result.Arguments.size()) + " != " + to_string(args.size()));

    for (size_t i=0; i<result.Arguments.size() && i<args.size(); i++) {
        check(result.Arguments[i].size() == args[i].size(),
               label + " - Arg[" + to_string(i) + "] size " + to_string(result.Arguments[i].size()) + " != " + to_string(args[i].size()));
        for (size_t j=0; j<result.Arguments[i].size() && j<args[i].size(); j++) {
            check(result.Arguments[i][j] == args[i][j],
                   label + " - Arg[" + to_string(i) + "][" + to_string(j) + "] '" + result.Arguments[i][j] + "' != '" + args[i][j] + "'");
        }
    }

    if (fail_count == before) {
        cout << label << ": PASS" << endl;
        pass_count++;
    } else {
        cout << label << ": FAIL" << endl;
    }
}

// Testa round-trip de mensagem com MULTIPLAS command lines
struct CLSpec {
    string name, alt, ver;
    vector<vector<string>> args;
};

void testMulti(const string& label, const vector<CLSpec>& specs) {
    // Serialize all CLs into one buffer
    string buffer;
    for (const auto& s : specs) {
        buffer += serializeCL(s.name, s.alt, s.ver, s.args);
    }
    buffer += "\n";

    cout << "\n--- " << label << " ---" << endl;
    cout << "Buffer: [" << buffer << "]" << endl;

    // Parse each "ng -..." line
    vector<ParsedCL> parsed;
    int t = 0;
    while (t < (int)buffer.size()) {
        if (t+3 < (int)buffer.size() && buffer[t]=='n' && buffer[t+1]=='g' && buffer[t+2]==' ' && buffer[t+3]=='-') {
            int u = t;
            while (u < (int)buffer.size() && buffer[u] != '\n') u++;
            int lineSize = u - t + 1;
            char* temp = new char[lineSize];
            for (int v = 0; v < u-t; v++) temp[v] = buffer[t+v];
            ParsedCL cl = parseCL(temp, lineSize);
            delete[] temp;
            parsed.push_back(cl);
            t = u + 1;
        } else {
            t++;
        }
    }

    int before = fail_count;
    check(parsed.size() == specs.size(), "parsed " + to_string(parsed.size()) + " CLs, expected " + to_string(specs.size()));

    for (size_t i = 0; i < parsed.size() && i < specs.size(); i++) {
        cout << "  CL[" << i << "]: Name='" << parsed[i].Name << "' Alt='" << parsed[i].Alternative << "' Ver='" << parsed[i].Version << "' Args=" << parsed[i].Arguments.size() << endl;
        for (size_t j = 0; j < parsed[i].Arguments.size(); j++) {
            cout << "    Arg[" << j << "]: ";
            for (const auto& e : parsed[i].Arguments[j]) cout << "'" << e << "' ";
            cout << endl;
        }

        check(parsed[i].Name == specs[i].name, "CL[" + to_string(i) + "] Name mismatch");
        check(parsed[i].Alternative == specs[i].alt, "CL[" + to_string(i) + "] Alt mismatch");
        check(parsed[i].Version == specs[i].ver, "CL[" + to_string(i) + "] Ver mismatch");
        check(parsed[i].Arguments.size() == specs[i].args.size(), "CL[" + to_string(i) + "] arg count mismatch");

        for (size_t j=0; j<parsed[i].Arguments.size() && j<specs[i].args.size(); j++) {
            check(parsed[i].Arguments[j].size() == specs[i].args[j].size(),
                   "CL[" + to_string(i) + "] Arg[" + to_string(j) + "] size mismatch");
            for (size_t k=0; k<parsed[i].Arguments[j].size() && k<specs[i].args[j].size(); k++) {
                check(parsed[i].Arguments[j][k] == specs[i].args[j][k],
                       "CL[" + to_string(i) + "] Arg[" + to_string(j) + "][" + to_string(k) + "] '" + parsed[i].Arguments[j][k] + "' != '" + specs[i].args[j][k] + "'");
            }
        }
    }

    if (fail_count == before) {
        cout << label << ": PASS" << endl;
        pass_count++;
    } else {
        cout << label << ": FAIL" << endl;
    }
}

int main() {
    cout << "=== NG-004 Serialization Round-Trip Tests ===" << endl;

    // === Single command line tests ===
    cout << "\n--- Single Command Line Tests ---" << endl;

    testSingle("Hello IHC (1 arg, 9 elems)", "-hello", "--ihc", "0.1",
        {{"E8CE389C","00000001","00000011","A34AC3C8","GWSCN","HTSCN","Ethernet","eth0","08:00:27:65:00:08"}});

    testSingle("Stress ping (0 args)", "-stresstest", "--ping", "0.1", {});

    testSingle("Stress ping (1 arg)", "-stresstest", "--ping", "0.1", {{"42"}});

    testSingle("SCN (1 arg)", "-scn", "--seq", "0.1", {{"ABCD1234"}});

    testSingle("-m --cl (3 args: 1+1+1)", "-m", "--cl", "0.1",
        {{"15B239D1"}, {"E8CE389C"}, {"E8CE389C"}});

    testSingle("-m --cl (3 args: 1+4+4)", "-m", "--cl", "0.1",
        {{"15B239D1"}, {"E8CE389C","00000001","00000011","A34AC3C8"}, {"E8CE389C","00000001","00000011","A34AC3C8"}});

    testSingle("Single arg 1 elem", "-test", "--flag", "0.1", {{"ABC123"}});

    testSingle("Single arg 8 elems", "-p", "--notify", "0.1",
        {{"1","KEY","8","H1","H2","H3","H4","H5"}});

    // === Multi-command-line tests ===
    cout << "\n--- Multi Command Line Tests ---" << endl;

    // Hello message: -m --cl + -hello --ihc + -scn --seq
    testMulti("Hello IHC 0.1 (3 CLs)",
        {
            {"-m", "--cl", "0.1", {{"15B239D1"}, {"E8CE389C","00000001","00000011","A34AC3C8"}, {"FFFFFFFF","FFFFFFFF","FFFFFFFF","FFFFFFFF"}}},
            {"-hello", "--ihc", "0.1", {{"E8CE389C","00000001","00000011","A34AC3C8","GWSCN","HTSCN","Ethernet","eth0","08:00:27:65:00:08"}}},
            {"-scn", "--seq", "0.1", {{"ABCD1234"}}},
        });

    // Stress message without workaround (0 args on stress ping)
    testMulti("Stress ping (3 CLs, 0 args on stress)",
        {
            {"-m", "--cl", "0.1", {{"15B239D1"}, {"E8CE389C"}, {"A34AC3C8"}}},
            {"-stresstest", "--ping", "0.1", {}},
            {"-scn", "--seq", "0.1", {{"0"}}},
        });

    // Stress message with workaround (1 arg on stress ping)
    testMulti("Stress ping (3 CLs, 1 arg workaround)",
        {
            {"-m", "--cl", "0.1", {{"15B239D1"}, {"E8CE389C"}, {"A34AC3C8"}}},
            {"-stresstest", "--ping", "0.1", {{"42"}}},
            {"-scn", "--seq", "0.1", {{"0"}}},
        });

    // Edge: -m --cl with 4-elem dest + stress + SCN
    testMulti("IHC 4-elem dest + stress (3 CLs)",
        {
            {"-m", "--cl", "0.1", {{"15B239D1"}, {"E8CE389C","00000001","00000011","A34AC3C8"}, {"E8CE389C","00000001","00000011","A34AC3C8"}}},
            {"-stresstest", "--ping", "0.1", {{"1"}}},
            {"-scn", "--seq", "0.1", {{"0"}}},
        });

    cout << "\n=== Summary: " << pass_count << " passed, " << fail_count << " failed ===" << endl;
    return fail_count > 0 ? 1 : 0;
}
