# SPEC-022: NRInfoPayload01 — Cache Local, Não Reencaminhar

**Data:** 2026-07-15
**Estado:** Corrigida (modelo NG correcto)
**Autor:** Hermes Agent + Antonio Alberti
**Relacionada:** SPEC-021 (NRSubBind01), HTGetBind01 (cat=18)

---

## 1. Problema Original (ANULADO)

A versão anterior desta SPEC assumia que o `NRInfoPayload01` deveria criar uma mensagem separada com `-d --b` + payload e reencaminhá-la directamente para o destino. **Esta premissa estava errada** e viola o modelo pub/sub invertido do NovaGenesis.

## 2. Modelo NG Correcto (Pub/Sub Invertido vs MQTT)

No NovaGenesis, ao contrário do MQTT onde o broker faz push do conteúdo para os subscritores, o fluxo é:

```
Publicador (ContentApp Source)
  │
  ├─ 1. Publica conteúdo com -p --notify
  │      O -p --notify inclui -info --payload com o payload
  │
  ▼
NRNCS Source (caching node)
  │
  ├─ 2. NRPubNotify01: armazena binding (cat=18, hash→nome)
  │      no HT local. Apenas o nome, NÃO o conteúdo.
  │
  ├─ 3. NRInfoPayload01: EXTRAI o Payload[] da mensagem
  │      recebida e GUARDA EM DISCO no path do NRNCS
  │      (IO/NRNCS/<nome_ficheiro>).
  │      NÃO cria -d --b. NÃO reencaminha.
  │      Apenas cache.
  │
  ├─ 4. NRPubNotify01: envia ng -notify ao Repo
  │      informando que o conteúdo está disponível.
  │
  ▼
Repo (subscritor)
  │
  ├─ 5. Recebe notificação → submete ng -s --b
  │
  ▼
NRNCS Source (atende subscrição)
  │
  ├─ 6. NRSubBind01 → ng -g --b ao HT
  │
  ├─ 7. HTGetBind01 (cat=18):
  │      ├─ Lê binding → obtém nome do ficheiro
  │      ├─ Lê ficheiro do disco (IO/NRNCS/<nome>)
  │      ├─ Cria InlineResponseMessage com:
  │      │   - -d --b (delivery)
  │      │   - -info --payload + payload
  │      │   - -scn --seq (do -g --b)
  │      └─ Devolve ao NRSubBind01
  │
  └─ 8. NRSubBind01 reencaminha para o Repo
```

**Princípio fundamental:** O `-info --payload` que chega no `-p --notify` serve **apenas para fazer cache local**. A entrega ao subscritor (`-d --b`) só acontece quando o subscritor faz `ng -s --b` e o `HTGetBind01` serve o conteúdo do cache.

## 3. O Erro das SPECs Anteriores

As SPECs 021 e 022 (versão anterior) foram escritas assumindo que:
- O `NRInfoPayload01` deveria criar `-d --b` e reencaminhar
- O `HTGetBind01` para cat=18 não era o caminho principal

Isto quebrou o modelo NG porque:
1. O conteúdo era reencaminhado antes de o subscritor subscrever
2. O `HTGetBind01` (cat=18) tentava ler o ficheiro do disco mas o ficheiro nunca era guardado — resultando em `HasPayloadFlag=true` com `PayloadSize=0`
3. O alarme "ficheiro não encontrado" nunca era emitido

## 4. Correcção

### 4.1 NRInfoPayload01 — Cache em disco

Em `NRNCS/src/NRInfoPayload01.cpp`:

| Antes (forwarding) | Depois (cache) |
|---|---|
| Extraía payload e criava nova msg com `-d --b` | Extrai payload e guarda em disco |
| Usava `PGW->PushToInputQueue()` | Usa `ConvertPayloadFromCharArrayToFile()` |
| Adicionava `-scn --s` | Apenas cache; sem CLs extra |

**Código implementado:**

```cpp
// Cache the payload to disk in the NRNCS path
CachePath = PB->GetPath();

// Check if file already exists (idempotent)
File F;
if (F.OpenInputFile(Values.at(0), CachePath, "BINARY") == OK)
{
    F.CloseFile();
    // Already cached — skip
    return Status;
}

_ReceivedMessage->SetPayloadFileName(Values.at(0));
_ReceivedMessage->SetPayloadFilePath(CachePath);
_ReceivedMessage->SetPayloadFileOption("BINARY");
_ReceivedMessage->ConvertPayloadFromCharArrayToFile();
```

### 4.2 HTGetBind01 — Alarme quando cache não existe

Em `Common/src/HTGetBind01.cpp` (cat=18, linhas 131-137):

| Antes | Depois |
|---|---|
| Chamava `ConvertPayloadFromFileToCharArray()` sem verificar retorno | Verifica retorno; só adiciona `-info --payload` se OK |
| Falha silenciosa quando ficheiro não existe | Emite ALARM se ficheiro não encontrado |

```cpp
if (InlineResponseMessage->ConvertPayloadFromFileToCharArray() == OK)
{
    PMB->NewInfoPayloadCommandLine("0.1", _Values, InlineResponseMessage, NewHTDeliveryBind01);
}
else
{
    PB->S << Offset << "(ALARM: Payload file " << _Values->at(0)
          << " not found in cache at " << ThePath
          << ". The NRInfoPayload01 on the source must cache it before the subscription arrives.)" << endl;
}
```

## 5. Ficheiros Alterados

| Ficheiro | Mudança |
|----------|---------|
| `NRNCS/src/NRInfoPayload01.cpp` | Substituído forwarding por cache em disco |
| `Common/src/HTGetBind01.cpp` | Alarme quando ficheiro não existe em cache |

## 6. Verificação

1. Compilar: `cd build && cmake .. && make -j$(nproc) NRNCS`
2. Deploy para source guest e repository guest
3. Source36: verificar `IO/NRNCS/` contém os ficheiros cacheados
4. Repo61: verificar que recebe conteúdo com hash correcto
5. Log: verificar `(Cached file: ...)` no NRNCS source
6. Se faltar ficheiro em cache, verificar `(ALARM: Payload file ... not found in cache)`