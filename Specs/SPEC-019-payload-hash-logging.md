# SPEC-019: Payload Hash Logging para Traceabilidade

**Data:** 2026-07-11
**Estado:** Implementado — ContentApp e NRNCS
**Autor:** Hermes Agent
**Relacionada:** SPEC-014 (data race), SPEC-015 (getline corruption), SPEC-017 (loop sem break), SPEC-018 (ResetPayload), SPEC-020 (subscription re-delivery)

---

## 1. Objectivo

Adicionar logging do hash NG (MurmurHash3_x86_32, seed 3571) em cada ponto onde um payload é processado, para rastrear a integridade do conteúdo ao longo do pipeline sem precisar de verificação posterior.

## 2. Pipeline de Payload com Hash Logging

```ascii
Source (VM 102)                    Repo (VM 101)
───────────────                    ──────────────
ContentApp Source
  └─ Publica ficheiro (JPG, TXT)
       ↓ SHM via PGCS
NRNCS (Source)
  └─ NRInfoPayload01.cpp
     └─ (NRNCS forwarding payload: file=X, size=N, hash=H)  ← SPEC-019
       ↓ NGAL raw socket
NRNCS (Repo)
  └─ NRInfoPayload01.cpp
     └─ ResetPayload() + SetPayloadFromCharArray()           ← SPEC-018
       ↓ SHM via PGCS
ContentApp Repository
  └─ CoreInfoPayload01.cpp
     └─ (ContentApp received payload: file=X, size=N, hash=H)  ← SPEC-019
     └─ ConvertPayloadFromCharArrayToFile()
     └─ Marca subscription como "Delivered"                    ← SPEC-020
```

## 3. Pontos de Logging

| Ponto | Ficheiro | Status | Output |
|-------|----------|--------|--------|
| NRNCS (forwarding) | `NRNCS/src/NRInfoPayload01.cpp` | ✅ Implementado | `(NRNCS forwarding payload: file=X, size=N, hash=H)` |
| ContentApp (recebido) | `ContentApp/src/CoreInfoPayload01.cpp` | ✅ Implementado | `(ContentApp received payload: file=X, size=N, hash=H)` |

## 4. Implementação

### 4.1 ContentApp (já implementado, linhas 104-123)

```cpp
// SPEC-019: Log payload hash at ContentApp for traceability
{
    string PayloadHash;
    File F1;
    F1.OpenInputFile(Values.at(0), PayloadPath, "BINARY");
    F1.seekg(0, ios::end);
    long long PayloadSize = F1.tellg();
    F1.seekg(0);
    if (PayloadSize > 0) {
        char* payload_bytes = new char[PayloadSize];
        F1.read(payload_bytes, PayloadSize);
        PayloadHash = NameGenerator::GetInstance().GenerateFromCharArray(payload_bytes, PayloadSize);
        delete[] payload_bytes;
    }
    F1.CloseFile();
    PB->S << Offset << "(ContentApp received payload: file=" << Values.at(0)
          << ", size=" << PayloadSize << " bytes, hash=" << PayloadHash << ")" << endl;
}
```

Nota: Lê o ficheiro do disco **após** `ConvertPayloadFromCharArrayToFile()`. Isto verifica o que foi realmente escrito.

### 4.2 NRNCS (já implementado, linhas 92-100)

```cpp
// SPEC-019: Log payload hash at NRNCS for traceability
{
    string PayloadHash;
    unsigned char* payload_bytes = (unsigned char*)Payload;
    PayloadHash = NameGenerator::GetInstance().GenerateFromCharArray(
        (const char*)payload_bytes, Size);
    PB->S << Offset << "(NRNCS forwarding payload: file=" << Values.at(0)
          << ", size=" << Size << " bytes, hash=" << PayloadHash << ")" << endl;
}
```

Nota: Calcula hash do payload **in-memory** antes de copiar para o InlineResponseMessage. Não precisa de I/O.

## 5. Verificação nos Logs

Os logs do NRNCS e Repo confirmam que o hash logging funciona:

**NRNCS (source36):**
```
(NRNCS forwarding payload: file=Service_Offer_1046744630.txt, size=36 bytes, hash=789703CA)
```

**ContentApp (repo61):**
```
(ContentApp received payload: file=Service_Offer_1046744630.txt, size=36 bytes, hash=789703CA)
```

Hash `789703CA` consistente em ambos — o payload chega íntegro ao ContentApp.

## 6. Ficheiros Afectados

| Ficheiro | Acção | Razão |
|----------|-------|-------|
| `ContentApp/src/CoreInfoPayload01.cpp` | ✅ Já implementado | — |
| `NRNCS/src/NRInfoPayload01.cpp` | ✅ Já implementado | — |