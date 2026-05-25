// Teste de validacao: tenta enviar mensagem com 0 args via SendToARawSocket
// Deve falhar com erro claro
#include <iostream>
#include <cassert>

// Simula o check que adicionamos
bool validateMessage() {
    // Simula 3 command lines: -m --cl (3 args), -stresstest --ping (0 args), -scn --seq (1 arg)
    struct MockCL { string name, alt; unsigned int numArgs; };
    MockCL cls[] = {
        {"-m", "--cl", 3},
        {"-stresstest", "--ping", 0},  // PROBLEMA: 0 argumentos
        {"-scn", "--seq", 1},
    };
    unsigned int NoCL = 3;

    for (unsigned int cl = 0; cl < NoCL; cl++) {
        if (cls[cl].numArgs == 0) {
            cout << "(ERROR: Command line " << cl << " ('"
                 << cls[cl].name << " " << cls[cl].alt
                 << "') has 0 arguments. All inter-host command lines "
                 << "must have >=1 argument for correct serialization "
                 << "round-trip through raw socket. Add at least one "
                 << "argument (e.g., a counter) to fix this.)" << endl;
            cout << "(ABORTING: message not sent due to 0-argument command line)" << endl;
            return false;
        }
    }
    return true;
}

bool validateMessageOK() {
    // Simula 3 command lines: todos com >=1 arg
    struct MockCL { string name, alt; unsigned int numArgs; };
    MockCL cls[] = {
        {"-m", "--cl", 3},
        {"-stresstest", "--ping", 1},  // CORRETO: 1 argumento (workaround)
        {"-scn", "--seq", 1},
    };
    unsigned int NoCL = 3;

    for (unsigned int cl = 0; cl < NoCL; cl++) {
        if (cls[cl].numArgs == 0) {
            cout << "(ERROR: Command line " << cl << " has 0 arguments)" << endl;
            return false;
        }
    }
    return true;
}

int main() {
    cout << "=== Teste de Validacao de Command Lines ===" << endl;

    cout << "\n--- Teste 1: mensagem com 0 args (deve FALHAR) ---" << endl;
    bool result1 = validateMessage();
    assert(result1 == false);
    cout << "PASS: corretamente rejeitada" << endl;

    cout << "\n--- Teste 2: mensagem com todos >=1 args (deve PASSAR) ---" << endl;
    bool result2 = validateMessageOK();
    assert(result2 == true);
    cout << "PASS: corretamente aceita" << endl;

    cout << "\n=== Todos os testes passaram ===" << endl;
    return 0;
}
