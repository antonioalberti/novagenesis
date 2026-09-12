# SPEC-033 R2 — Initial caller audit (production components)

Generated from clean HEAD `71df07e`; syntactic inventory records: 78.
This is a review aid, not proof of semantic safety. Calls with file-bearing overloads are excluded by the four-argument shape; multiline and wrappers require manual review.

Initial classification: 6 visibly status-checked, 72 not visibly status-checked.

## Common
- `Common/src/CLIRunInitialization01.cpp:78` — status not visibly checked; output `StoringInitialBinds`; enclosing: `unknown`
- `Common/src/Process.cpp:542` — status not visibly checked; output `Message*& M`; enclosing: `unknown`
- `Common/src/Process.cpp:1100` — status not visibly checked; output `PListBindingsMessage`; enclosing: `unknown`
- `Common/src/HTRunPeriodic01.cpp:94` — status not visibly checked; output `BindReport`; enclosing: `unknown`
- `Common/src/HTRunPeriodic01.cpp:127` — status not visibly checked; output `ListBindings`; enclosing: `unknown`
- `Common/src/HTRunPeriodic01.cpp:159` — status not visibly checked; output `RunPeriodic`; enclosing: `unknown`
- `Common/src/CLI.cpp:69` — status not visibly checked; output `PIM`; enclosing: `unknown`
- `Common/src/CLI.cpp:197` — status not visibly checked; output `PM`; enclosing: `unknown`
- `Common/src/GWMsgCl01.cpp:160` — status not visibly checked; output `AutomaticInlineResponseMessage`; enclosing: `unknown`
- `Common/src/GWMsgCl01.cpp:494` — status not visibly checked; output `AutomaticInlineResponseMessage`; enclosing: `unknown`
- `Common/src/GWMsgCl01.cpp:629` — status not visibly checked; output `AutomaticInlineResponseMessage`; enclosing: `unknown`
- `Common/src/GWExposition02.cpp:259` — status not visibly checked; output `FreshHello`; enclosing: `unknown`
- `Common/src/GWExposition02.cpp:381` — status not visibly checked; output `SelfHello`; enclosing: `unknown`
- `Common/src/GWExposition02.cpp:448` — status not visibly checked; output `SelfMsg`; enclosing: `unknown`
- `Common/src/MessageBuilder.cpp:1213` — status visibly checked; output `_M`; enclosing: `unknown`
- `Common/src/MessageBuilder.cpp:1248` — status visibly checked; output `_M`; enclosing: `unknown`
- `Common/src/MessageBuilder.cpp:1283` — status visibly checked; output `_M`; enclosing: `unknown`
- `Common/src/MessageBuilder.cpp:1318` — status visibly checked; output `_M`; enclosing: `unknown`
- `Common/src/MessageBuilder.cpp:1353` — status visibly checked; output `_M`; enclosing: `unknown`
- `Common/src/GWRunHelloIPC02.cpp:83` — status not visibly checked; output `IPCHello`; enclosing: `unknown`
- `Common/src/GWRunInitialization01.cpp:82` — status not visibly checked; output `StoringInitialBinds`; enclosing: `unknown`
- `Common/src/GWRunInitialization01.cpp:209` — status not visibly checked; output `RunExposition`; enclosing: `unknown`
- `Common/src/GWRunInitialization01.cpp:250` — status not visibly checked; output `RunHelloIPC`; enclosing: `unknown`
- `Common/src/GW.cpp:113` — status not visibly checked; output `PIM`; enclosing: `unknown`
- `Common/src/GW.cpp:609` — status visibly checked; output `PM`; enclosing: `unknown`
- `Common/src/GW.cpp:850` — status not visibly checked; output `PM`; enclosing: `unknown`

## PGCS
- `PGCS/src/PGRunHello01.cpp:105` — status not visibly checked; output `PGIHCHello`; enclosing: `unknown`
- `PGCS/src/CoreMsgCl01.cpp:93` — status not visibly checked; output `Run`; enclosing: `unknown`
- `PGCS/src/CoreRunEvaluate01.cpp:554` — status not visibly checked; output `SubData`; enclosing: `unknown`
- `PGCS/src/CoreRunEvaluate01.cpp:684` — status not visibly checked; output `PublishAcceptance`; enclosing: `unknown`
- `PGCS/src/CoreRunEvaluate01.cpp:836` — status not visibly checked; output `Revoke`; enclosing: `unknown`
- `PGCS/src/CoreRunInitialize01.cpp:94` — status not visibly checked; output `StoringInitialBinds`; enclosing: `unknown`
- `PGCS/src/CoreRunInitialize01.cpp:171` — status not visibly checked; output `RunPeriodic`; enclosing: `unknown`
- `PGCS/src/PGRunExposition01.cpp:232` — status not visibly checked; output `Exposition`; enclosing: `unknown`
- `PGCS/src/PGRunPublishing01.cpp:122` — status not visibly checked; output `Publish`; enclosing: `unknown`
- `PGCS/src/CoreRunSubscribe01.cpp:117` — status not visibly checked; output `Subcription`; enclosing: `unknown`
- `PGCS/src/PGRunHello02.cpp:104` — status not visibly checked; output `PGIHCHello`; enclosing: `unknown`
- `PGCS/src/CoreRunPeriodic01.cpp:119` — status not visibly checked; output `RunPeriodic`; enclosing: `unknown`
- `PGCS/src/CoreRunPeriodic01.cpp:149` — status not visibly checked; output `RunEvaluate`; enclosing: `unknown`
- `PGCS/src/CoreRunDiscover01.cpp:121` — status not visibly checked; output `Discovery`; enclosing: `unknown`
- `PGCS/src/CoreRunDiscover01.cpp:264` — status not visibly checked; output `Discovery`; enclosing: `unknown`
- `PGCS/src/CoreNotifyS01.cpp:246` — status not visibly checked; output `SubscriptionM`; enclosing: `unknown`
- `PGCS/src/Core.cpp:180` — status not visibly checked; output `PIM`; enclosing: `unknown`
- `PGCS/src/PGHelloIHC02.cpp:317` — status not visibly checked; output `StoreBind01Msg`; enclosing: `unknown`
- `PGCS/src/CoreRunExpose01.cpp:127` — status not visibly checked; output `Publish`; enclosing: `unknown`
- `PGCS/src/PGHelloIHC03.cpp:209` — status not visibly checked; output `StoreBind01Msg`; enclosing: `unknown`
- `PGCS/src/PGRunHello03.cpp:113` — status not visibly checked; output `PGIHCHello`; enclosing: `unknown`
- `PGCS/src/PGRunInitialization01.cpp:231` — status not visibly checked; output `StoringInitialBinds`; enclosing: `unknown`
- `PGCS/src/PGRunInitialization01.cpp:467` — status not visibly checked; output `RunPeriodic`; enclosing: `unknown`
- `PGCS/src/PGRunStresstest01.cpp:112` — status not visibly checked; output `StressPing`; enclosing: `unknown`
- `PGCS/src/PGRunPeriodic01.cpp:128` — status not visibly checked; output `RunPeriodic`; enclosing: `unknown`
- `PGCS/src/PGRunPeriodic01.cpp:362` — status not visibly checked; output `RunHello01`; enclosing: `unknown`
- `PGCS/src/PGRunPeriodic01.cpp:395` — status not visibly checked; output `RunHello02`; enclosing: `unknown`
- `PGCS/src/PGRunPeriodic01.cpp:427` — status not visibly checked; output `RunHello03`; enclosing: `unknown`
- `PGCS/src/PGRunPeriodic01.cpp:490` — status not visibly checked; output `RunExposition`; enclosing: `unknown`
- `PGCS/src/PGRunPeriodic01.cpp:562` — status not visibly checked; output `RunPublishing`; enclosing: `unknown`
- `PGCS/src/PGRunPeriodic01.cpp:669` — status not visibly checked; output `RunStresstest`; enclosing: `unknown`
- `PGCS/src/PG.cpp:234` — status not visibly checked; output `PIM`; enclosing: `unknown`

## NRNCS
- `NRNCS/src/NRRunPeriodic01.cpp:111` — status not visibly checked; output `RunPeriodic`; enclosing: `unknown`
- `NRNCS/src/NRRunPeriodic01.cpp:219` — status not visibly checked; output `ExposingInitialBinds`; enclosing: `unknown`
- `NRNCS/src/NRRunInitialization01.cpp:96` — status not visibly checked; output `StoringInitialBinds`; enclosing: `unknown`
- `NRNCS/src/NRRunInitialization01.cpp:272` — status not visibly checked; output `RunPeriodic`; enclosing: `unknown`
- `NRNCS/src/NR.cpp:112` — status not visibly checked; output `PIM`; enclosing: `unknown`
- `NRNCS/src/NRSubBind01.cpp:130` — status not visibly checked; output `GetBindMessage`; enclosing: `unknown`
- `NRNCS/src/NRPubNotify01.cpp:174` — status not visibly checked; output `Notify`; enclosing: `unknown`
- `NRNCS/src/NRDeliveryBind01.cpp:123` — status not visibly checked; output `StoreBindings`; enclosing: `unknown`

## ContentApp
- `ContentApp/src/CoreMsgCl01.cpp:104` — status not visibly checked; output `Run`; enclosing: `unknown`
- `ContentApp/src/CoreRunInitialize01.cpp:95` — status not visibly checked; output `StoringInitialBinds`; enclosing: `unknown`
- `ContentApp/src/CoreRunInitialize01.cpp:172` — status not visibly checked; output `RunPeriodic`; enclosing: `unknown`
- `ContentApp/src/CoreRunSubscribe01.cpp:138` — status not visibly checked; output `Subcription`; enclosing: `unknown`
- `ContentApp/src/CoreRunPeriodic01.cpp:130` — status not visibly checked; output `RunPeriodic`; enclosing: `unknown`
- `ContentApp/src/CoreRunPeriodic01.cpp:339` — status not visibly checked; output `SubscriptionM`; enclosing: `unknown`
- `ContentApp/src/CoreRunDiscover01.cpp:131` — status not visibly checked; output `Discovery`; enclosing: `unknown`
- `ContentApp/src/CoreRunDiscover01.cpp:283` — status not visibly checked; output `Discovery`; enclosing: `unknown`
- `ContentApp/src/CoreRunContentPublish01.cpp:339` — status not visibly checked; output `RunPhotoPublish`; enclosing: `unknown`
- `ContentApp/src/CoreNotifyS01.cpp:234` — status not visibly checked; output `SubscriptionM`; enclosing: `unknown`
- `ContentApp/src/Core.cpp:187` — status not visibly checked; output `PIM`; enclosing: `unknown`
- `ContentApp/src/CoreRunExpose01.cpp:126` — status not visibly checked; output `Publish`; enclosing: `unknown`

