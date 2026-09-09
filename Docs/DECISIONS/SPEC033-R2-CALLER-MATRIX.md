# SPEC-033 R2 — Complete semantic caller matrix (triage)

HEAD: `71df07e`; scope: Common, PGCS, NRNCS and ContentApp.
The parser reconciled {len(recs)} source-level calls to `Process::NewMessage(double, short, bool, Message*&)`, excluding comments, definitions and file-bearing overloads. Each row is based on the enclosing source context and is conservative: a blocker candidate is not claimed to be a proven runtime failure until reachability/capacity is established.

## Disposition rule

- `blocker candidate`: return is not checked and the output is consumed or propagated immediately; if exhaustion is reachable, this path needs caller handling or a supported invariant.
- `potentially failure-safe`: a status check is visible, but the complete failure/wrapper path still requires confirmation.
- `unresolved`: the excerpt does not establish safe failure semantics; inspect the enclosing path and capacity invariant.

## Matrix

| # | Location | Output | Return status | Later use | Disposition | Evidence |
|---:|---|---|---|---|---|---|
| 1 | `Common/src/CLI.cpp:69` | `PIM` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 2 | `Common/src/CLI.cpp:197` | `PM` | checked | not shown | potentially failure-safe | status checked in surrounding path; failure branch needs verification |
| 3 | `Common/src/CLIRunInitialization01.cpp:78` | `StoringInitialBinds` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 4 | `Common/src/GW.cpp:113` | `PIM` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 5 | `Common/src/GW.cpp:609` | `PM` | checked | yes | potentially failure-safe | status checked in surrounding path; failure branch needs verification |
| 6 | `Common/src/GW.cpp:850` | `PM` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 7 | `Common/src/GWExposition02.cpp:259` | `FreshHello` | unchecked | not shown | unresolved | no immediate output use in excerpt; enclosing invariant/path audit required |
| 8 | `Common/src/GWExposition02.cpp:381` | `SelfHello` | unchecked | not shown | unresolved | no immediate output use in excerpt; enclosing invariant/path audit required |
| 9 | `Common/src/GWExposition02.cpp:448` | `SelfMsg` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 10 | `Common/src/GWMsgCl01.cpp:160` | `AutomaticInlineResponseMessage` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 11 | `Common/src/GWMsgCl01.cpp:494` | `AutomaticInlineResponseMessage` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 12 | `Common/src/GWMsgCl01.cpp:629` | `AutomaticInlineResponseMessage` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 13 | `Common/src/GWRunHelloIPC02.cpp:83` | `IPCHello` | unchecked | not shown | unresolved | no immediate output use in excerpt; enclosing invariant/path audit required |
| 14 | `Common/src/GWRunInitialization01.cpp:82` | `StoringInitialBinds` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 15 | `Common/src/GWRunInitialization01.cpp:209` | `RunExposition` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 16 | `Common/src/GWRunInitialization01.cpp:250` | `RunHelloIPC` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 17 | `Common/src/HTRunPeriodic01.cpp:127` | `ListBindings` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 18 | `Common/src/HTRunPeriodic01.cpp:159` | `RunPeriodic` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 19 | `Common/src/MessageBuilder.cpp:1213` | `_M` | checked | yes | potentially failure-safe | status checked in surrounding path; failure branch needs verification |
| 20 | `Common/src/MessageBuilder.cpp:1248` | `_M` | checked | yes | potentially failure-safe | status checked in surrounding path; failure branch needs verification |
| 21 | `Common/src/MessageBuilder.cpp:1283` | `_M` | checked | yes | potentially failure-safe | status checked in surrounding path; failure branch needs verification |
| 22 | `Common/src/MessageBuilder.cpp:1318` | `_M` | checked | yes | potentially failure-safe | status checked in surrounding path; failure branch needs verification |
| 23 | `Common/src/MessageBuilder.cpp:1353` | `_M` | checked | yes | potentially failure-safe | status checked in surrounding path; failure branch needs verification |
| 24 | `Common/src/Process.cpp:1100` | `PListBindingsMessage` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 25 | `ContentApp/src/Core.cpp:187` | `PIM` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 26 | `ContentApp/src/CoreMsgCl01.cpp:104` | `Run` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 27 | `ContentApp/src/CoreNotifyS01.cpp:234` | `SubscriptionM` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 28 | `ContentApp/src/CoreRunContentPublish01.cpp:339` | `RunPhotoPublish` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 29 | `ContentApp/src/CoreRunDiscover01.cpp:131` | `Discovery` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 30 | `ContentApp/src/CoreRunDiscover01.cpp:283` | `Discovery` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 31 | `ContentApp/src/CoreRunExpose01.cpp:126` | `Publish` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 32 | `ContentApp/src/CoreRunInitialize01.cpp:95` | `StoringInitialBinds` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 33 | `ContentApp/src/CoreRunInitialize01.cpp:172` | `RunPeriodic` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 34 | `ContentApp/src/CoreRunPeriodic01.cpp:130` | `RunPeriodic` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 35 | `ContentApp/src/CoreRunPeriodic01.cpp:339` | `SubscriptionM` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 36 | `ContentApp/src/CoreRunSubscribe01.cpp:138` | `Subcription` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 37 | `NRNCS/src/NR.cpp:112` | `PIM` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 38 | `NRNCS/src/NRDeliveryBind01.cpp:123` | `StoreBindings` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 39 | `NRNCS/src/NRPubNotify01.cpp:174` | `Notify` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 40 | `NRNCS/src/NRRunInitialization01.cpp:96` | `StoringInitialBinds` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 41 | `NRNCS/src/NRRunInitialization01.cpp:272` | `RunPeriodic` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 42 | `NRNCS/src/NRRunPeriodic01.cpp:111` | `RunPeriodic` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 43 | `NRNCS/src/NRRunPeriodic01.cpp:219` | `ExposingInitialBinds` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 44 | `NRNCS/src/NRSubBind01.cpp:130` | `GetBindMessage` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 45 | `PGCS/src/Core.cpp:180` | `PIM` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 46 | `PGCS/src/CoreMsgCl01.cpp:93` | `Run` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 47 | `PGCS/src/CoreNotifyS01.cpp:246` | `SubscriptionM` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 48 | `PGCS/src/CoreRunDiscover01.cpp:121` | `Discovery` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 49 | `PGCS/src/CoreRunDiscover01.cpp:264` | `Discovery` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 50 | `PGCS/src/CoreRunEvaluate01.cpp:554` | `SubData` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 51 | `PGCS/src/CoreRunEvaluate01.cpp:684` | `PublishAcceptance` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 52 | `PGCS/src/CoreRunEvaluate01.cpp:836` | `Revoke` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 53 | `PGCS/src/CoreRunExpose01.cpp:127` | `Publish` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 54 | `PGCS/src/CoreRunInitialize01.cpp:94` | `StoringInitialBinds` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 55 | `PGCS/src/CoreRunInitialize01.cpp:171` | `RunPeriodic` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 56 | `PGCS/src/CoreRunPeriodic01.cpp:119` | `RunPeriodic` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 57 | `PGCS/src/CoreRunPeriodic01.cpp:149` | `RunEvaluate` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 58 | `PGCS/src/CoreRunSubscribe01.cpp:117` | `Subcription` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 59 | `PGCS/src/PG.cpp:234` | `PIM` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 60 | `PGCS/src/PGHelloIHC02.cpp:317` | `StoreBind01Msg` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 61 | `PGCS/src/PGHelloIHC03.cpp:209` | `StoreBind01Msg` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 62 | `PGCS/src/PGRunExposition01.cpp:232` | `Exposition` | unchecked | not shown | unresolved | no immediate output use in excerpt; enclosing invariant/path audit required |
| 63 | `PGCS/src/PGRunHello01.cpp:105` | `PGIHCHello` | unchecked | not shown | unresolved | no immediate output use in excerpt; enclosing invariant/path audit required |
| 64 | `PGCS/src/PGRunHello02.cpp:104` | `PGIHCHello` | unchecked | not shown | unresolved | no immediate output use in excerpt; enclosing invariant/path audit required |
| 65 | `PGCS/src/PGRunHello03.cpp:113` | `PGIHCHello` | unchecked | not shown | unresolved | no immediate output use in excerpt; enclosing invariant/path audit required |
| 66 | `PGCS/src/PGRunInitialization01.cpp:231` | `StoringInitialBinds` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 67 | `PGCS/src/PGRunInitialization01.cpp:467` | `RunPeriodic` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 68 | `PGCS/src/PGRunPeriodic01.cpp:128` | `RunPeriodic` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 69 | `PGCS/src/PGRunPeriodic01.cpp:362` | `RunHello01` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 70 | `PGCS/src/PGRunPeriodic01.cpp:395` | `RunHello02` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 71 | `PGCS/src/PGRunPeriodic01.cpp:427` | `RunHello03` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 72 | `PGCS/src/PGRunPeriodic01.cpp:490` | `RunExposition` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 73 | `PGCS/src/PGRunPeriodic01.cpp:562` | `RunPublishing` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 74 | `PGCS/src/PGRunPeriodic01.cpp:669` | `RunStresstest` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 75 | `PGCS/src/PGRunPublishing01.cpp:122` | `Publish` | unchecked | yes | blocker candidate | unchecked return followed by output consumption/propagation |
| 76 | `PGCS/src/PGRunStresstest01.cpp:112` | `StressPing` | unchecked | not shown | unresolved | no immediate output use in excerpt; enclosing invariant/path audit required |

## Audit conclusion

The matrix contains many blocker candidates in startup, periodic, discovery, subscription, routing, stress and photo paths. No caller is cleared solely because ordinary runs rarely exhaust capacity. The first-overload-only production correction remains blocked until blocker candidates are either made failure-safe by an explicitly approved caller amendment or cleared by an enforced capacity/lifecycle invariant.

Detailed code excerpts used for this matrix: `/tmp/ng-caller-contexts.md`.

## Confirmed wrapper evidence

The five MessageBuilder wrappers were exercised against a full production Process
in the external fixture `wrapper_exhaustion.cpp`. All returned `ERROR` while
leaving their independently retained outputs non-null and marked for deletion:
`wrappers=1,1,1,1,1`, `out0..out4_null=0`, `out0..out4_marked=1`, `count=30000`.
This confirms the wrapper failure path is unsafe and must precede any Process.cpp
output-clearing change. See `wrapper-baseline-run.log` and
`wrapper-build.log` in the characterization directory.
