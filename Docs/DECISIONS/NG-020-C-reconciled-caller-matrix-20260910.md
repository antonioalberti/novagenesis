# SPEC-033 R3-C — reconciled caller matrix

**Source revision:** `0dc2b38e1d95e09877a4b3e7c90cf2d3bce282b9` (AIOPT3)
**Method:** three Astra reviews reconciled by exact `file:line`; conflicts and omissions conservatively become `UNRESOLVED`.

| Classification | Count |
|---|---:|
| SAFE | 8 |
| UPSTREAM-DEFECT-OUTSIDE-C | 38 |
| POINTER-PRESERVATION-COMPATIBILITY-RISK | 3 |
| UNRESOLVED | 27 |
| **Total** | **76** |

| # | Caller | Classification | Reconciliation note |
|---:|---|---|---|
| 1 | `Common/src/CLI.cpp:69` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 2 | `Common/src/CLI.cpp:197` | **UNRESOLVED** | single Astra classification |
| 3 | `Common/src/CLIRunInitialization01.cpp:78` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 4 | `Common/src/GW.cpp:113` | **SAFE** | single Astra classification |
| 5 | `Common/src/GW.cpp:615` | **SAFE** | single Astra classification |
| 6 | `Common/src/GW.cpp:856` | **SAFE** | single Astra classification |
| 7 | `Common/src/GWExposition02.cpp:259` | **UNRESOLVED** | single Astra classification |
| 8 | `Common/src/GWExposition02.cpp:381` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 9 | `Common/src/GWExposition02.cpp:448` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 10 | `Common/src/GWMsgCl01.cpp:160` | **UNRESOLVED** | single Astra classification |
| 11 | `Common/src/GWMsgCl01.cpp:494` | **UNRESOLVED** | single Astra classification |
| 12 | `Common/src/GWMsgCl01.cpp:629` | **UNRESOLVED** | single Astra classification |
| 13 | `Common/src/GWRunHelloIPC02.cpp:83` | **UNRESOLVED** | single Astra classification |
| 14 | `Common/src/GWRunInitialization01.cpp:82` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 15 | `Common/src/GWRunInitialization01.cpp:209` | **UNRESOLVED** | conflicting Astra classifications across batches |
| 16 | `Common/src/GWRunInitialization01.cpp:250` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 17 | `Common/src/HTRunPeriodic01.cpp:127` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 18 | `Common/src/HTRunPeriodic01.cpp:159` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 19 | `Common/src/MessageBuilder.cpp:1214` | **SAFE** | single Astra classification |
| 20 | `Common/src/MessageBuilder.cpp:1253` | **SAFE** | single Astra classification |
| 21 | `Common/src/MessageBuilder.cpp:1292` | **SAFE** | single Astra classification |
| 22 | `Common/src/MessageBuilder.cpp:1331` | **SAFE** | single Astra classification |
| 23 | `Common/src/MessageBuilder.cpp:1369` | **SAFE** | single Astra classification |
| 24 | `Common/src/Process.cpp:1100` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 25 | `PGCS/src/Core.cpp:180` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 26 | `PGCS/src/CoreMsgCl01.cpp:93` | **UNRESOLVED** | single Astra classification |
| 27 | `PGCS/src/CoreNotifyS01.cpp:246` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 28 | `PGCS/src/CoreRunDiscover01.cpp:121` | **UNRESOLVED** | single Astra classification |
| 29 | `PGCS/src/CoreRunDiscover01.cpp:264` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 30 | `PGCS/src/CoreRunEvaluate01.cpp:554` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 31 | `PGCS/src/CoreRunEvaluate01.cpp:684` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 32 | `PGCS/src/CoreRunEvaluate01.cpp:836` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 33 | `PGCS/src/CoreRunExpose01.cpp:127` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 34 | `PGCS/src/CoreRunInitialize01.cpp:94` | **UNRESOLVED** | single Astra classification |
| 35 | `PGCS/src/CoreRunInitialize01.cpp:171` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 36 | `PGCS/src/CoreRunPeriodic01.cpp:119` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 37 | `PGCS/src/CoreRunPeriodic01.cpp:149` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 38 | `PGCS/src/CoreRunSubscribe01.cpp:117` | **UNRESOLVED** | single Astra classification |
| 39 | `PGCS/src/PG.cpp:234` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 40 | `PGCS/src/PGHelloIHC02.cpp:317` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 41 | `PGCS/src/PGHelloIHC03.cpp:209` | **POINTER-PRESERVATION-COMPATIBILITY-RISK** | single Astra classification |
| 42 | `PGCS/src/PGRunExposition01.cpp:232` | **UNRESOLVED** | single Astra classification |
| 43 | `PGCS/src/PGRunHello01.cpp:105` | **UNRESOLVED** | single Astra classification |
| 44 | `PGCS/src/PGRunHello02.cpp:104` | **POINTER-PRESERVATION-COMPATIBILITY-RISK** | single Astra classification |
| 45 | `PGCS/src/PGRunHello03.cpp:113` | **UNRESOLVED** | single Astra classification |
| 46 | `PGCS/src/PGRunInitialization01.cpp:231` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 47 | `PGCS/src/PGRunInitialization01.cpp:467` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 48 | `PGCS/src/PGRunPeriodic01.cpp:128` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 49 | `PGCS/src/PGRunPeriodic01.cpp:362` | **UNRESOLVED** | conflicting Astra classifications across batches |
| 50 | `PGCS/src/PGRunPeriodic01.cpp:395` | **UNRESOLVED** | conflicting Astra classifications across batches |
| 51 | `PGCS/src/PGRunPeriodic01.cpp:427` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 52 | `PGCS/src/PGRunPeriodic01.cpp:490` | **UNRESOLVED** | single Astra classification |
| 53 | `PGCS/src/PGRunPeriodic01.cpp:562` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 54 | `PGCS/src/PGRunPeriodic01.cpp:669` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 55 | `PGCS/src/PGRunPublishing01.cpp:122` | **POINTER-PRESERVATION-COMPATIBILITY-RISK** | single Astra classification |
| 56 | `PGCS/src/PGRunStresstest01.cpp:112` | **UNRESOLVED** | single Astra classification |
| 57 | `NRNCS/src/NR.cpp:112` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 58 | `NRNCS/src/NRDeliveryBind01.cpp:123` | **UNRESOLVED** | single Astra classification |
| 59 | `NRNCS/src/NRPubNotify01.cpp:174` | **UNRESOLVED** | not present in supplied batch report |
| 60 | `NRNCS/src/NRRunInitialization01.cpp:96` | **UNRESOLVED** | single Astra classification |
| 61 | `NRNCS/src/NRRunInitialization01.cpp:272` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 62 | `NRNCS/src/NRRunPeriodic01.cpp:111` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 63 | `NRNCS/src/NRRunPeriodic01.cpp:219` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 64 | `NRNCS/src/NRSubBind01.cpp:130` | **UNRESOLVED** | single Astra classification |
| 65 | `ContentApp/src/Core.cpp:187` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 66 | `ContentApp/src/CoreMsgCl01.cpp:104` | **UNRESOLVED** | single Astra classification |
| 67 | `ContentApp/src/CoreNotifyS01.cpp:234` | **UNRESOLVED** | single Astra classification |
| 68 | `ContentApp/src/CoreRunContentPublish01.cpp:339` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 69 | `ContentApp/src/CoreRunDiscover01.cpp:131` | **UNRESOLVED** | single Astra classification |
| 70 | `ContentApp/src/CoreRunDiscover01.cpp:283` | **UNRESOLVED** | single Astra classification |
| 71 | `ContentApp/src/CoreRunExpose01.cpp:126` | **UNRESOLVED** | single Astra classification |
| 72 | `ContentApp/src/CoreRunInitialize01.cpp:95` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 73 | `ContentApp/src/CoreRunInitialize01.cpp:172` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 74 | `ContentApp/src/CoreRunPeriodic01.cpp:130` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 75 | `ContentApp/src/CoreRunPeriodic01.cpp:339` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |
| 76 | `ContentApp/src/CoreRunSubscribe01.cpp:138` | **UPSTREAM-DEFECT-OUTSIDE-C** | single Astra classification |

## Decision

The inventory is complete at 76 rows, but semantic compatibility is not closed for the unresolved rows. The three conflicting rows and the omitted `NRNCS/src/NRPubNotify01.cpp:174` remain unresolved. C stays NO-GO; no Process.cpp production edit is authorized.

## Next evidence required

Supply the complete enclosing functions and material helper/handler contracts for the unresolved rows, prioritizing direct dereferences, pointer reuse, output cleanup and exception handlers.
