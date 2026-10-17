You are GPT-6 Astra. This is exactly ONE owner-authorized documentary review. Do not use tools, execute code, build, run tests, or imply independent filesystem inspection. This is not a request to implement or operate NovaGenesis.

===== OWNER CONSENT AND EXACT SCOPE =====
The owner’s immediately preceding message explicitly authorizes exactly one Astra documentary review of the primary packet SHA-256:
`a88434f223c3b66635d2589140477c01c5142ae94ba0bb839d026c3fde8013bd`.
That consent is fresh and specific to this one call. The frozen validation receipt and packet predate this consent; their `fresh_owner_exact_packet_sha_consent=false` / `astra_submitted=false` values are freeze-time facts, not a contradiction of the present consent. No follow-up call is authorized, even if you return AMEND or request more context.

The primary review target is only the frozen SPEC-039 v1.14/r12 documentary packet below. The full current SPEC and manifest are supplied as digest-matched context so you can check relevant duplication/consistency; this does not expand the review to implementation, unrelated historical criteria, or runtime acceptance. Distinguish concrete normative blockers from optional advice. Do not call a file or test independently verified by you: all evidence is supplied text.

===== REQUESTED DECISION =====
Review the r12 response to the embedded r11 AMEND. Assess whether it closes r11 B1 (common lease/supervisor/result-directory authority; lease loss blocks entry/re-entry and preserves pending evidence), B2 (truthful historical cleanup outcome), and the bounded raw-evidence-preservation advice, while preserving r08 B2/B3 controls. Check cross-references among the SPEC, complete manifest, changed schema branches, classification rules, artifact projection, and exact packet. In particular, look for stale `not_started` requirements in PASS/FAIL, a missing `complete` outcome, permissive or contradictory journal-classifier rules, incorrect missing/unavailable sets, or a false claim that unexecuted PID cross-artifact checks passed.

Return concise UK English in this form:
1. VERDICT: APPROVE / AMEND / HOLD — documentary scope only.
2. MUST-FIX blockers: each with exact file/section/key and concrete contradiction, or explicitly “None”. Separate optional advice.
3. Evidence limitation: state what you did not execute or independently inspect.
4. Authority boundary: explicitly grant no implementation/build/runtime/IPC/VM/commit/push/D3 authority.
Do not reject solely because the focused packet has a frozen digest rather than embedding its self-hash, or because the receipt records consent as false at pre-consent freeze time. Do not reopen r07 findings without identifying a new, concrete contradiction in the supplied r12 bytes. Do not infer that compile/link-only D1-H or synthetic fixtures prove D2 runtime correctness or performance.

===== FROZEN IDENTITIES AND WORKTREE CONTEXT =====
Repository branch: AIOPT5
Current document HEAD: 756f493b7049434ab67cf2382486a48f606a470c; tree: 9adf665c0eb06213a29a6c12d650451948c7a4ab
Functional source baseline: 1af604dad66148618cc0e578c28adb4a9829e607; tree: 5a3ebb9fbdf61185f05b18e58b8799ced40eb60b
SPEC SHA-256: 0cda2743e797cfbed952a1bad7c30e8e0f15d37c57e51d70e3388b9453103115
Manifest SHA-256: 63ccf50e6ecd19940f0c0b5e98d438ae6e34f5b2dbf0304faef97b5eca7c0917
Primary packet SHA-256: a88434f223c3b66635d2589140477c01c5142ae94ba0bb839d026c3fde8013bd
Validation receipt SHA-256: 481fb1dd8b5cad5951a5916a16e4e03481dc8fe70687923a3f60c7c77b5e4c73
Prior r09 review SHA-256: 4b52a1759768714492d4c2bc5099fc9a3626a334e1a33c65e627f1725494fd01
Prior r10 review SHA-256: 6aa0669c3cec1b11c2516d6c361d78460663cde9f097cc4c5de8264557e64c2e
Prior r11 review SHA-256: 9ef48bdfafbfd9818613d9356f9b011972f6efc0af1d2433f7a039c3ba260ed7
Current Git status (context only; no source-code changes):
```text
## AIOPT5...origin/AIOPT5 [ahead 2]
 M Specs/SPEC-039-common-performance-measurement.md
 M Specs/SPEC-062-ng-elc-aiopt5-cross-vm-lifecycle-oracle.md
?? Specs/RESULTS-SPEC-039/
?? Specs/RESULTS-SPEC-059/
```
The current packet bytes and all listed identities were rehashed immediately before request assembly. The packet is embedded below byte-for-byte. Full SPEC and full manifest are supplied below at the exact SHA-256 values above; the packet’s changed-clause projections are not substituted for those full files.

===== HISTORICAL REVIEW LINEAGE =====
- r09 (`4b52a1759768714492d4c2bc5099fc9a3626a334e1a33c65e627f1725494fd01`): AMEND. B1 identified a contradiction between “finalizer does not touch IPC” and its assigned bounded inventory/current-identity inspection and journal-authorized cleanup; r08 B2/B3 were otherwise substantially addressed.
- r10 (`6aa0669c3cec1b11c2516d6c361d78460663cde9f097cc4c5de8264557e64c2e`): AMEND. The IPC boundary was resolved; B1 identified that abnormal finalization required an INCONCLUSIVE record while unconditional finalizer preconditions required successful joins and Process/GW destruction.
- r11 (`9ef48bdfafbfd9818613d9356f9b011972f6efc0af1d2433f7a039c3ba260ed7`): AMEND. Its complete response is embedded verbatim in the r12 packet. B1 required a consistent common lease/file-authority gate; B2 required truthful cleanup history after an intent/removal. Optional advice required normal strict completeness checks not to block bounded abnormal raw preservation.
- These are documentary findings only. The launcher is still NOT_PROVISIONED; the semantic finalizer is absent.

===== LOCAL VALIDATION EVIDENCE (HERMES-RERUN, 2026-10-13) =====
Commands, run read-only against the frozen hashes:
`PYTHONDONTWRITEBYTECODE=1 /usr/bin/python3 /home/gandalf/CodeRepository/infra-novagenesis-tools/verify_spec039_v14_r12.py`
`PYTHONDONTWRITEBYTECODE=1 /usr/bin/python3 /home/gandalf/CodeRepository/infra-novagenesis-tools/test_spec039_v14_r12_classifier.py`
`git diff --check`
All three commands exited 0. Verifier output:
```json
{
  "identity": "PASS",
  "schemas": "5/5 meta-valid",
  "cleanup_classifier": "PASS",
  "classifier_cases": {
    "not_started": "PASS",
    "partial": "PASS",
    "complete": "PASS",
    "empty_unknown_malformed_reordered_truncated_created_without_cleanup": "unknown"
  },
  "stale_abnormal_outcome_rules": "ABSENT",
  "cross_artifact_pid_set": "NOT EXECUTED",
  "astra_submitted": false,
  "writes_performed": false
}
```
The classifier test emits no stdout and exited 0. These are local synthetic/documentary checks, not runtime-finalizer tests. The complete validation receipt and both checker sources follow.

===== BASELINE SOURCE EXCERPTS (READ-ONLY CONTEXT) =====
These bytes are from functional baseline `1af604dad66148618cc0e578c28adb4a9829e607` and their complete-file SHA-256 values match the frozen packet pins. The excerpts show the three `sem_open` failure branches that r08 B2 required to suppress in the opt-in test build, plus the already packet-pinned worker join and Process destruction ordering. They describe existing baseline code only; no fix is implemented or claimed.
FILE Common/src/GW.cpp; functional baseline 1af604dad66148618cc0e578c28adb4a9829e607; SHA-256 7893766ab808473f53b66d8fb9e3804ae5c579bc37d700d90e78155a2ab2cc5e
--- Common/src/GW.cpp:658-667 ---
658:   // Stop the output thread
659:   if (StopGateway == false)
660:   {
661:     StopGateway = true;
662:     OutputQueueCV.notify_all();
663:   }
664: 
665:   T->join();
666:   delete T;
667: }
--- Common/src/GW.cpp:963-971 ---
963:       // Semaphore kept open in CachedSemaphores (Phase 2: no sem_close per iteration)
964: 
965:     } // (mutex == SEM_FAILED) means it was unable to create/open the semaphore
966:     else
967:     {
968:       perror("reading SHM : unable to create/open semaphore");
969: 
970:       sem_unlink(SemaphoreName.c_str());
971:     }
--- Common/src/GW.cpp:1258-1264 ---
1258:       } //(mutex != SEM_FAILED)
1259:       else
1260:       {
1261:         perror("writing SHM : unable to open/create the semaphore");
1262: 
1263:         sem_unlink(_oqs.c_str());
1264:       }
--- Common/src/GW.cpp:1318-1325 ---
1318:   else
1319:   {
1320:     perror("While initiating the SHM: unable to create semaphore");
1321: 
1322:     sem_unlink(SemaphoreName.c_str());
1323: 
1324:     S << Offset << "(ERROR: Unable to create the semaphore while discovering the peer shmid)" << endl;
1325:   }

FILE Common/src/Process.cpp; functional baseline 1af604dad66148618cc0e578c28adb4a9829e607; SHA-256 bd45b77db19adf7a3c0f78bf5970996afce9720b77ea15519f5a4bcf703845d1
--- Common/src/Process.cpp:228-257 ---
228:   // Create the process blocks
229:   NewBlock("HT", PB3);
230:   NewBlock("GW", PB2);
231:   // NewBlock("CLI",PB1);
232: }
233: 
234: Process::~Process()
235: {
236:   cout << "Deleting " << LN << endl;
237: 
238:   cout << "Deleting Messages container" << endl;
239: 
240:   DeleteMessages();
241: 
242:   cout << "Done" << endl;
243: 
244:   vector<Block*>::iterator it;
245:   unsigned int i = 0;
246: 
247:   for (it = Blocks.begin(); it != Blocks.end(); it++)
248:   {
249:     delete Blocks[i];
250: 
251:     i++;
252:   }
253: 
254:   Blocks.clear();
255: 
256:   delete PMB;
257: }

===== FILE: Specs/RESULTS-SPEC-039/astra-review-v1.12-r09-20261012.md — historical response =====
## 1. VERDICT: AMEND — documentary contract only

Reviewed only the supplied packet identified as SHA-256 `560ee5654d9a8859e3e21aa916a1d7a6` **[correction: exact identity below]**:

`560ee5654d9a8859e3e21aa916a1dca9f0e564fe1f8eac41fb4ee8f4bd3397a6`

The r09 amendment substantially addresses r08 B2/B3, but contains one blocking contradiction about finalizer responsibilities. No unchanged r07 checker findings are reopened.

- **Dedicated UID/launcher/lease:** adequate as a mandatory, fail-closed prerequisite, not proof of an existing enforcement mechanism. The explicit `NOT_PROVISIONED`/`BLOCKED` states, prohibition on ordinary-host-UID execution, rejection of census-only evidence and trusted-root boundary appropriately prevent a readiness claim.
- **Diagnostic unlink suppression:** properly bounded to the three pinned branches, the opt-in isolated diagnostic build and targeted zero-unlink tests. It complements, rather than replaces, continuous exclusion.
- **Journal and inventories:** the required retained bytes, intent/outcome ordering, fsync obligations, ambiguous-crash handling, no-retry rule and final hash binding substantially close the earlier evidence gaps. Raw inventory digests and summaries must be independently recomputed.
- **Semaphore mapping:** the retained matching maps-line route can support pointer-range/shared-mapping/device/inode checks against the recorded semaphore and libc identity. This is an untested evidence procedure; missing or contradictory linkage must remain `INCONCLUSIVE`.
- **Classification:** schemas alone do not establish safety. The combined semantic contract appropriately requires evidence-derived status and excludes unsafe or incomplete runs from both PASS and correctness FAIL, subject to resolving the blocker below.

## 2. Must-fix blocker

### B1 — Contradictory finalizer IPC boundary

**Exact locations**

- **“Proposed runtime artifacts”**, D2 proposed allowlist row: `--finalize` performs finalisation “without constructing Process/GW **or touching IPC**”.
- **§7.1**, paragraphs beginning “The three pinned…” and “After Gateway/output threads…”: only the separate finalizer may clean resources; it verifies current identities and performs removal.
- **`collection_responsibility.outer_finalizer`**: the finalizer logs cleanup intent/outcome and performs exact identity-bound cleanup.
- **`sandbox.runtime_finalizer_cleanup`**: cleanup occurs after the run process exits.

**Concrete risk:** these requirements cannot all be satisfied. An implementation following the no-IPC clause cannot perform the assigned cleanup; an implementation following the cleanup clauses breaches that explicit boundary. It also leaves current-identity inspection and cleanup ownership contradictory at the point where safety depends on them.

**Required amendment:** establish one consistent owner and sequence. If `--finalize` owns cleanup, replace “or touching IPC” with a narrowly defined allowance for inventory/current-identity inspection and journal-authorised exact removal under the continuously verified lease. Continue to prohibit Process/GW construction, resource creation, production IPC use and speculative or broad cleanup. Align the cited clauses.

## 3. Optional advisories — not blockers

- Make the matching `mapping_line` mandatory for successful semaphore evidence, or add explicit mapping permissions and offset fields. The current structured alternative lacks those fields; the semantic contract must reject projections that cannot prove a shared mapping.
- Separate historical fixture counts from newly required negative classes and remove duplicated policy entries. These are not evidence that r09 semantic fixtures ran.
- State explicitly that retained journal bytes support replay but do not independently prove historical fsync calls; durability also depends on the subsequently verified collector implementation.

## 4. Authority boundary

The launcher remains **unprovisioned**, and execution remains **BLOCKED**. No tools, code execution, hash recomputation or runtime validation were performed.

**No operational authority is granted:** no implementation, configure/build, harness/finalizer execution, IPC, namespace, VM/SSH, D3, cleanup, commit, push or release. Documentary approval, if subsequently obtained, would not replace the separate provisioning, verification and owner-authorisation gates.

===== FILE: Specs/RESULTS-SPEC-039/astra-review-v1.13-r10-20261012.md — historical response =====
## 1. VERDICT: AMEND — documentary scope only

Reviewed only the supplied packet identified as SHA-256 `55141aab788839b0346d7ec1bda46c9e4cf86e9d33bc64ce67e242fcb2ff7146`. No tools, hash recomputation or runtime validation were used.

The original r09 prohibition on “touching IPC” is resolved. The finalizer’s IPC allowance is now narrowly and consistently limited to required inventory reads, exact current-identity inspection, and journal-authorised `IPC_RMID`/`sem_unlink`, with an immediate identity and lease recheck before each destructive call.

However, **one finalizer-sequence contradiction remains on abnormal termination**.

## 2. Must-fix blocker

### B1 — Unconditional finalizer startup prerequisites conflict with required failure finalisation

**Exact packet locations**
- **§7.1, numbered item 2:** uncertain ownership requires `_Exit` without invoking potentially unsafe `Process/GW` destructors; after process exit, “only the parent-side finalizer in item 5” may produce the final `INCONCLUSIVE` record. Partial construction and blocked join follow the same fail-closed classification.
- **Manifest `collection_responsibility.outer_finalizer`:** the finalizer “starts only after” harness exit/log closure, worker stop/join **and `Process/GW` destruction**.
- **Manifest `finalizer_ipc_boundary.owner_and_sequence` and `.preconditions`:** likewise require completed worker joins and `Process/GW` destruction.

**Concrete risk:** the specified abnormal-exit path deliberately cannot establish those startup prerequisites. An implementation must therefore either breach the unconditional startup rule to produce the required final record, or omit the finalisation explicitly assigned to it. Process termination is not evidence that C++ destructors or joins completed.

**Required amendment:** distinguish normal cleanup sequencing from abnormal-exit evidence finalisation. Explicitly permit the same sole finalizer to produce an `INCONCLUSIVE` record after confirmed process-tree termination/log closure when joins or destruction were incomplete or unavailable. Separately state the cleanup eligibility for that path, retaining all journal, identity and continuous-lease gates—or explicitly prohibit destructive calls there. Do not treat termination as successful join/destruction. Align the cited SPEC and manifest clauses.

## 3. Safeguards and optional advisories

**Safeguards remain intact at documentary level:**
- Finalizer construction of `Process/GW`, candidate creation/opening, production message traffic, broad/speculative cleanup and retry after ambiguous removal remain prohibited.
- Lease loss or uncertainty, journal ambiguity and identity mismatch prohibit destructive calls and preserve residue; ambiguous intent/outcome records cannot establish removal or trigger automatic retry.
- B2/B3 protections remain: dedicated-UID continuous exclusion, diagnostic suppression of all three pinned failure-path unlinks, retained/replayed journal and raw inventories, mapping evidence, and evidence-derived classification. These are requirements, not implementation or runtime proof.

**Optional r09 advisories remain non-blocking:** tighten mapping-evidence representation; separate historical fixture counts from newly required cases and remove duplicated policy entries; clarify that journal bytes do not independently prove historical fsync calls. No unchanged r07 checker findings are reopened.

## 4. Authority boundary

The launcher remains **NOT_PROVISIONED** and D2 **BLOCKED**. **No operational authority is granted**: no implementation, configure/build, harness/finalizer execution, IPC, namespace, VM/SSH, D3, cleanup, commit, push or release.

===== FILE: Specs/RESULTS-SPEC-039/precode-validation-v1.14-r12-20261012.json — frozen validation receipt =====
{
  "schema_version": "ng039-precode-validation-receipt/v2",
  "created_utc_date": "2026-10-12",
  "scope": "Local documentary/schema/classifier checks for SPEC-039 v1.14/r12 only; not runtime, implementation, IPC or finalizer evidence.",
  "repository": "AIOPT5",
  "document_head": "756f493b7049434ab67cf2382486a48f606a470c",
  "files": {
    "spec": {
      "path": "Specs/SPEC-039-common-performance-measurement.md",
      "sha256": "0cda2743e797cfbed952a1bad7c30e8e0f15d37c57e51d70e3388b9453103115"
    },
    "manifest": {
      "path": "Specs/RESULTS-SPEC-039/precode-manifest-v1.14-r12.json",
      "sha256": "63ccf50e6ecd19940f0c0b5e98d438ae6e34f5b2dbf0304faef97b5eca7c0917"
    },
    "packet": {
      "path": "Specs/RESULTS-SPEC-039/precode-packet-v1.14-amend-closure-20261012-sanitized-draft-r12.md",
      "sha256": "a88434f223c3b66635d2589140477c01c5142ae94ba0bb839d026c3fde8013bd",
      "bytes": 70789
    }
  },
  "validation_drivers": {
    "contract_verifier": {
      "path": "/home/gandalf/CodeRepository/infra-novagenesis-tools/verify_spec039_v14_r12.py",
      "sha256": "6a29bd9b24c2c60440383edeb1c89a14af04ce19ea7e25d564bcb1b211f8948c",
      "command": "PYTHONDONTWRITEBYTECODE=1 /usr/bin/python3 /home/gandalf/CodeRepository/infra-novagenesis-tools/verify_spec039_v14_r12.py",
      "result": "PASS; read-only; no files written"
    },
    "classifier_probes": {
      "path": "/home/gandalf/CodeRepository/infra-novagenesis-tools/test_spec039_v14_r12_classifier.py",
      "sha256": "b68c4299be7416aa26289d4d7c6628831b021294072d3091c81b10cc4984f80c",
      "command": "PYTHONDONTWRITEBYTECODE=1 /usr/bin/python3 /home/gandalf/CodeRepository/infra-novagenesis-tools/test_spec039_v14_r12_classifier.py",
      "result": "PASS; executes only synthetic in-memory journal records; no files written"
    }
  },
  "executed_checks": {
    "git_diff_check": "PASS",
    "draft_2020_12_schemas_meta_valid": {
      "result": "PASS",
      "count": 5,
      "note": "collection_responsibility is manifest policy data, not a JSON Schema."
    },
    "cleanup_outcome_contract": {
      "result": "PASS",
      "outcomes": ["not_started", "partial", "complete", "unknown"],
      "stale_abnormal_not_started_rule": "ABSENT from classification_rules.pass_requires and fail_requires"
    },
    "cleanup_history_classifier": {
      "result": "PASS",
      "executed_synthetic_cases": {
        "schema_valid_complete_no_intent_and_no_created_resource": "not_started",
        "schema_valid_complete_failed_cleanup": "partial",
        "schema_valid_complete_successful_cleanup": "complete",
        "empty": "unknown",
        "unknown_label": "unknown",
        "malformed_record": "unknown",
        "reordered_sequence": "unknown",
        "intent_without_outcome": "unknown",
        "truncated_without_journal_closed": "unknown",
        "created_resource_without_any_cleanup_intent_or_outcome": "unknown",
        "created_resource_without_its_own_cleanup_while_another_is_cleaned": "unknown"
      }
    },
    "supervisor_receipt_fixture": {
      "result": "PASS",
      "details": "A synthetic schema-valid receipt is accepted; an empty wait_receipts array is rejected."
    },
    "supervisor_pid_set_cross_artifact_validation": "NOT EXECUTED: semantic finalizer is absent; this receipt does not claim it as schema or fixture coverage."
  },
  "authorization": {
    "astra_submitted": false,
    "fresh_owner_exact_packet_sha_consent": false,
    "implementation_build_runtime_ipc_vm_d3": "NOT PERFORMED / NOT AUTHORIZED"
  },
  "limitations": [
    "The cleanup classifier and fixtures are documentary, bounded offline probes; they do not execute a runtime finalizer.",
    "The worktree remains uncommitted; SHA-256 pins bind the reviewed bytes, not a clean Git tree.",
    "This sidecar intentionally does not embed its own digest to avoid a self-referential hash cycle."
  ]
}


===== FILE: /home/gandalf/CodeRepository/infra-novagenesis-tools/verify_spec039_v14_r12.py — SHA-256 6a29bd9b24c2c60440383edeb1c89a14af04ce19ea7e25d564bcb1b211f8948c =====
#!/usr/bin/env python3
"""Read-only local contract verifier for SPEC-039 v1.14/r12."""
from __future__ import annotations

import copy
import hashlib
import json
import subprocess
from pathlib import Path

from jsonschema import Draft202012Validator

REPO = Path("/home/gandalf/workspace/novagenesis")
RESULTS = REPO / "Specs/RESULTS-SPEC-039"
SPEC = REPO / "Specs/SPEC-039-common-performance-measurement.md"
MANIFEST = RESULTS / "precode-manifest-v1.14-r12.json"
PACKET = RESULTS / "precode-packet-v1.14-amend-closure-20261012-sanitized-draft-r12.md"
R11 = RESULTS / "astra-review-v1.14-r11-20261012.md"
PINS = {
    "head": "756f493b7049434ab67cf2382486a48f606a470c",
    "spec": "0cda2743e797cfbed952a1bad7c30e8e0f15d37c57e51d70e3388b9453103115",
    "manifest": "63ccf50e6ecd19940f0c0b5e98d438ae6e34f5b2dbf0304faef97b5eca7c0917",
    "packet": "a88434f223c3b66635d2589140477c01c5142ae94ba0bb839d026c3fde8013bd",
}


def sha(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def strict_pairs(pairs: list[tuple[str, object]]) -> dict[str, object]:
    result: dict[str, object] = {}
    for key, value in pairs:
        if key in result:
            raise ValueError(f"duplicate JSON key: {key}")
        result[key] = value
    return result


def reject_constant(value: str) -> None:
    raise ValueError(f"non-finite JSON value {value}")


def reject(action, label: str) -> None:
    try:
        action()
    except Exception:
        return
    raise AssertionError(f"negative probe unexpectedly accepted: {label}")


def classify(events: object, journal_schema: dict[str, object]) -> str:
    """Classify only a complete, schema-valid, ordered journal stream."""
    if not isinstance(events, list) or not events:
        return "unknown"
    validator = Draft202012Validator(journal_schema)
    try:
        for item in events:
            validator.validate(item)
    except Exception:
        return "unknown"

    if [item["sequence"] for item in events] != list(range(1, len(events) + 1)):
        return "unknown"
    if any(events[i]["monotonic_ns"] < events[i - 1]["monotonic_ns"] for i in range(1, len(events))):
        return "unknown"
    if len({item["run_id"] for item in events}) != 1:
        return "unknown"

    labels = [item["event"] for item in events]
    if labels[0] != "JOURNAL_OPENED" or labels[-1] != "JOURNAL_CLOSED":
        return "unknown"
    if labels.count("JOURNAL_OPENED") != 1 or labels.count("JOURNAL_CLOSED") != 1:
        return "unknown"
    if labels.count("RUN_SHUTDOWN_COMPLETE") != 1:
        return "unknown"
    shutdown = labels.index("RUN_SHUTDOWN_COMPLETE")
    if any(label in {"CLEANUP_INTENT", "REMOVED", "CLEANUP_FAILED"} for label in labels[:shutdown]):
        return "unknown"

    created: set[tuple[object, object, object]] = set()
    pending: set[tuple[object, object, object]] = set()
    outcomes: dict[tuple[object, object, object], str] = {}
    for item in events[1:-1]:
        label = item["event"]
        resource = (item["resource_type"], item["candidate_key"], item["source_name"])
        if label == "CREATE_INTENT":
            if resource in created or resource in pending:
                return "unknown"
            pending.add(resource)
        elif label == "CREATED":
            if resource not in pending:
                return "unknown"
            pending.remove(resource)
            created.add(resource)
        elif label == "CREATE_FAILED":
            if resource not in pending:
                return "unknown"
            pending.remove(resource)
        elif label == "CLEANUP_INTENT":
            if resource not in created or resource in pending or resource in outcomes:
                return "unknown"
            pending.add(resource)
        elif label in {"REMOVED", "CLEANUP_FAILED"}:
            if resource not in pending:
                return "unknown"
            pending.remove(resource)
            outcomes[resource] = label
        elif label != "RUN_SHUTDOWN_COMPLETE":
            return "unknown"

    if pending:
        return "unknown"
    intent_count = labels.count("CLEANUP_INTENT")
    if intent_count == 0:
        return "not_started" if not created else "unknown"
    if len(outcomes) != intent_count or set(outcomes) != created:
        return "unknown"
    if any(outcome == "CLEANUP_FAILED" for outcome in outcomes.values()):
        return "partial"
    return "complete" if all(outcome == "REMOVED" for outcome in outcomes.values()) else "unknown"


def event(sequence: int, name: str, resource: str = "journal", key: int | None = None) -> dict[str, object]:
    return {
        "schema_version": "ng039-reservation-journal-event/v2",
        "run_id": "NG039-D2-0123456789",
        "sequence": sequence,
        "event": name,
        "monotonic_ns": sequence,
        "pid": 2345,
        "uid": 1001,
        "resource_type": resource,
        "candidate_key": key,
        "source_name": None,
        "error_number": None,
        "identity": None,
    }


def main() -> None:
    head = subprocess.check_output(["git", "-C", str(REPO), "rev-parse", "HEAD"], text=True).strip()
    actual = {"head": head, "spec": sha(SPEC), "manifest": sha(MANIFEST), "packet": sha(PACKET)}
    if actual != PINS:
        raise SystemExit("frozen pin mismatch: " + json.dumps(actual, sort_keys=True))

    manifest = json.loads(MANIFEST.read_text(), object_pairs_hook=strict_pairs, parse_constant=reject_constant)
    packet = PACKET.read_text()
    if sha(SPEC) != manifest["spec"]["sha256"] or sha(SPEC) not in packet or sha(MANIFEST) not in packet:
        raise AssertionError("SPEC/manifest/packet binding mismatch")
    if R11.read_text() not in packet:
        raise AssertionError("prior r11 response missing/not exact")

    schemas = [entry for entry in manifest["evidence_schemas"].values() if "$schema" in entry]
    assert len(schemas) == 5
    for schema in schemas:
        Draft202012Validator.check_schema(schema)

    rules = manifest["classification_rules"]
    stale_rule = "abnormal_evidence_only is INCONCLUSIVE, has cleanup_outcome=not_started and performs no IPC/cleanup/journal mutation"
    assert stale_rule not in rules["pass_requires"]
    assert stale_rule not in rules["fail_requires"]

    record_schema = manifest["evidence_schemas"]["run_record"]
    required_names = set(rules["required_artifact_names"])
    failure_schema = record_schema["properties"]["failure"]["oneOf"][1]
    missing_schema = failure_schema["properties"]["missing_artifacts"]
    unavailable_schema = failure_schema["properties"]["unavailable_artifacts"]
    assert len(required_names) == 8
    assert set(missing_schema["items"]["enum"]) == required_names
    assert set(unavailable_schema["items"]["enum"]) == required_names
    assert missing_schema["maxItems"] == unavailable_schema["maxItems"] == 8
    assert "harness-diagnostics.txt" not in missing_schema["items"]["enum"]
    abnormal = record_schema["allOf"][-1]["then"]
    assert set(abnormal["properties"]["failure"]["properties"]["cleanup_outcome"]["enum"]) == {
        "not_started", "partial", "complete", "unknown"
    }

    journal_schema = manifest["evidence_schemas"]["reservation_journal_event"]
    no_intent = [event(1, "JOURNAL_OPENED"), event(2, "RUN_SHUTDOWN_COMPLETE"), event(3, "JOURNAL_CLOSED")]
    success = [
        event(1, "JOURNAL_OPENED"), event(2, "CREATE_INTENT", "sysv_shm", 11),
        event(3, "CREATED", "sysv_shm", 11), event(4, "RUN_SHUTDOWN_COMPLETE"),
        event(5, "CLEANUP_INTENT", "sysv_shm", 11), event(6, "REMOVED", "sysv_shm", 11),
        event(7, "JOURNAL_CLOSED"),
    ]
    failed = copy.deepcopy(success)
    failed[5] = event(6, "CLEANUP_FAILED", "sysv_shm", 11)
    assert classify(no_intent, journal_schema) == "not_started"
    assert classify(success, journal_schema) == "complete"
    assert classify(failed, journal_schema) == "partial"
    for invalid in (
        [], ["UNKNOWN_EVENT"], [event(1, "JOURNAL_OPENED"), {"event": "JOURNAL_CLOSED"}],
        [event(2, "JOURNAL_OPENED"), event(1, "JOURNAL_CLOSED")], success[:-1],
        [event(1, "JOURNAL_OPENED"), event(2, "CLEANUP_INTENT", "sysv_shm", 11), event(3, "JOURNAL_CLOSED")],
    ):
        assert classify(invalid, journal_schema) == "unknown"
    created_without_cleanup = copy.deepcopy(success)
    created_without_cleanup.insert(4, event(4, "CREATE_INTENT", "sysv_shm", 12))
    created_without_cleanup.insert(5, event(5, "CREATED", "sysv_shm", 12))
    for sequence, item in enumerate(created_without_cleanup, 1):
        item["sequence"] = sequence
        item["monotonic_ns"] = sequence
    assert classify(created_without_cleanup, journal_schema) == "unknown"
    supervisor_schema = manifest["evidence_schemas"]["supervisor_termination"]
    supervisor = {"schema_version": "ng039-supervisor-termination/v1", "run_id": "NG039-D2-0123456789", "launcher_sha256": "a" * 64, "lease_path": "/run/lock/ng039/test.lock", "lease_device": 1, "lease_inode": 2, "process_tree_pids": [2345], "wait_receipts": [{"pid": 2345, "raw_wait_status": 9}], "stdout_closed": True, "stderr_closed": True, "stdout_sha256": "b" * 64, "stderr_sha256": "c" * 64, "timeout_fired": True, "captured_monotonic_ns": 10}
    Draft202012Validator(supervisor_schema).validate(supervisor)
    reject(lambda: Draft202012Validator(supervisor_schema).validate({**supervisor, "wait_receipts": []}), "empty supervisor receipts")

    assert manifest["local_validation_this_freeze"]["astra_submitted"] is False
    print(json.dumps({"identity": "PASS", "schemas": "5/5 meta-valid", "cleanup_classifier": "PASS", "classifier_cases": {"not_started": "PASS", "partial": "PASS", "complete": "PASS", "empty_unknown_malformed_reordered_truncated_created_without_cleanup": "unknown"}, "stale_abnormal_outcome_rules": "ABSENT", "cross_artifact_pid_set": "NOT EXECUTED", "astra_submitted": False, "writes_performed": False}, indent=2))


if __name__ == "__main__":
    main()


===== FILE: /home/gandalf/CodeRepository/infra-novagenesis-tools/test_spec039_v14_r12_classifier.py — SHA-256 b68c4299be7416aa26289d4d7c6628831b021294072d3091c81b10cc4984f80c =====
#!/usr/bin/env python3
"""Focused fail-closed probes for the r12 cleanup-history classifier."""
from __future__ import annotations

import importlib.util
import json
from pathlib import Path

MODULE = Path(__file__).with_name("verify_spec039_v14_r12.py")
spec = importlib.util.spec_from_file_location("verify_spec039_v14_r12", MODULE)
assert spec and spec.loader
validator = importlib.util.module_from_spec(spec)
spec.loader.exec_module(validator)


def journal_schema() -> dict[str, object]:
    manifest = json.loads(
        (Path("/home/gandalf/workspace/novagenesis")
         / "Specs/RESULTS-SPEC-039/precode-manifest-v1.14-r12.json").read_text()
    )
    return manifest["evidence_schemas"]["reservation_journal_event"]


def event(sequence: int, name: str, resource: str = "journal", key: int | None = None) -> dict[str, object]:
    return {
        "schema_version": "ng039-reservation-journal-event/v2",
        "run_id": "NG039-D2-0123456789",
        "sequence": sequence,
        "event": name,
        "monotonic_ns": sequence,
        "pid": 2345,
        "uid": 1001,
        "resource_type": resource,
        "candidate_key": key,
        "source_name": None,
        "error_number": None,
        "identity": None,
    }


def classify(events: list[object]) -> str:
    return validator.classify(events, journal_schema())


def expect(events: list[object], outcome: str) -> None:
    got = classify(events)
    assert got == outcome, f"expected {outcome!r}, got {got!r} for {events!r}"


def main() -> None:
    no_intent = [
        event(1, "JOURNAL_OPENED"),
        event(2, "RUN_SHUTDOWN_COMPLETE"),
        event(3, "JOURNAL_CLOSED"),
    ]
    successful_cleanup = [
        event(1, "JOURNAL_OPENED"),
        event(2, "CREATE_INTENT", "sysv_shm", 11),
        event(3, "CREATED", "sysv_shm", 11),
        event(4, "RUN_SHUTDOWN_COMPLETE"),
        event(5, "CLEANUP_INTENT", "sysv_shm", 11),
        event(6, "REMOVED", "sysv_shm", 11),
        event(7, "JOURNAL_CLOSED"),
    ]
    failed_cleanup = successful_cleanup.copy()
    failed_cleanup[5] = event(6, "CLEANUP_FAILED", "sysv_shm", 11)

    expect(no_intent, "not_started")
    expect(successful_cleanup, "complete")
    expect(failed_cleanup, "partial")
    created_without_any_cleanup = [
        event(1, "JOURNAL_OPENED"),
        event(2, "CREATE_INTENT", "sysv_shm", 11),
        event(3, "CREATED", "sysv_shm", 11),
        event(4, "RUN_SHUTDOWN_COMPLETE"),
        event(5, "JOURNAL_CLOSED"),
    ]
    expect(created_without_any_cleanup, "unknown")

    # Absence, unknown labels, malformed events, incomplete transactions and
    # any sequence/order violation fail closed as unknown.
    expect([], "unknown")
    expect(["UNKNOWN_EVENT"], "unknown")
    expect([event(1, "JOURNAL_OPENED"), {"event": "JOURNAL_CLOSED"}], "unknown")
    expect([event(2, "JOURNAL_OPENED"), event(1, "JOURNAL_CLOSED")], "unknown")
    expect([event(1, "JOURNAL_OPENED"), event(2, "CLEANUP_INTENT", "sysv_shm", 11), event(3, "JOURNAL_CLOSED")], "unknown")
    expect(successful_cleanup[:-1], "unknown")
    created_without_cleanup = successful_cleanup.copy()
    created_without_cleanup.insert(4, event(4, "CREATE_INTENT", "sysv_shm", 12))
    created_without_cleanup.insert(5, event(5, "CREATED", "sysv_shm", 12))
    for sequence, item in enumerate(created_without_cleanup, 1):
        item["sequence"] = sequence
        item["monotonic_ns"] = sequence
    expect(created_without_cleanup, "unknown")
    reversed_cleanup = successful_cleanup.copy()
    reversed_cleanup[4], reversed_cleanup[5] = reversed_cleanup[5], reversed_cleanup[4]
    expect(reversed_cleanup, "unknown")


if __name__ == "__main__":
    main()


===== FILE: Specs/SPEC-039-common-performance-measurement.md — FULL current bytes, SHA-256 0cda2743e797cfbed952a1bad7c30e8e0f15d37c57e51d70e3388b9453103115 =====
# SPEC-039: Gateway Measurement Baseline on AIOPT5

**Version:** v1.14
**Date:** 2026-10-12
**Author:** Hermes; owner-directed same-SPEC documentary amendment
**Status:** In Progress
**Gate:** D0 remains limited to Q0/Q1. The v1.10/r07 checker amendments received documentary `APPROVE` only. The v1.11/r08 host-profile packet received `AMEND`; follow-up withdrew B1 as a sender-envelope typo and confirmed B2/B3. Astra reviewed v1.12/r09 and v1.13/r10: r09's IPC-boundary contradiction is resolved, while r10 identified one remaining B1—abnormal termination requires an `INCONCLUSIVE` record even when normal joins/destruction cannot be proved. This v1.14 same-SPEC amendment defines separate normal-cleanup and abnormal-evidence-only finalizer modes. It is unreviewed; D2 remains blocked pending fresh exact-SHA Astra review, provisioned prerequisites and a separate owner execution decision. D3 is not authorised.
**Branch:** AIOPT5
**Baseline commit:** `1af604dad66148618cc0e578c28adb4a9829e607`
**D0-approved v1.1 draft SHA-256:** `8026e1320149c318a5218eaa36c730f262081a5bbe0c2d582b23c307c7144e1c` (owner approval: 2026-10-04; documentary Q0/Q1 scope only; does not approve this v1.14 amendment)
**Owner task:** NG-040, under NG-039
**Related:** `SPEC-032-gw-input-queue-batching.md`, `SPEC-033-message-commandline-refactor.md`

> v1.14 is the same-SPEC documentary response to the sole r10 B1 blocker. It retains the r09/r10 finalizer IPC boundary and r08 B2/B3 controls, while separating two finalization paths: normal cleanup requires a durable shutdown-complete journal event plus confirmed process termination/log closure; abnormal evidence-only finalization may write only a final `INCONCLUSIVE` record after every recorded process is reaped and logs are closed, and performs no IPC inspection or destructive cleanup. Process exit is not proof that workers joined or `Process/GW` was destroyed. The dedicated launcher/lease remains unprovisioned, D2 is blocked, and fresh exact-SHA Astra review plus a separate owner execution decision are required. D3 remains unauthorised.

> This AIOPT5 Proposal uses the existing SPEC-039 number; it is not a restoration of the archived AIOPT3 Proposal or its approval. The archived copy was deliberately set aside during the NG-050 release freeze; its SHA-256 is `c46dd50a6d58c760eba165a8dfdc3e390df41e8049cf0f95527655350c80da4f`. Keep that copy intact. The v1.1 D0 approval was documentary only. Later, separately scoped owner permissions are recorded in §4; they do not transfer to other gates or authorise a performance claim.

## 1. Problem and decision

The owner has paused SPEC-027 RAW multi-worker reception and prioritised measuring the Gateway (`GW`) without changing `PGCS` or `PG`. The old Common performance plan and its measurements belonged to AIOPT3 and cannot serve as an AIOPT5 before/after baseline. First establish whether the GW scheduling, network-buffer consumption, or shared-memory (SHM) paths consume material time or strand work. Optimisation selection is a later decision, not a premise of this SPEC.

The committed AIOPT5 `GW.cpp` computes a wait of at most 1 ms and uses `!InputQueue.empty()` as a condition-variable predicate (`Gateway`, near lines 488–515). A future-dated head leaves the queue non-empty and is not popped until its scheduled time is strictly less than `GetTime()` (near lines 523–552). This is a source-level busy-loop candidate, not a measured dominant bottleneck. The Gateway drains the network receive queue under its mutex before attempting a SHM read (near lines 658–763); the relative contribution and fairness of those stages remain unmeasured.

## 2. Scope and immutable starting point

- Current first target: characterise one existing GW consumer (`N=1`) in two conditions only: idle and future-only. Do not infer RAW receive workers from `N=1`. Due-work, network-only and mixed network/SHM conditions are explicitly deferred until the first result justifies a second, separately approved measurement slice.
- For a future execution, use an isolated checkout at the exact baseline commit and record whether any test harness or diagnostics alter that tree. The current AIOPT5 checkout has staged and unstaged changes, including SPEC-027 telemetry in `GW.cpp`/`GW.h`; neither those changes nor the failed 2026-10-02 diagnostic run are a clean comparator. Do not reset, stash, overwrite, or silently transplant them.
- Preserve message bytes, inverted pub/sub, message lifetime, one-due-message-per-iteration scheduling, strict due-time test, priority, output retry semantics, SHM layout/semaphores, and process teardown. `SPEC-032-gw-input-queue-batching.md` remains Deferred.
- Exclude changes to `PGCS`/`PG`, RAW receive concurrency, remote VMs, release/Phase 0 acceptance, full Common/SAR/HT benchmarking, and a GW algorithmic patch. The archived SPEC-041 is a hypothesis and historical Proposal only, not an approved AIOPT5 implementation contract.

## 3. D0-approved measurement design (execution not authorised)

A run is one fresh, isolated local process start with one GW consumer and a fixed, recorded workload. Requests/messages within the same run are not independent repetitions. The initial Q0/Q1 proposal is five independent runs per condition, each with 30 s warm-up, 120 s measured interval, and up to 30 s bounded drain; the owner may revise these constants before authorising execution, but they must then be frozen in a manifest. Never reuse dirty caches, live trial locks, or existing IPC resources at the D2 candidate keys/names.

First gate only, on the same executable and fixed environment:

| Condition | Input | Primary diagnostic question |
|---|---|---|
| Q0 idle | No offered application messages | CPU and polling floor |
| Q1 future head | One message scheduled well beyond the measured window | Immediate returns/cycles while waiting |

Q2 (due-work service), Q3 (network-only publication) and Q4 (mixed network/SHM fairness) remain named questions, not planned runs or acceptance gates. They need a separate owner decision after Q0/Q1. The harness must document how it reaches the real GW path, validates the future message without executing it early, and creates/cleans its own local resources. D0 v1.1 did not authorise source edits; the later, narrowly scoped D2 seam permission is recorded in §4 and does not allow algorithm changes or work outside the frozen test seam.

Q0's zero application-offer stimulus is not proof that `InputQueue` is empty: Process/GW construction runs initialization and can queue bootstrap or periodic messages. Record observed queue state separately; do not claim a true idle floor unless the run proves no queued head or explicitly labels Q0 as “no application offers.”

For every run, capture the baseline SHA and source-tree identity, compiler/libc, effective per-target flags/macros, executable and configuration hashes, workload inputs/seed/rate/payload distribution, CPU allocation, log mode, wall-clock duration, CPU time, peak RSS, and exit/cleanup status. Use separate quiet `-O0` and quiet verified `-O2` builds if approved; their difference is a build-profile effect, not an algorithmic GW gain. Sanitizer builds are correctness diagnostics, never timing comparators. Keep logs bounded; disable per-message DEBUG in timed windows.

For Q0/Q1, independently observe GW cycles, wait calls, immediately returned waits with a future head, blocked duration and wake reasons, process CPU time, peak RSS, SHM poll attempts and maximum inter-poll gap, future-message offered/admitted/pending/executed counts and cleanup status. A future message must not execute within the window; reconcile its pending state. Define each counter's increment point, owner and unit. Missing boundary observations mean `INCONCLUSIVE` for that claim, not zero. First inventory existing counters and their actual update/emission sites; do not assume the dirty SPEC-027 header or a previous smoke supplies them. If Q0/Q1 indicate a material issue, a later approved slice can add due-work completion, queue residence, output retries and network/SHM fairness metrics.

Acceptance of the *Q0/Q1 measurement* gate requires a frozen manifest, repeatable harness/inputs, all planned run outcomes including failed/incomplete runs, raw data plus a small analysis script, source/binary/configuration bindings, and a written diagnosis of future-head waiting versus idle. Report run-level distributions and uncertainty rather than treating cycles as independent runs. This limited result cannot identify the dominant end-to-end bottleneck. Do not call an optimisation beneficial or instrumentation overhead ≤2% without comparable quiet and instrumented measurements that support the specific claim.

## 4. Gates and permissions

1. **D0 — Documentary review:** v1.1 was approved by the owner on 2026-10-04 for Q0/Q1 only, against the v1.1 digest above. This approval does not transfer to other scopes or confer execution authority.
2. **D1 — Local preparation:** pin the exact functional checkout, define the real production-linked fixture, freeze commands/manifest, and obtain scoped authorization for any test-only code/build. Preserve the dirty primary AIOPT5 tree; do not transplant its unrelated changes. The unavailable namespace profile is not replaced by an immediately runnable host profile: §7.1 now requires a provisioned dedicated single-run UID/launcher, otherwise D2 remains blocked before IPC creation.
3. **D1-H — Compile/link probe:** owner-authorised source/build-only probe in an isolated checkout at `1af604dad66148618cc0e578c28adb4a9829e607`. It passed, but the executable was not run. This proves symbol/type linkage only—not GW construction, Q0/Q1 behavior, correctness, or performance.
4. **D2a — Correctness oracle and testability:** the owner authorised a compile-time-gated test seam in `Common` and one local D2 correctness run, with no algorithm change, D3 timing, `PGCS`/`PG`, VM, commit or push. Freeze the exact files, oracle, host-controlled resource-attribution procedure, command and cleanup procedure in the pre-code packet. The current public API does not expose queue contents or pending/execution state; `GW::Gateway()` starts an output worker and touches IPC. For Q1, construct one Process-owned `Message` with an in-memory future `Time` and offer it through the real public `GW::PushToInputQueue` path. Do **not** use `NGAL_CS::DeliverToGateway` for this future-head stimulus: `GW::Gateway()` creates received messages with `NewMessage(0, ...)` (`GW.cpp:614–623`), while wire serialization emits command lines rather than the local scheduling `Time` (`Message.cpp:1599–1602`). The D2 run therefore tests queue admission/scheduling, not NGAL receive/deserialization, wire-format timing, or network delivery. Use three inert, syntactically shaped command lines whose action tuple is not registered in GW; this satisfies `NoCL > 2` (`GW.cpp:241–263`) while ensuring an unexpected early `Run` has no external action.

   The compile-time-gated observer exposes `GWQueueTestObservation` through a test-only scalar-copy accessor; it never retains a `Message*`. The snapshot includes a 64-bit event sequence, validity, one-way Gateway-start/stop state, registered target ID, event message ID, observed queue depth/head identity/head time, actual wait timeout/return, the existing strict due decision and actual pop result, sticky target admission/non-due/pop/Run entry/return counters, monotonic empty-queue count, overflow state, and a first-target-wait record containing its own sequence, target/head identity, head time/depth, timeout, wait return and due result. Value-initialise storage as `GWQueueTestObservation D2Observation{}`; `None` is the zero enum value. `RegisterD2Target` runs once before Gateway start, locks `InputQueueMutex`, rejects ID zero or a started Gateway, resets/binds the observation, and leaves it invalid until the first real event. Set `gateway_started=true` under that mutex before entering the loop; never clear it or allow re-registration. Writers increment sequence, update scalar data and counters, then publish validity; counters saturate and set overflow rather than wrap. At the first real `wait_for` return with the registered target as head, fill the sticky first-target-wait record once; later events cannot overwrite it. At Q0/Q1 start the harness captures sequence/counter baselines and tests for new events before the window ends. Writers at already-locked admission/wait/pop sites use a non-locking helper; Run/stop boundaries take the mutex only outside those scopes. No I/O, callbacks, or test-only symbols/state/calls in the default build.
5. **D2b — Correctness test code/build/run:** only after this v1.14 host-controlled profile is positively reviewed under fresh exact-SHA consent, the dedicated-identity preconditions are provisioned and verified, and a separate owner execution decision is recorded, implement only the frozen allowlist: root `CMakeLists.txt`; `Common/src/GW.h`; `Common/src/GW.cpp`; new `tests/spec039/gw_q0_q1_d2.cpp`; and new `tests/spec039/q0_q1_d2.manifest.json`. Add the separate option `NG_ENABLE_NG039_GW_Q0_Q1_D2` (default `OFF`) and target `ng039_gw_q0_q1_d2`; define `NG_GW_QUEUE_TEST_OBSERVATION` for `Common` and the harness only inside an isolated diagnostic target graph: the option-on root CMake path must declare only `Common` and this harness and return before production application targets, with a configure-time target-census guard rejecting any other target. Keep the existing D1-H harness/manifest as historical evidence and do not overwrite them. Gate observer declarations, storage, methods and production call sites together. In the diagnostic build only, guard all three pinned `GW.cpp` branches that call `sem_unlink` after `sem_open` failure (`GW.cpp:970,1263,1322`): they must record a bounded failure event and must not call `sem_unlink`; the macro-off default build preserves existing behavior. Add targeted tests proving each injected `sem_open` failure path performs no unlink. This safety change is specified, not authorized for implementation by this document. Build the D2 target in a fresh isolated checkout at the pinned functional baseline. The dedicated-UID launcher and lease must be pinned and verified before any D2 action; until provisioned, no launch command is authorized and the old direct `timeout <harness>` invocation is prohibited. The manifest must freeze exact launcher/lease identity, run/finalizer commands, normal-versus-abnormal mode predicates, the `supervisor-termination.json` schema, and process-tree bounds before the separate execution gate. Resolve all path aliases before any command. Keep worktree, build directories, HOME/TMPDIR and results outside `.hermes`; use only named workspace/repository roots. The harness spawns no child processes.

   The trusted dedicated-UID launcher and exclusive lease must be verified before any IPC action; the direct host-UID launch command is prohibited. Only then capture complete host inventories for `/proc/sysvipc/{shm,sem,msg}` and `/dev/shm`; host inventories may be non-empty, but all five candidate key/name pairs must be absent. Derive and record `D2_PROCESS_KEY` and selected attempt using §7.1's exact algorithm; candidate keys are `{11, D2_PROCESS_KEY, D2_PROCESS_KEY+1, D2_PROCESS_KEY+2, D2_PROCESS_KEY+3}`. Embed exact raw `/proc/sysvipc` bytes (canonical base64) in each inventory artifact; finalizer must recompute raw digests and normalized summaries from those bytes. Create and fsync `reservation-journal.jsonl` before reservation. Abort before construction on unreadable/over-cap inventory, candidate collision, key overflow, missing lease, or ambiguity. Then exclusively reserve all five SHM segments and five POSIX semaphores and journal each intent/outcome. NovaGenesis constructors may open only these reserved resources. A collision, mode mismatch, unverified returned-handle/libc mapping, incomplete journal or failed reservation blocks construction/PASS; do no speculative cleanup.

   After Gateway/output threads stop and join and `Process/GW` are destroyed, retain the dedicated-UID lease through finalization. Remove only exact run-created SHM IDs/names whose successful journal receipts and current identities match. Name-based `sem_unlink` is permitted only while the verified exclusive execution domain remains held; if the lease is lost, another process appears, or identity/journal state is ambiguous, perform no unlink/RMID for the disputed object and preserve residue. Append/fsync each cleanup intent and outcome; an intent without a durable outcome is `INCONCLUSIVE`, never evidence of removal. Capture final inventory, including exact raw table bytes, and require it to match pre. Any failed cleanup, residual candidate, unexplained change, blocked join, timeout or ambiguous exit is `INCONCLUSIVE`; no broad cleanup. The finalizer verifies and hashes the journal and all bundle artifacts. Keep D2 at `N=1`, Q0/Q1 correctness only; no performance claim, algorithm change, `PGCS`/`PG`, VM, commit or push unless separately authorized.
6. **D3 — Characterisation:** Q0/Q1 timing is a separate gate, allowed only after D2 passes and the owner explicitly authorises the frozen measurement plan. Preserve all run outcomes and identities; classify the future-head hypothesis `MEASURED`, `INCONCLUSIVE`, or `BLOCKED`.
7. **D4 — Follow-up decision:** decide whether to stop, approve another measurement slice, or propose a performance implementation SPEC. Any future change requires a predeclared baseline/metric, actual-source review, correctness controls and its own owner-approved implementation allowlist. Astra or subagent opinions never grant implementation or runtime authority.

### 4.5 GPT-6 Astra consultation checkpoints (advisory; no execution authority)

| Checkpoint | Packet and question | Gate informed |
|---|---|---|
| Before D2 test-seam/code | Exact SPEC/manifest/packet hashes, functional baseline, D1-H manifest, proposed observer/interface diff and isolated CMake target graph; the dedicated-UID launcher, exclusive lease, `supervisor-termination.json` wait receipts and exact process-tree evidence; all three guarded `sem_unlink` paths; journal event schema/state transitions; raw `/proc/sysvipc` retention/recomputation and exact returned-`sem_t`→libc/dev-shm inode mapping. Ask whether the trust boundary actually excludes same-UID mutation from pre-inventory through finalizer, whether any failure path can still unlink an unowned object, whether crash/replay semantics forbid ambiguous cleanup, whether normal-cleanup and abnormal-evidence-only finalizer modes are mutually consistent, whether process-tree wait receipts and closed logs suffice to authorize an INCONCLUSIVE-only no-cleanup path, and whether the schemas can support a non-vacuous PASS without claiming unverified evidence. | The frozen v1.10/r07 B1/B2/B3 packet received `APPROVE` only for its documentary findings. The immutable v1.11/r08 packet `3295a1bffd234673aaa82e2e7e254f3d5bad6c1e747f4d18123f702510445272` received `AMEND`; follow-up withdrew B1 (sender-envelope typo) and retained B2/B3. The v1.12/r09 packet returned `AMEND` on the IPC boundary; the v1.13/r10 packet closed that issue but returned `AMEND` on abnormal finalization. This v1.14 response requires a fresh exact-SHA Astra review after manifest/packet freeze. Review is advisory and cannot authorize D2 or expand owner scope. |
| After D2 correctness result, before D3 | Frozen diff/build/test manifest, all raw outcomes including failures, identities and cleanup evidence. Ask whether the D2 assertions are supported and what remains inconclusive. | Planned checkpoint; a separate Astra submission requires owner consent. The owner decides whether to accept D2 and authorize D3. |
| After D3, before any implementation SPEC | Frozen raw data, analysis, source/build/workload identities and hypothesis classification. Ask whether the evidence supports a future implementation proposal and its acceptance metric. | Future checkpoint only; separate owner consent is required. Astra advice never authorises production changes. |

The owner authorised one GPT-6 Astra review via OpenAI Codex for the frozen v1.2 pre-code D2 packet. That call returned HOLD on the v1.2 SPEC SHA-256 `8973948334538715a9e02474be7b1d88a24e6b9235339870ba126d0570d91` and packet SHA-256 `f29274fa1fa38701ee8a5b9c4ae47366dee9e8f75e372dac826541c57ccca942`; its consent was consumed. On 2026-10-05 the owner separately authorised one review of the sanitized v1.4 packet SHA-256 `ba4ec6ec4b6660e98c3e7d0cdbbd6f18b7945ab6f6bdd30c2294e6c654150948`, bound to SPEC v1.4 SHA-256 `f54901cad03209f94fe98cfad3bf62953895ed24fef3d771a89b89f5229217e1` and baseline `1af604dad66148618cc0e578c28adb4a9829e607`. Astra returned AMEND, response ID `resp_0a12ead61fee5339016ac370498edc87d28ec6212dafa33cf9`. The archived raw SSE SHA-256 is `25b8c29265d327c34323e12f2c35177c2069c13b1d7e99f83a42ffea75240a58`; archive manifest SHA-256 is `46f41c9af22a6b1c9e86e2a66682dfd83f7323fb25d1c47aded2aa52c95e6782`. The r03 AMEND is advisory; its one-call consent is consumed and grants no implementation, build, runtime, IPC/VM, or D3 authority. Raw SSE SHA-256 `14f95d55a4a5611f518a0e236c98edd255e6809acd256b105177f975577bef09`; parsed-response SHA-256 `6fae28a46fd385e0d5aa87e8a1bc7149c832fcd8878170b470dab558941bc587`; archive manifest SHA-256 `b26c7767c9a74f2c9ee716a59257f79064c5480191addbc5f61da998989ca5fd`. The packet’s “NOT submitted” line is a time-frozen pre-review snapshot; this paragraph records the later submission/result. The v1.6 r04 packet SHA-256 `754ad970c1bb5e915262e825856345416360154274ece78ae80642ca6a30d829`, tied to the v1.6 SPEC SHA-256 `3296530c13801de945ed97b920532533c1a4a44d628075a9c5e61cdcc7ee01cb` and baseline `1af604dad66148618cc0e578c28adb4a9829e607`, was reviewed once on 2026-10-05 and returned AMEND (response ID `resp_0d91609fd866592e016ac3b3b903e487d2891f1ae2407fb075`). Raw SSE SHA-256 `a1c62af8a4e35a59aec3428fdf45818f2dd89b3f57dce9c3bf8be9a6279fe08d`; parsed-response SHA-256 `3655369b4c6c348e439ec87e48f6a8c1e789c46d5d4500890bee8577b8a969d3`; archive manifest SHA-256 `3f6dbf25e22ffd3ce5ce9290935fd41db162fd81aa67923c445b5ea250fbe2a8`. That exact-SHA consent is consumed. The review closed r03 findings 3 and 4b–4d at design level only; finding 4a remains must-fix. The v1.7 r05 packet SHA-256 `1b45bd2838f82686414257a9fe1c4cec0945b1faba04d59170d99e0e3367508a`, tied to SPEC v1.7 SHA-256 `b73bcc7361bb797b4ddd40952860f0092fb840bb11b31bf0a99659bef8475b70` and baseline `1af604dad66148618cc0e578c28adb4a9829e607`, was reviewed once on 2026-10-05 and returned AMEND (response ID `resp_066534905404d5a5016ac3b9c91aac87d2b521fcde5b122863`). Raw SSE SHA-256 `3d375e4cc4551793dff18a299e3e82ded99bf631a5fdfb0aed195a4dae24dc31`; parsed-response SHA-256 `9b0fa2aa410d6055e50da31134e42af29d97ece12aac77d971299ece7e3851dc`; archive manifest SHA-256 `27eabdd3b469b7044f9ed4935058ffe509f3a92712371924cc73b76dd619266a`. Usage: 22,569 input / 1,650 output / 24,219 total tokens; 8.11725 estimated credits (not USD); quota at return 5h 51%, weekly 86%. Three 4a must-fixes remain: preprocessor-forwarded and function-like observer macros; `-ansi` dialect override; Python assertions disabled by `PYTHONOPTIMIZE=1`. Optional hardening: actual `-o` target mapping, dynamic-linker conflict rejection, and stronger run-record schema `allOf`. Findings 3 and 4b–4d remain design-level closures only. The r05 consent is consumed. SPEC-039 v1.8 is local/unreviewed; any r06 review requires fresh exact-SHA consent. D2 remains blocked.

No VM/SSH/QGA operation, algorithmic production change, commit, push, or release is authorised by these gates. All Astra reviews to date were separately owner-authorised by exact packet SHA and their one-call consents are consumed; none expands the D2 seam/runtime scope. The old NG-062 trial locks and evidence remain untouched.

## 5. Files and evidence

| Path | State/intent |
|---|---|
| `Specs/SPEC-039-common-performance-measurement.md` | v1.14 same-SPEC response to the r10 abnormal-finalization blocker; r08 B2/B3 and r09 IPC-boundary protections retained. Documentary only. Dedicated-UID launcher and diagnostic no-unlink behavior remain unimplemented/unprovisioned. Fresh review and separate execution authorization required. |
| `Common/src/GW.cpp`, `Common/src/GW.h` | Read-only under D0; a D2 seam may touch only the explicitly frozen compile-time-gated scope in §4. |
| D2 proposed allowlist (not implemented) | `CMakeLists.txt`, `Common/src/GW.h`, `Common/src/GW.cpp`, new `tests/spec039/gw_q0_q1_d2.cpp`, and `tests/spec039/q0_q1_d2.manifest.json`; separate opt-in option defaults OFF. No modifications to NGAL, `Process`, `PGCS` or `PG`, no algorithm change, separate source helper file, or D3 timing files are proposed. The D2 harness may construct the existing `Process/GW` under test; the same allowlisted harness binary's `--finalize` mode runs after the test exits without constructing `Process/GW`, creating IPC resources, or sending/receiving production messages. In `normal_cleanup` mode, it requires a durable `RUN_SHUTDOWN_COMPLETE` journal event proving worker joins and `Process/GW` destruction, plus a validated required `supervisor-termination.json` artifact containing one raw `waitpid` receipt per recorded process and stdout/stderr closure state/hashes; only then may it read inventories, inspect exact identities, and perform journal-authorized `IPC_RMID`/`sem_unlink` under the continuously verified lease. In `abnormal_evidence_only` mode, only while trusted launcher, execution/result-directory identities and lease remain valid and a valid supervisor receipt proves every process reaped and logs closed, the finalizer may hash raw, safely readable allowlisted evidence without requiring the complete bundle or parsing/replaying non-control artifacts. It writes `INCONCLUSIVE`; `failure.missing_artifacts` and `failure.unavailable_artifacts` are disjoint, accurate sets; cleanup history is exactly one of `not_started`, `partial`, `complete` or `unknown`, derived from valid evidence and never forced to `not_started`. It performs no IPC inspection, cleanup, journal mutation or retry. If lease, receipt, process/log proof or result-directory authority is unconfirmed, leave the pending record and report `BLOCKED/INCONCLUSIVE`. The trusted dedicated-UID launcher/lease is a provisioned external runtime prerequisite, not part of the five-file code allowlist; it does not exist here and direct host-UID execution is prohibited. |
| Isolated D1-H artifacts | Historical manifest `tests/spec039/q0_q1.manifest.json` SHA-256 `9a7c4cda741e81192642053c4bdc60c3a5c0a46afcfa6c658ff2e2324f24225a`; source file hashes match. A fresh compile/link-only rebuild on 2026-10-10 used the unchanged isolated baseline checkout and an output root under `workspace/ng039-d1h-rebuild-20261010/` (outside `.hermes`): configure/build exit 0, CMake 3.28.3, GNU C++ 13.3.0, binary SHA-256 `060266a24546bc9a8e77ec7b0af409cb480bae770e56f65e95f2727877e5cc7d`, configure-log SHA-256 `65fb7c5ef0f1613c7aa181794678203c1d70d0c1313f8b20a51b471ef8330feb`, build-log SHA-256 `658cdeec0de9d743438c2683f0bca710b3a7b53302374303793a7838d3246e98`. This is a new build, not recovery of the old binary; the executable was not run, and this proves no correctness/performance behavior. |
| `Specs/RESULTS-SPEC-039/` | Contains historical r06/r07/r08 review packets and responses plus the D1-H build-only receipt; none is a D2/D3 runtime result. The v1.12/r09 and v1.13/r10 packets were submitted and returned `AMEND`; the v1.14/r11 draft is documentary only. No correctness or timing result exists. Historical AIOPT3 results remain in the NG-050 archive. |
| Obsidian NG-039 / NG-040 | Scope and blockers; neither an experimental oracle nor build evidence. |

## 6. Rollback and open questions

If the approved D0 design is later abandoned, mark this Proposal as abandoned/superseded instead of deleting it; leave the archive, current AIOPT5 diffs and historical trial evidence untouched. Later test-only changes must have an explicit allowlist and a reversible isolated checkout.

Before D2 code: freeze the compile-time-gated observer, exact five-file allowlist, in-memory future-message admission through `PushToInputQueue`, diagnostic no-unlink behavior, dedicated-UID launcher identity/lease, journal/inventory schemas and §7.1 cleanup oracle in a new pre-code packet. Obtain fresh exact-SHA consent before Astra. Do not apply the diff until positive documentary review and a separate execution decision; launcher provisioning is also a distinct prerequisite. Q0 distinguishes zero application offers from bootstrap queue state; Q1 uses a future `Message::Time` created locally because wire serialization does not carry that scheduling field. Cleanup must reconcile the Process-owned pending test message only after Gateway/output workers stop. The owner’s D2 scope is one correctness-only local run and does not include NGAL ingress coverage, D3 or algorithm changes. Q2–Q4 remain deferred; do not import approval or measurements from archived AIOPT3 SPECs.

## 7. D2a closure amendment — v1.8–v1.14 (documentary; not implementation authorization)

This section preserves the unchanged D2 correctness scope and the r08–r10 review history. The v1.12/r09 packet's IPC-boundary B1 was resolved in v1.13/r10. Astra r10 confirmed r08 B2/B3 and the r09 IPC fix, but identified a new B1: normal finalizer prerequisites conflict with the required failure-path `INCONCLUSIVE` record. This v1.14 text separates normal cleanup from abnormal evidence-only finalization. No D2 code, configure/build, runtime or IPC is authorized by this documentary amendment.

### 7.1 Host-controlled execution profile — v1.14 (documentary; execution blocked)

The v1.11 direct launch under the ordinary host UID is prohibited for D2. Mode `0600`, umask `0077`, exclusive creation and a pre-inventory do not prevent a same-UID process from replacing a POSIX semaphore name or removing/reusing a SysV resource during the run. This amendment does not claim the current host can satisfy the stronger prerequisite. No D2 configure/build, harness/finalizer execution, IPC creation/removal or measurement may occur until the dedicated single-run identity and launcher are provisioned, independently inspected, pinned in the manifest, and separately authorised by the owner. If they cannot be provided, the D2 execution profile remains `BLOCKED` before any candidate resource is created.

The run and its sequential finalizer must execute under a dedicated non-login UID reserved solely for this D2 run, through a trusted owner-controlled single-run launcher. The launcher must prevent another process from entering that UID's execution domain from before pre-inventory until finalization/lease release, hold an exclusive run lease across launch and finalization, and record its exact path, file identity/hash, UID/GID, lease identity and process-tree identity. A process census alone is not proof of exclusivity. The ordinary interactive UID, an unverified cooperative lock, or a missing launcher/lease is insufficient. Only controlled supervisor/harness/finalizer descendants for this run may use the UID; no login shell, unrelated service or concurrent run may use it. Treat host root/administrator as trusted; if root-level concurrent mutation is in the threat model, this shared-host profile is unacceptable and remains blocked. Record precondition evidence in the run record. Loss or uncertainty of identity/lease is `INCONCLUSIVE`, prohibits further name-based unlink/RMID, and preserves residue for owner-directed recovery. The launcher is outside the five-file code allowlist and has not been provisioned or authorised by this documentary amendment.

The three pinned `GW.cpp` paths at lines 970, 1263 and 1322 call `sem_unlink(name)` when `sem_open` fails. In the future diagnostic build only, the already-listed `NG_GW_QUEUE_TEST_OBSERVATION` compile-time guard must suppress all three unlinks and emit a bounded failure event; the normal macro-off build remains unchanged. Failure to open a semaphore aborts the diagnostic run; only the separate finalizer may clean a resource, and only from a complete reservation journal under a still-valid dedicated-UID lease. Synthetic fault-injection coverage must prove each path makes zero `sem_unlink` calls. This required source behavior is design text only; code changes still need separate owner authorisation.

Before constructing `Process/GW`, capture complete host-visible inventories for `/proc/sysvipc/shm`, `/proc/sysvipc/sem`, `/proc/sysvipc/msg`, and `/dev/shm`. Global inventories may be non-empty, but every candidate key/name must be absent. Capture each raw `/proc/sysvipc` table byte-for-byte in canonical base64 fields inside its phase inventory artifact; the finalizer decodes those bytes, recomputes each SHA-256 over the exact raw bytes, parses the table using the pinned Linux table grammar, and independently recomputes normalized rows/IDs before comparing them to stored summaries. Reject non-canonical base64, duplicate/invalid rows, digest mismatch, parse mismatch, unreadable/over-cap input or any inventory artifact exceeding its bound as `INCONCLUSIVE` before resource creation. The `/dev/shm` structured listing is sorted by bytewise name and retains each entry's kind, device, inode, UID/GID, mode, link count and size; the finalizer compares actual post/final listings with captured records. Inventory incompleteness never means “empty”.

Choose `D2_PROCESS_KEY` deterministically: for attempts `0..31`, compute `0x40000000 + (uint32_be(SHA256(ASCII(run_id + ':' + decimal(attempt)))[0:4]) mod 0x3FFFFFF8)` and select the first base whose four-key range is unoccupied and fits positive signed 32-bit `key_t`; key `11` is checked separately. Record the selected attempt and candidate set `{11, D2_PROCESS_KEY, D2_PROCESS_KEY+1, D2_PROCESS_KEY+2, D2_PROCESS_KEY+3}`. If no base is available, abort `INCONCLUSIVE`.

Before any candidate create, create `reservation-journal.jsonl` in the run result directory using exclusive non-symlink creation and mode `0600`; fsync the file and containing directory. This file is a required runtime artifact, capped at 256 KiB and 128 events, one strict JSON object per newline-terminated record, with monotonically increasing sequence numbers and the same run ID; use `ng039-reservation-journal-event/v2` for the event schema. Each event contains schema version, sequence, event kind, monotonic timestamp, process PID/UID, resource kind, candidate key/source name, and nullable identity/result fields. Event kinds are `JOURNAL_OPENED`, `CREATE_INTENT`, `CREATED`, `CREATE_FAILED`, `RUN_SHUTDOWN_COMPLETE`, `CLEANUP_INTENT`, `REMOVED`, `CLEANUP_FAILED`, and `JOURNAL_CLOSED`. Append/fsync `RUN_SHUTDOWN_COMPLETE` exactly once only after all Gateway/output workers have stopped and joined and `Process/GW` has been destroyed; it must precede every `CLEANUP_INTENT`. Write and fsync `CREATE_INTENT` before each `shmget`/`sem_open`; after the call, append and fsync exactly one `CREATED` record with returned identity or `CREATE_FAILED` with errno. Keep the descriptor open, verify its file identity before each append, and never rewrite/truncate prior events. In normal cleanup, the finalizer appends/fsyncs each `CLEANUP_INTENT` before cleanup and `REMOVED`/`CLEANUP_FAILED` after. It may close the journal only after final inventory capture and all events are durable. Abnormal evidence-only finalization never appends to, repairs or closes this journal. A missing event, sequence gap/duplicate, partial line, identity mismatch, crash between intent and outcome, or failure to fsync makes the run `INCONCLUSIVE`; the finalizer must not infer success or retry deletion for an ambiguous resource.

After pre-inventory and journal readiness, set harness umask to `0077`; exclusively reserve five SysV SHM keys with `shmget(..., IPC_CREAT|IPC_EXCL|0600)` at pinned `MaxSegmentSize` and five exact POSIX semaphore names with `sem_open(..., O_CREAT|O_EXCL, 0600, 1)`. Verify effective modes are exactly `0600`. Each `CREATED` event records exact key/name and exclusive-create evidence: for SHM, `shmid`, key, `IPC_STAT` size, creator PID/UID, owner UID and mode; for semaphore, exact source name, `sem_t*` address, `/dev/shm` resolved path, device/inode/UID/GID/mode/link count, and libc identity. A failed/mismatched reservation aborts before `Process/GW` construction; remove only resources whose successful journal receipt and current identity are exact while the lease remains valid. Otherwise do no deletion and report `INCONCLUSIVE`.

For each semaphore, record the `sem_open` provider via `dladdr`, resolved libc path, file device/inode, GNU build ID when available, SHA-256 of the opened libc file, `gnu_get_libc_version()`, and exact production source name/path. Immediately after successful exclusive `sem_open`, capture `/proc/self/maps` and prove the returned `sem_t*` address lies within a shared mapping whose device/inode equals the `stat` identity of exact `/dev/shm/sem.<decimal-name>`. Retain the exact matching maps line or a strict projection including address range, permissions, offset, device and inode in the reservation journal. Recompute/validate this linkage from retained evidence; version/path plus a boolean is insufficient. The mapping procedure is target-libc-specific and is not claimed tested here. If `sem_open` returns no handle, mapping is absent, path/device/inode disagree, or mapping cannot be captured, abort before `Process/GW`; do not set `semaphore_name_mapping_verified=true`, do not PASS, and do no speculative deletion.

After Gateway/output workers stop and join and `Process/GW` are destroyed, the harness must append/fsync exactly one `RUN_SHUTDOWN_COMPLETE` event to the still-open reservation journal. This event is the positive proof that normal shutdown completed; a caller boolean or process exit is not a substitute. The trusted supervisor then waits/reaps every recorded harness process PID and closes stdout/stderr before invoking the same allowlisted `--finalize` binary. It selects `normal_cleanup` only when the journal contains exactly one valid shutdown-complete event, every recorded process has a verified wait receipt, both logs are closed, no timeout/signal/ambiguous termination occurred, and the lease remains continuously valid. In this mode the finalizer captures post inventory, validates/replays the journal, reads required inventories, inspects exact current identities (`shmctl(..., IPC_STAT, ...)` for SysV SHM and `stat`/equivalent metadata for the exact POSIX semaphore object), and removes only journal-proven run-created resources. For each resource it appends/fsyncs `CLEANUP_INTENT`, rechecks lease and identity immediately before the destructive call, invokes only exact `shmctl(exact_shmid, IPC_RMID, ...)` or `sem_unlink(exact_recorded_name)`, then appends/fsyncs the matching outcome. The `normal_cleanup` gate above is satisfied only while the trusted launcher, supervisor receipt, execution identity, result-directory identity and continuous lease remain valid through final-record closure. If normal-shutdown proof is absent while these common authority/file-safety conditions remain valid, the finalizer may choose `abnormal_evidence_only`. Loss or ambiguity of the lease, supervisor proof, execution identity or result-directory authority is not an abnormal-mode trigger: do not invoke or re-enter the finalizer; preserve the pending/partial record and report `BLOCKED/INCONCLUSIVE`. A failure during normal cleanup after any `CLEANUP_INTENT` stops further destructive calls and retries; while authority remains valid, record `INCONCLUSIVE` with the evidence-derived cleanup outcome in the current finalization, otherwise retain pending recovery state. No ambiguous deletion is retried.

In `abnormal_evidence_only` mode, only while the trusted launcher, execution identity, result-directory identity and continuous lease remain verified, and after a valid `supervisor-termination.json` proves every recorded process was reaped and both logs were closed, the finalizer strictly validates the pending run record and supervisor receipt. It may then open only allowlisted evidence names using no-follow regular-file checks, preserving and hashing safely readable closed raw bytes within per-file caps; it does not require all eight evidence files, parse/replay their JSON or journal schemas, or enforce the normal complete-bundle check. `failure.missing_artifacts` lists required evidence names actually absent; the new `failure.unavailable_artifacts` lists required names present but unreadable/unsafe/over-cap. These sets are disjoint, exclude the run record and optional `harness-diagnostics.txt`, and neither may falsely classify a present file as missing. Malformed but safely readable evidence is preserved and hashed, not parsed or used to block an otherwise permitted `INCONCLUSIVE` record. A bounded cleanup-history classifier may recognize only well-formed journal intent/outcome records to distinguish `not_started`, `partial` or `complete`; it does not perform full schema replay, and any missing, malformed, truncated or conflicting history yields `unknown` without preventing evidence finalization. The final record sets `record_finalized=true`, `finalization_mode=abnormal_evidence_only`, and `status=INCONCLUSIVE`. `failure.cleanup_outcome` is `not_started` only if a complete validated journal proves neither `CLEANUP_INTENT` nor a successful `CREATED` resource; it is `partial` when valid durable evidence proves at least one cleanup action failed; it is `complete` only when a complete validated journal records exactly one `REMOVED` outcome for every successful `CREATED` resource and no cleanup failure, while the run remains `INCONCLUSIVE` because abnormal mode has no final-inventory proof; and it is `unknown` when cleanup history/outcomes are absent, malformed, truncated or ambiguous. The abnormal finalizer performs no new cleanup, does not retry, and preserves journal bytes. If the lease, receipt, process/log proof or result-directory authority fails at any point, do not write/re-enter finalization; keep the pending/partial record and report `BLOCKED/INCONCLUSIVE`. A present but partial, stale, or shutdown-unverified `ipc-post.json` or `ipc-final.json` may be preserved and hashed as a file, but must not be treated as a verified inventory: the run-record post/final inventory summaries remain unavailable, with `is_empty=null` and no claimed inventory digest. The finalizer never reads `/proc/sysvipc` or `/dev/shm`, calls `IPC_STAT`, `IPC_RMID` or `sem_unlink`, appends/repairs/closes the journal, or performs cleanup during this mode.

In `normal_cleanup` mode, capture a complete final inventory and require it to match pre exactly for all `/proc/sysvipc` raw bytes/parsed summaries and `/dev/shm` entries. The final run record lists `reservation-journal.jsonl` exactly once with final size/hash and every other required runtime artifact exactly once. The finalizer strictly parses journal/inventories, recomputes event order/transitions, raw table hashes and normalized inventories, proves each cleanup event maps to one successful create receipt, and derives status from those bytes rather than caller booleans. It must not accept `journal_fsynced=true`, `mapping_verified=true` or a final inventory match as substitutes for underlying evidence. PASS/FAIL require the complete bundle and safe normal cleanup; missing/contradictory evidence, unexplained change, residual candidate, failed join, timeout or ambiguous exit is `INCONCLUSIVE`; no broad host cleanup is permitted. If a failure occurs before journal closure, retain partial evidence and do not claim PASS/FAIL. If normal cleanup has written any `CLEANUP_INTENT` and then fails, stop subsequent destructive calls and retries; while lease and result-directory authority still hold, write an `INCONCLUSIVE` final record with `cleanup_outcome=partial` or `unknown` as supported by the journal. If authority or safe record writing is lost, leave the pending/partial evidence and report `BLOCKED/INCONCLUSIVE`; never re-enter finalization to claim `not_started`. The abnormal evidence-only path described above preserves the historical cleanup outcome and performs no further cleanup. D2 remains one controlled correctness run only; no network/VM, D3 timing, performance claim, algorithm change, PGCS/PG, commit or push unless separately authorised.

1. Host-controlled preflight before construction: the run starts only after the §7.1 inventory and attribution prerequisites pass. No constructor-time IPC may precede that successful preflight.

2. HT construction, Message lifetime, and failure-safe ownership: after the host-controlled preflight succeeds, construct the isolated non-`PGCS` `Process`. Its pinned constructor calls `NewBlock("HT", ...)` then `NewBlock("GW", ...)` (`Process.cpp:228–230`), and the `GW` constructor retrieves that HT (`GW.cpp:99–101`) before initialization dereferences it (`GWRunInitialization01.cpp:68–80`). Do not call `NewBlock("HT")` or `NewBlock("GW")` again; verify exactly two blocks exist and one lookup succeeds for each name. Before starting Gateway, create the Process-owned target, capture `GetDeleteFlag()==false` while single-threaded, and bind its ID with the observer. Capture the immutable scalar `Message::InstantiationNumber` (`Message.h:158`) before each production `Run`; use only that copied ID for Run-entry and Run-return events, publishing the return event before `DeleteMarkedMessages()` can reclaim the object. Never dereference a Message pointer after a possible reclamation; duplicate, zero, stale or wrapped identity is INCONCLUSIVE. The three-argument constructor used by `Process::NewMessage` initializes `Delete=false` and `ApplicationDeleted=DELETED_BY_CORE` (`Message.cpp:37–70`, especially 63–64); `DeleteMarkedMessages` deletes only true-flagged messages (`Process.cpp:885–917`). Keep the target out of the queue and do not mark it through Q0; use exactly three inert command lines `-ng039 --probe 0.2`, which match neither the registered action nor the `0.1` fallback in `Block::Run` (`Block.cpp:244–313`). For the expected Q0/Q1 pending path, after Gateway and its output worker stop/join, verify no target pop/Run and `Delete=false`; call `EraseMessage` only while Process is quiescent and require `GetNumberOfMessages()` to decrease by exactly one. Keep the detached object alive through `Process` destruction: its destructor calls `DeleteMessages()` before deleting `Blocks`, so after EraseMessage it will not own/delete this target; `GW::~GW` destroys actions, while the queue container drops raw pointer values. Delete the detached target exactly once after Process/GW destruction. If any target pop/Run, delete flag, count, or identity makes ownership uncertain, never dereference, erase, delete, or invoke the C++ Process/GW destructors on a possibly stale pointer: write a bounded/fsynced `run-record.pending.json` with `record_finalized=false` and no fabricated measurements, exit nonzero via `_Exit`, and let the isolated process/namespace terminate. The trusted supervisor must wait/reap every PID in the frozen harness process tree, close stdout/stderr, and write/fsync the required `supervisor-termination.json` containing exact per-PID raw `waitpid` statuses, log-closure state, and hashes of available closed logs. Only after every PID is reaped and both logs are closed may the parent-side finalizer in item 5 run `abnormal_evidence_only`: it writes a final `INCONCLUSIVE` record with unavailable post/final inventories and every missing artifact explicit, leaves the journal byte-for-byte as found, and performs no IPC inspection or cleanup. Absence of `RUN_SHUTDOWN_COMPLETE` forbids normal cleanup; process exit does not prove worker join or `Process/GW` destruction. A partial construction, blocked join, timeout, signal, or uncertain ownership follows this same no-cleanup path. If termination receipts or log closure are incomplete, do not invoke the finalizer or release the lease; keep the pending record and classify the gate `BLOCKED/INCONCLUSIVE`.

3. Fresh, correlated observation contract: keep the 64-bit event sequence and add a saturating `gateway_iteration_id`, assigned once at the start of each Gateway loop iteration and carried across its wait and due/pop decision. Preserve two distinct sticky scalar records: (a) while the existing `InputQueueMutex` lock is held when the real `wait_for` returns, if the registered target is the head, capture `WaitCapture{sequence, iteration_id, target_id, head_id, head_time, depth, timeout_ms, predicate_return}` once and never overwrite it; (b) while the production due/pop critical-section lock is held, capture the corresponding `DecisionCapture{sequence, iteration_id, candidate_id, candidate_time, actual_due_branch, actual_popped}` from the existing strict comparison/pop branch, without performing a second comparison, and never overwrite that capture. The records correlate only if the iteration IDs match, `decision.sequence > wait.sequence`, and the wait target/head ID matches the actual decision candidate. Set `correlation_status=MATCHED` only then. Otherwise use `PENDING`, `DECISION_NOT_REACHED`, `HEAD_MISMATCH`, `INVALID`, or `OVERFLOW`; no incomplete pair is exposed as a valid due result. Stop before the decision, a changed head, invalid/duplicate/wrapped ID, missed event, stale sequence, or counter overflow makes Q1 BLOCKED/INCONCLUSIVE, never zero/PASS. Under Q0/Q1, capture sequence/counter baselines at each window start: Q0 requires a fresh empty-observation delta inside its 100 ms no-controlled-offer window after bootstrap drain; Q1 requires target admission/head identity, a matched wait/decision pair newer than the Q1 baseline, zero target pops/Run entries/returns throughout 100 ms, and a future target time at end. Use `std::chrono::steady_clock` for correctness windows; `Message::Time` remains the scheduler clock. Set `gateway_started=true` under `InputQueueMutex` before the loop and never reset it; later registration fails. Before production `Run`, copy `Message::InstantiationNumber` to a scalar; emit Run-entry/return with that copy and publish the return before `DeleteMarkedMessages()` can reclaim the object. Never dereference a Message pointer after possible reclamation. These are correctness-window rules, not timing/performance evidence.

4. Bounded source/build verification contract: the five-path code allowlist and the CMake target census remain separate checks. The diagnostic graph contains exactly `Common` and `ng039_gw_q0_q1_d2`; the normal graph contains the pinned default targets, with the observer macro absent. The root CMake file returns before production application targets in the diagnostic branch. The compile-database checker receives the database, profile, pinned worktree root and fresh build root as four positional inputs after `python3 -c <checker>`; do not insert a `--` separator unless the checker explicitly consumes it. It verifies that each compile entry's working directory and actual `-o` output resolve under the build root, the source operand and entry `file` resolve to the same regular source inside the worktree, any `output` metadata agrees with `-o`, exactly one `-c` occurs, and compiler realpath/target census match the manifest. Require exactly one `-std=gnu++20`, only `-O0` optimization tokens, and the observer define exactly once on `Common` and the harness in diagnostic mode and nowhere in default mode. Parse joined and split `-D`/`-U` switches by base identifier, including function-like macro spellings; reject every function-like or undefined `NG_GW_QUEUE_TEST_OBSERVATION` form. Reject response files, `-Wp`, `-Xpreprocessor`, `-ansi`, `-traditional`, `-x`, forced-include and macro-file forms in both split (`-include file`, `-imacros file`) and joined (`-includefile`, `-imacrosfile`) spelling, plus toolchain-selection and plugin forms that can bypass those checks. Do not reject unrelated warning/debug switches through a purported universal grammar; preserve every complete argument vector and the compile database SHA-256 as evidence. This checker is scoped to the pinned CMake output and is not a generic validator for arbitrary compile databases.

Resolve the roots before configure. Use fresh, pairwise-disjoint source/build/results paths beneath `<WORKSPACE_ROOT>`; never place NovaGenesis source, build or result artifacts in `.hermes`. Invoke configure, build and both checkers through the same minimal `env -i` environment, retaining only explicit `PATH`, `HOME`, `TMPDIR`, `LANG` and `LC_ALL` values. Set `HOME` and `TMPDIR` to distinct directories under the named D2 scratch root. Record those retained values, command working directories, tool versions and exact commands in the manifest/run identity; inherited environment variables are not passed through. No broad environment-variable inventory or custom denylist is required.

For link validation, invoke `python3 -c <checker> <link.txt> <build-root> ng039_gw_q0_q1_d2` without an unconsumed `--`; the checker opens that exact named `link.txt` itself, requires a regular non-symlink file at the expected diagnostic target path under the build root, and never validates stdin or another caller-supplied string. Parse its contents with `shlex.split(posix=True)`, verify the pinned compiler realpath and exactly one standalone `-static`, `-static-libgcc`, and `-static-libstdc++`, and reject response files plus direct dynamic-link modes. For this target, reject `-Wl` and `-Xlinker` entirely rather than normalizing every forwarding grammar; this rejects `-Wl,-shared` and `-Xlinker -B -Xlinker dynamic` without a general linker-option parser. The checker uses explicit nonzero failures, not Python `assert`.

Freeze only the directly relevant synthetic cases: source/output path escape, mismatched compile metadata, repeated `-c`, wrong compiler, observer macro via `-Wp`/`-Xpreprocessor`, joined `-include`/`-imacros` override, alternate standard or optimization, wrong link-file path/content, missing/duplicate required static flag, `-Wl,-shared`, and the split `-Xlinker` dynamic override. For both literal checker entrypoints, test one complete accepted invocation and a missing-operand rejection, in normal Python and `PYTHONOPTIMIZE=1`; test the two joined forced-include negatives in the same modes. No real D2 configure/build/run is implied by these synthetic tests.

5. Bounded run-record and dual-mode finalizer contract: `tests/spec039/q0_q1_d2.manifest.json` defines strict Draft 2020-12 schemas for the pending/final run record v5, `supervisor-termination.json` receipt v1, IPC inventories, observer events and `reservation-journal.jsonl` events v2. The finalized run record's `finalization_mode` is exactly `normal_cleanup` or `abnormal_evidence_only`; pending records use null, and the finalizer derives the mode from journal and supervisor evidence rather than a caller label. The same allowlisted `--finalize` binary is the sole semantic authority. Both finalization modes require a strictly valid pending run record and supervisor receipt; every recorded PID reaped; closed stdout/stderr; unchanged execution/result-directory identities; and a dedicated-UID lease continuously held through final-record closure. If any common prerequisite is absent, ambiguous or lost, do not invoke/re-enter the finalizer or write a final record; retain pending/partial evidence and report `BLOCKED/INCONCLUSIVE`. In `normal_cleanup`, strictly parse/replay all required JSON/JSONL, verify raw `/proc/sysvipc` hashes/rows, exact artifact set, wait/log receipt, lease and semaphore mapping, then inspect exact identities and perform only journal-authorised cleanup after `CLEANUP_INTENT` fsync and immediate identity/lease recheck. `RUN_SHUTDOWN_COMPLETE` must be durable after joins/destruction and before any cleanup intent. PASS/FAIL require the full normal evidence and inventory gates; if a normal cleanup attempt fails after an intent, stop further destructive calls/retries and write `INCONCLUSIVE` with `cleanup_outcome=partial` or `unknown` as supported by durable evidence. If the final record cannot safely be written, leave pending recovery evidence.

In `abnormal_evidence_only`, normal shutdown proof may be absent, but all common prerequisites still hold. Strictly validate the pending record and supervisor receipt; for other allowlisted evidence files, use no-follow regular-file checks, preserve/hash safely readable closed raw bytes within bounds, but do not require the full bundle or parse/replay inventory, observer or journal schemas. Malformed but safely readable evidence must not block a final `INCONCLUSIVE` record. `failure.missing_artifacts` is exactly the required evidence names actually absent; `failure.unavailable_artifacts` records required names present but unsafe, unreadable or over-cap. The lists are disjoint; neither includes the record itself or optional `harness-diagnostics.txt`. Bound raw-byte reads by the per-file and 32 MiB total caps; classify over-budget files unavailable instead of aborting final-record creation. Present post/final files may be preserved but their inventory summaries remain unavailable, never empty. Derive `failure.cleanup_outcome`: `not_started` only when a complete validated journal proves neither `CLEANUP_INTENT` nor a successful `CREATED` resource; `partial` when valid durable evidence proves a cleanup attempt failed; `complete` only when a complete validated journal records exactly one `REMOVED` outcome for every successful `CREATED` resource and no cleanup failure, while the abnormal record remains `INCONCLUSIVE` without final-inventory proof; and `unknown` when cleanup history or outcomes are incomplete, malformed, truncated or ambiguous. This mode always yields `INCONCLUSIVE`, performs no IPC inspection, cleanup or journal mutation, and never retries. A cleanup intent or prior removal is reported truthfully, not rewritten as `not_started`.

The bounded semantic regression set must reject claimed PASS/FAIL for missing/unverified launcher or lease, a second/foreign process, diagnostic `sem_open` failure calling `sem_unlink`, pre-existing candidate keys/names, unreadable/over-cap/raw-parsed-mismatched normal inventories, noncanonical base64, missing/reordered/duplicated/unfsynced normal journal events, candidate identity/mode mismatch, missing `sem_t` mapping, residual candidates, unexpected host changes, or final-inventory mismatch. Reject normal cleanup without a durable `RUN_SHUTDOWN_COMPLETE`, complete supervisor wait receipts, closed logs and continuous lease. Reject invocation of either mode before all recorded PIDs are reaped and logs closed; when common launcher/receipt/lease/result-directory authority is absent, retain pending evidence and report `BLOCKED/INCONCLUSIVE`. Abnormal fixtures must accept malformed but safely readable non-control evidence as raw preserved/hashable bytes without requiring a complete bundle or parsing it; reject claims of PASS/FAIL, empty inventories, joins/destruction, optional diagnostics in `missing_artifacts`, overlap between `missing_artifacts` and `unavailable_artifacts`, or an inaccurate cleanup-history outcome. `not_started` requires a complete journal proving no cleanup intent and no successful `CREATED` resource; a proven failed attempt is `partial`; `complete` requires exactly one `REMOVED` outcome for every successful `CREATED` resource and no cleanup failure; missing/truncated/malformed, intent-without-outcome, or created-without-cleanup history is `unknown`. Abnormal mode must never retry cleanup or mutate journal/IPC. The finalizer must perform no speculative deletion for disputed resources. Normal PASS/FAIL requires a complete, replayable journal and every required evidence artifact; the normal bundle has eight evidence artifacts plus `run-record.json`, optional `harness-diagnostics.txt`, and a ten-file maximum. Normal artifacts are each ≤4 MiB, the journal ≤256 KiB/128 events, the final run record ≤256 KiB, and the complete normal bundle ≤32 MiB. Abnormal raw preservation remains no-follow and bounded: per-file/total read caps apply, but excess/unsafe present files are recorded as unavailable rather than making strict schema/completeness validation a prerequisite to the `INCONCLUSIVE` record.

The host-controlled runtime shape remains one controlled D2 process tree under the dedicated UID, one Q0/Q1 correctness run, no network input and no performance timing. The launcher serializes the run and finalizer under the verified exclusive lease; the harness itself spawns no child process. The host IPC namespace may contain unrelated objects, but all candidate resources must be absent before reservation and all exact cleanup remains bound to the durable journal. No broad host cleanup is permitted. If the launcher/lease or result-directory identity is absent/lost, a second process enters the identity domain, or supervisor evidence is incomplete, do not perform name-based unlink/RMID or invoke/re-enter finalization; preserve pending/partial evidence and report `BLOCKED/INCONCLUSIVE`. An abnormal record may be written only while common lease and file-safety authority remains valid.

**Next gate:** resolve the r11 B1 lease-loss versus abnormal-mode eligibility and B2 historical cleanup-outcome contradiction in this same v1.14 SPEC; scope strict completeness/schema/replay checks to `normal_cleanup`, while abnormal evidence-only mode safely preserves malformed/partial non-control evidence and records missing/unavailable files plus `not_started`/`partial`/`complete`/`unknown` cleanup history without IPC or retries. Update run-record v5, manifest and packet; validate both abnormal and normal failure branches and bind fresh exact hashes. The r11 one-call consent is consumed; obtain fresh owner consent bound to the final r12 packet SHA before Astra. D2 implementation/configure/build/run remains blocked until a positive documentary review, provisioned launcher/preconditions and separate owner execution gate. D3 and Q2–Q4 remain deferred. This work is documentary, not correctness or performance evidence.

## 8. Revision history

| Version | Date | Change |
|---|---|---|
| v1.1 | 2026-10-04 | Owner-approved documentary D0 contract for Q0/Q1 only. |
| v1.2 | 2026-10-04 | Recorded the separately bounded D2 seam/correctness scope, D1-H compile/link result, and pre-code Astra checkpoint; the exact-SHA review returned HOLD. No D2 code/runtime followed. |
| v1.3 | 2026-10-05 | First local draft of the same-SPEC documentary closure requirements; superseded by v1.4 after a source-grounded HT construction prerequisite was identified. |
| v1.4 | 2026-10-05 | Same-SPEC closure draft with HT-before-GW prerequisite. The exact pre-code packet SHA `ba4ec6ec4b6660e98c3e7d0cdbbd6f18b7945ab6f6bdd30c2294e6c654150948` was reviewed by GPT-6 Astra and returned AMEND. No D2 code/runtime followed. |
| v1.5 | 2026-10-05 | Same-SPEC documentary closure response to the v1.4 AMEND. Sanitized r03 packet SHA `7121fa9307beb790ff093319d6d9bc54fb9d296bf68d8c705647fbcebf740797` was reviewed by GPT-6 Astra and returned AMEND. No D2 code/runtime followed. |
| v1.6 | 2026-10-05 | Same-SPEC response to the r03 AMEND. Sanitized r04 packet SHA `754ad970c1bb5e915262e825856345416360154274ece78ae80642ca6a30d829` returned AMEND: findings 3 and 4b–4d closed at design level; finding 4a remained must-fix. No D2 code/runtime followed. |
| v1.7 | 2026-10-05 | Same-SPEC documentary response to r04; sanitized r05 packet returned AMEND with three 4a must-fixes. No D2 code/runtime followed. |
| v1.8 | 2026-10-05 | Same-SPEC response to r05: supported compiler-argument grammar, forwarded/function-like macro rejection, `-ansi` rejection, output-derived target identity, assertion-independent exact link checks, dynamic-link conflict rejection, and schema-level PASS/failure conditions. Local/unreviewed; fresh exact-SHA consent is required for r06; D2/D3 remain blocked. |
| v1.9 | 2026-10-10 | Editorial privacy-only redaction of the workstation-specific D1-H checkout path to a stable role alias; no design, scope, or gate change. |
| v1.10 | 2026-10-10 | Owner-directed, same-SPEC response to r06 AMEND. Keeps the source/output containment, exact `-c` count, minimal `env -i`, actual `link.txt` binding and no-false-PASS contract; relaxes generic argument grammar, exhaustive environment denylisting and duplicated schema/finalizer rules. The frozen B1/B2/B3 r07 packet later received documentary `APPROVE` only. |
| v1.11 | 2026-10-11 | Owner-directed replacement of the unavailable user/IPC/mount/PID namespace sandbox by host-controlled candidate-resource attribution, 0600 exclusive reservations, recorded libc semaphore-name mapping, pre/post/final inventory equality and fail-closed identity-checked cleanup. No AppArmor exception or global security change. |
| v1.12 | 2026-10-12 | Same-SPEC documentary response to r08 B2/B3: require a dedicated single-run UID/launcher or remain blocked; suppress diagnostic `sem_unlink` after all three `sem_open` failure paths; add fsynced reservation journal, retained raw `/proc/sysvipc` bytes/recomputation and evidence binding the returned `sem_t` mapping to the exact inode. Astra r09 substantially accepted B2/B3 but returned AMEND on contradictory finalizer IPC scope; no implementation/runtime authorization. |
| v1.13 | 2026-10-12 | Same-SPEC response to the sole r09 B1: reconcile the finalizer's responsibility by allowing only bounded inventory/current-identity inspection and journal-authorized exact `IPC_RMID`/`sem_unlink` under the continuous dedicated-UID lease, while forbidding Process/GW construction, resource creation, production message traffic, ambiguous retries and broad cleanup. Astra r10 resolved the IPC boundary and preserved B2/B3, but returned AMEND on abnormal finalization. No implementation/runtime authorization. |
| v1.14 | 2026-10-12 | Same-SPEC response to r10 abnormal-finalization B1: adds durable `RUN_SHUTDOWN_COMPLETE`, supervisor PID wait/log receipts, run-record v5 and journal-event v2. The r11 documentary review returned AMEND on lease-loss eligibility and truthful cleanup history after a cleanup attempt. The r12 amendment clarifies common lease/file-authority gates, separates normal full-schema validation from abnormal bounded raw preservation, adds missing/unavailable artifact sets and evidence-derived cleanup outcomes, and forbids finalizer re-entry or cleanup retries. No implementation/runtime authority. |


===== FILE: Specs/RESULTS-SPEC-039/precode-manifest-v1.14-r12.json — FULL current bytes, SHA-256 63ccf50e6ecd19940f0c0b5e98d438ae6e34f5b2dbf0304faef97b5eca7c0917 =====
{
  "amendment_response": {
    "optional_r05_hardening": {
      "actual_output_based_target_mapping": "incorporated",
      "conflicting_dynamic_link_controls": "incorporated",
      "schema_level_status_consistency": "incorporated"
    },
    "owner_directed_relaxation": {
      "environment": "use env -i with five explicit retained values; no exhaustive inherited-variable denylist",
      "generic_compile_argument_grammar": "relaxed: validate only provenance and acceptance-critical compiler controls; retain complete argv vectors and database digest for review",
      "linker_forwarding": "reject -Wl/-Xlinker outright; do not implement a forwarding normalizer",
      "schema_vs_finalizer": "schema validates shape/local implications; one finalizer owns cross-artifact semantics",
      "scope": "same SPEC-039, same five-file D2 code allowlist and same Q0/Q1 correctness gate"
    },
    "r05_must_fix_4a": [
      "Reject compiler-option encodings that bypass macro inspection, including -Wp, and -Xpreprocessor; recognize function-like macro names by base identifier.",
      "Reject -ansi and all unsupported/alternate dialect, optimization, response-file, forced-include, toolchain, and driver-option forms; use an explicit supported-argument grammar.",
      "Replace Python assert checks in link validation with explicit nonzero failures that remain active under python3 -O and PYTHONOPTIMIZE=1."
    ],
    "r05_verdict": "AMEND",
    "r06_must_fixes": [
      "compile source/output/build-root containment and exactly one -c",
      "freeze a deterministic environment for configure/build/checkers",
      "read and bind the exact link.txt; reject forwarded linker controls",
      "prevent schema-valid PASS/FAIL contradictions using local schema constraints plus a bounded semantic finalizer"
    ],
    "r06_verdict": "AMEND",
    "r07_documentary_approval": "The frozen v1.10/r07 B1/B2/B3 packet returned APPROVE for documentary closure only; no D2 implementation/runtime authority. Its one-call consent is consumed.",
    "r08_followup": {
      "b1": "Withdrawn: the first submission envelope omitted `128` from the SPEC SHA; actual r08 packet/SPEC/manifest agreed. Sender error, not packet defect.",
      "b2": "Confirmed: ordinary same-UID host execution does not enforce continuous object identity; diagnostic build must suppress all three GW.cpp sem_unlink-on-sem_open-failure paths, and D2 remains blocked without an enforceable dedicated single-run identity/lease.",
      "b3": "Confirmed: require a durable replayable journal, retained raw /proc/sysvipc bytes for digest/summary recomputation, and evidence binding returned sem_t* mapping to exact libc/dev-shm identity.",
      "source_review": "Three independent read-only reviews corroborated B2/B3. No agent altered files or performed runtime/IPC operations."
    },
    "r08_host_profile_amendment": "Owner-directed same-SPEC amendment: replace namespace setup with full host inventory, absence checks for candidate keys/names, exclusive reservation of five exact SHM and POSIX semaphore resources, durable identity journal, exact cleanup, and final inventory equality. No broad host IPC cleanup is permitted.",
    "review_status": "The frozen v1.10/r07 B1/B2/B3 packet returned documentary APPROVE after earlier r07 AMEND snapshots. This v1.11/r08 host-controlled profile is a new unsubmitted working draft. Fresh owner consent bound to its exact final packet SHA is required before Astra.",
    "v1_12_b2": {
      "diagnostic_guard": "Under NG_GW_QUEUE_TEST_OBSERVATION only, all three GW.cpp sem_open failure branches at baseline lines 970, 1263, 1322 record failure and do not call sem_unlink; macro-off behavior unchanged. Design requirement only; implementation needs separate owner authorization.",
      "execution_status": "BLOCKED until an owner-controlled dedicated non-login UID and single-run launcher/lease are provisioned and verified",
      "root_threat_model": "Host root/administrator is trusted; if concurrent root mutation is in threat scope, reject the shared-host profile.",
      "same_spec": "SPEC-039"
    },
    "v1_12_b3": {
      "journal": "Required append-only fsynced reservation-journal.jsonl; strict event schema/order/replay and final bundle hash binding.",
      "raw_inventory": "Each IPC inventory artifact retains exact /proc/sysvipc table bytes as canonical base64; finalizer recomputes SHA-256 and parsed summary from decoded bytes.",
      "semaphore_mapping": "Record exact libc identity and prove returned sem_t* address lies in a /proc/self/maps shared mapping with device/inode equal to the exact semaphore file; mismatch is INCONCLUSIVE. Procedure is specified, not executed."
    },
    "v1_8_documentary_response": [
      "Compile-command target identity comes from the actual -o output, with metadata/path disagreement rejected.",
      "Joined and split -D/-U spellings are parsed; malformed, forwarded, duplicate, conflicting, undefined, and function-like observer-macro forms fail closed.",
      "Require exactly one -std=gnu++20 and only -O0; reject -ansi, -traditional, -x, unknown driver flags, and response files.",
      "Link validation uses shlex tokenization, exact standalone static flags, pinned compiler identity, response-file rejection, and explicit dynamic-link conflict checks.",
      "Run-record schema allOf expresses PASS/FAIL/INCONCLUSIVE cross-field conditions; fixtures cover each conditional."
    ]
  },
  "changed_file_allowlist": [
    "CMakeLists.txt",
    "Common/src/GW.h",
    "Common/src/GW.cpp",
    "tests/spec039/gw_q0_q1_d2.cpp",
    "tests/spec039/q0_q1_d2.manifest.json"
  ],
  "classification_rules": {
    "artifact_names_unique": "Normal_cleanup: exactly eight required runtime evidence artifacts, optional harness-diagnostics.txt at most once, run-record.json separately required. Abnormal_evidence_only: no complete-bundle requirement; missing_artifacts and unavailable_artifacts are exact disjoint sets over required evidence names; optional diagnostics is not required or marked missing.",
    "assertion_values": [
      "PASS",
      "FAIL",
      "INCONCLUSIVE",
      "NOT_RUN"
    ],
    "dev_shm_bounds": {
      "max_entries": 4096,
      "max_file_bytes": 9223372036854775807,
      "meaning": "host inventory representation cap; not a private tmpfs limit",
      "sum_bytes_max": 9223372036854775807
    },
    "fail_requires": [
      "record_finalized=true and failure.code=ASSERTION_FAILED",
      "at least one correctness assertion FAIL; every inventory/reservation/cleanup assertion PASS; no INCONCLUSIVE/NOT_RUN assertion",
      "known harness exit; normal outer code 0..123; signal=null; timeout_fired=false",
      "all run-created IPC removed by exact identity and final inventory equals pre; failure.missing_artifacts=[]; no uncertainty trigger",
      "failure summary exactly matches recomputed evidence",
      "dedicated UID/launcher/lease verified",
      "journal complete and replayable",
      "raw table digests/summaries recomputable",
      "returned semaphore mapping verified",
      "all required run artifacts including reservation-journal.jsonl present",
      "complete closed journal and eight runtime artifacts",
      "raw inventories/hashes independently recomputable",
      "diagnostic failure paths cannot unlink by name",
      "validated supervisor-termination.json receipt binds the exact process-tree PID set to raw waitpid statuses; stdout/stderr closed and available log hashes match",
      "normal_cleanup has durable RUN_SHUTDOWN_COMPLETE after worker joins and Process/GW destruction; no timeout/signal/ambiguous wait",
      "abnormal_evidence_only is INCONCLUSIVE, requires confirmed process reaping and closed logs, and performs zero IPC inspection/destructive calls",
      "schema-valid supervisor-termination.json whose raw wait receipts cover the exact process_tree_pids and whose closed-log hashes match",
      "durable RUN_SHUTDOWN_COMPLETE before normal_cleanup and before every cleanup intent"
    ],
    "failure_safety_assertions": [
      "host_inventory_preflight",
      "pre_inventory_complete",
      "candidate_keys_absent_pre",
      "candidate_resources_reserved",
      "process_blocks_verified",
      "gateway_joined",
      "output_worker_joined",
      "target_erased_once",
      "post_inventory_complete",
      "candidate_resources_attributable",
      "candidate_resources_removed",
      "final_inventory_complete",
      "candidate_keys_absent_final",
      "final_inventory_matches_pre",
      "finalization_complete",
      "evidence_complete",
      "wait_decision_correlated",
      "scalar_run_identity_valid"
    ],
    "failure_summary_consistency": "Finalizer computes all failure fields from validated assertions, events, artifacts and inventories; caller-supplied contradictions map to INCONCLUSIVE/EVIDENCE_INVALID.",
    "inconclusive_precedence": [
      "timeout/signal or ambiguous outer code",
      "missing/invalid stage/final/artifact or duplicate artifact name",
      "finalizer/schema/size/bundle failure",
      "identity/sequence/correlation ambiguity or counter overflow",
      "partial construction or blocked join",
      "incomplete host inventory, failed exclusive reservation, incomplete journal, incorrect mode, libc name-mapping mismatch, or candidate identity mismatch",
      "failed exact cleanup, residual candidate, or final inventory mismatch",
      "contradictory observation/assertion or unavailable required field",
      "any NOT_RUN/INCONCLUSIVE acceptance assertion",
      "missing/unverified dedicated UID launcher or lease before any IPC creation",
      "lease loss or second process entering the dedicated UID domain",
      "sem_open failure path attempts sem_unlink in diagnostic build",
      "raw /proc/sysvipc bytes absent, non-canonical, over-cap, digest-mismatched or parser-mismatched",
      "reservation journal missing/partial/reordered/duplicated/unfsynced or crash between cleanup intent and outcome",
      "returned sem_t* mapping missing/mismatched with exact libc/dev-shm identity",
      "missing/unverified dedicated UID launcher/lease before IPC creation",
      "lease loss or second process enters execution identity",
      "diagnostic sem_open failure path calls sem_unlink",
      "raw /proc/sysvipc bytes missing/noncanonical/over-cap/hash- or parse-mismatched",
      "journal event absent/partial/reordered/duplicate/unfsynced",
      "crash after cleanup intent without durable outcome",
      "returned sem_t mapping absent or mismatched",
      "missing/invalid supervisor-termination.json, PID-set mismatch, duplicate/missing wait receipt, undecodable wait status, open stdout/stderr log",
      "abnormal_evidence_only attempts IPC inspection, cleanup, journal repair/close, claims PASS/FAIL or claims unavailable inventory is empty",
      "normal_cleanup without exactly one durable RUN_SHUTDOWN_COMPLETE before any cleanup intent",
      "missing/invalid supervisor receipt, PID-set mismatch, duplicate/missing wait receipt, undecodable raw wait status, or open logs",
      "normal_cleanup without exactly one durable RUN_SHUTDOWN_COMPLETE before cleanup",
      "abnormal mode attempts IPC/cleanup/journal mutation, claims PASS/FAIL, or reports unavailable inventories as empty",
      "both modes require continuous lease and stable result-directory identity through final record; lease loss yields pending BLOCKED/INCONCLUSIVE",
      "abnormal evidence-only may preserve malformed but safely readable non-control files without schema/replay; normal strict completeness rules do not gate this path",
      "cleanup_outcome is evidence-derived: not_started only with a complete no-intent/no-CREATED proof; partial for a proven failed attempt; complete only with one REMOVED outcome for every successful CREATED resource and no cleanup failure; otherwise unknown, with no retry"
    ],
    "local_fixture_coverage": {
      "invalid_count": 14,
      "invalid_examples": [
        "PASS assertion NOT_RUN",
        "candidate key/name collision before reservation",
        "partial exclusive reservation",
        "incomplete reservation journal",
        "mismatched candidate resource identity",
        "residual candidate after cleanup",
        "unexpected non-candidate change",
        "final inventory differs from pre",
        "incomplete final inventory",
        "unmatched Q1",
        "nonzero PASS exit",
        "missing required artifact",
        "FAIL without false assertion",
        "pending with failure claim",
        "missing dedicated UID/launcher lease",
        "second process enters execution UID",
        "diagnostic sem_open failure calls sem_unlink",
        "raw sysvipc bytes/digest/parsed summary mismatch",
        "partial or reordered reservation journal",
        "crash after cleanup intent before durable outcome",
        "returned sem_t pointer maps to wrong dev-shm inode",
        "missing dedicated UID launcher/lease",
        "second process enters exclusive run identity",
        "sem_open failure path issues sem_unlink",
        "raw sysvipc digest/parsed summary mismatch",
        "missing/partial/reordered/unfsynced journal event",
        "cleanup intent without outcome after crash",
        "returned sem_t pointer maps to wrong inode",
        "missing supervisor wait receipt for a recorded process PID",
        "supervisor receipt PID set differs from execution_identity.process_tree_pids",
        "stdout/stderr not closed before finalizer",
        "normal cleanup without durable RUN_SHUTDOWN_COMPLETE",
        "abnormal evidence-only mode attempts IPC_RMID/sem_unlink or journal repair",
        "abnormal final record claims PASS/FAIL or empty post/final inventory",
        "actually absent required evidence artifact omitted from failure.missing_artifacts",
        "optional harness-diagnostics.txt listed in failure.missing_artifacts",
        "missing supervisor-termination.json",
        "waitpid receipt PID mismatch/duplicate/missing",
        "closed-log hash mismatch",
        "normal cleanup without RUN_SHUTDOWN_COMPLETE",
        "abnormal mode performs cleanup or claims PASS/FAIL",
        "lease loss blocks finalizer entry/re-entry and leaves pending evidence",
        "malformed but safely readable non-control artifact is raw-preserved in abnormal mode",
        "missing/unavailable evidence sets overlap or misclassify a present file",
        "cleanup intent followed by failure reported as not_started",
        "cleanup retry after ambiguous durable intent/outcome",
        "empty, unknown, malformed or reordered journal classified as not_started, partial or complete"
      ],
      "status": "historical v1.8 local result; detailed standalone fixtures were not retained in the current worktree",
      "valid_count": 4
    },
    "observed_exit": {
      "ambiguous_code": "125-127 wrapper/invocation failure and 128-255 without independent signal evidence are INCONCLUSIVE; never infer a unique harness signal from a shell code",
      "outer_code": "raw timeout-wrapper exit code; always preserve it",
      "signal": "only set from an independent unambiguous wait result; otherwise null",
      "timeout_fired": "true on an explicit supervisor timeout; code 124 is treated conservatively as timeout"
    },
    "pass_requires": [
      "record_finalized=true; failure=null; outer_code=0; harness_code=0; signal=null; timeout_fired=false",
      "every required assertion PASS; actual Q0/Q1 values are non-null and satisfy the frozen local conditions",
      "complete host pre/post/final inventories; candidate keys/names absent in pre; all five SHM and semaphore candidates exclusively reserved, journaled, mode 0600, and mapped under the recorded libc",
      "post candidate identities match exclusive reservations; exact run-created resources removed after stop/join; final complete inventory equals pre byte/semantic identity; no counter overflow; Q1 correlation MATCHED",
      "verified dedicated UID, launcher identity and exclusive lease held from pre-inventory through finalizer; no concurrent process in the execution identity domain",
      "all three diagnostic GW.cpp sem_open-failure paths are built with the no-sem_unlink guard and their injected failure tests pass",
      "complete strictly ordered fsynced reservation journal, closed and hash-bound as reservation-journal.jsonl",
      "raw /proc/sysvipc bytes retained in canonical base64; recomputed hashes and parsed normalized summaries match each inventory record",
      "each returned sem_t* is proven to map to the exact recorded libc-backed /dev/shm device/inode for its decimal source name",
      "verified dedicated non-login UID/launcher identity and exclusive lease continuously held through finalizer",
      "all three diagnostic GW.cpp sem_open-failure branches are guarded; targeted failure tests show no sem_unlink",
      "complete ordered fsynced reservation journal is closed and hash-bound as required artifact",
      "raw /proc/sysvipc bytes retained; hashes and normalized summaries recomputed from exact bytes",
      "each returned sem_t* is proven to map to exact recorded libc-backed /dev/shm inode",
      "validated supervisor-termination.json receipt binds the exact process-tree PID set to raw waitpid statuses; stdout/stderr closed and available log hashes match",
      "normal_cleanup has durable RUN_SHUTDOWN_COMPLETE after worker joins and Process/GW destruction; no timeout/signal/ambiguous wait",
      "abnormal_evidence_only is INCONCLUSIVE, requires confirmed process reaping and closed logs, and performs zero IPC inspection/destructive calls",
      "schema-valid supervisor-termination.json whose raw wait receipts cover the exact process_tree_pids and whose closed-log hashes match",
      "durable RUN_SHUTDOWN_COMPLETE before normal_cleanup and before every cleanup intent"
    ],
    "post_inventory": "Complete inventory after teardown; only the five exact resources created under successful exclusive reservations may differ from pre, and each must match its durable identity receipt before cleanup.",
    "required_artifact_names": [
      "stdout.txt",
      "stderr.txt",
      "ipc-pre.json",
      "ipc-post.json",
      "ipc-final.json",
      "observer-events.json",
      "reservation-journal.jsonl",
      "supervisor-termination.json"
    ],
    "run_id_uniqueness": "unique per execution; compare across records.",
    "semantic_finalizer": "The finalizer recomputes evidence completeness, artifact hashes, inventory summaries, event sequence/counter consistency, Q0/Q1 correlations and overall status; schema-valid assertion labels alone are never acceptance.",
    "staged_and_final_inconclusive": {
      "finalized": "status=INCONCLUSIVE and failure is a bounded object",
      "not_finalized": "status=INCONCLUSIVE and failure=null; no failure claim before finalization"
    },
    "staging": "record_finalized=false => status INCONCLUSIVE; staging is never in final bundle.",
    "status_values": [
      "PASS",
      "FAIL",
      "INCONCLUSIVE"
    ],
    "sysv_bounds": {
      "all_host_inventory_max_segments": 4096,
      "max_candidate_segment_bytes": 33554532,
      "max_candidate_segments": 5,
      "max_candidate_total_bytes": 167772660,
      "reported_total_equals_segment_sum": true,
      "unique_ids": true
    }
  },
  "cmake": {
    "build_type": "Debug",
    "changed_file_allowlist_is_not_target_census": true,
    "cmake_version": "3.28.3",
    "compile_commands_verification": {
      "argument_scope": "Purpose-specific pinned-CMake checker, not a universal compiler command parser. Preserve the complete argv vector and compile database SHA-256; do not reject unrelated warning/debug flags solely because they are outside a generic grammar.",
      "checker_logic": "\\\nimport json, os, re, shlex, sys\nfrom pathlib import Path\n\nEXPECTED = {\n    \"diagnostic\": {\"Common\", \"ng039_gw_q0_q1_d2\"},\n    \"default\": {\"Common\", \"ContentApp\", \"IoTTestApp\", \"NBTestApp\", \"PGCS\", \"NRNCS\"},\n}\nCOMPILER = os.path.realpath(\"/usr/bin/x86_64-linux-gnu-g++-13\")\nOBSERVER = \"NG_GW_QUEUE_TEST_OBSERVATION\"\nREQUIRED = {\"-O0\", \"-g\", \"-g3\", \"-Wall\", \"-fmessage-length=0\", \"-pthread\", \"-Wno-deprecated\", \"-lpthread\", \"-lrt\", \"-fno-omit-frame-pointer\"}\nSOURCE_SUFFIXES = (\".c\", \".cc\", \".cpp\", \".cxx\", \".c++\", \".C\")\n\ndef reject(message):\n    raise ValueError(message)\n\ndef resolved(value, base, strict):\n    if not isinstance(value, str) or not value.strip():\n        reject(\"empty path metadata\")\n    p = Path(value)\n    if not p.is_absolute():\n        p = base / p\n    return p.resolve(strict=strict)\n\ndef under(path, root):\n    try:\n        path.relative_to(root)\n        return True\n    except ValueError:\n        return False\n\ndef tokens_for(entry):\n    if not isinstance(entry, dict): reject(\"entry must be object\")\n    if not isinstance(entry.get(\"directory\"), str) or not entry[\"directory\"]: reject(\"directory is empty\")\n    if not isinstance(entry.get(\"file\"), str) or not entry[\"file\"]: reject(\"file is empty\")\n    args = entry.get(\"arguments\")\n    command = entry.get(\"command\")\n    if args is not None and (not isinstance(args, list) or not args or any(not isinstance(x, str) for x in args)):\n        reject(\"arguments must be a nonempty string array\")\n    if command is not None:\n        if not isinstance(command, str) or not command: reject(\"command must be nonempty string\")\n        if any(c in command for c in \"$`;&|<>*?\"): reject(\"shell/glob syntax is unsupported\")\n        try: parsed = shlex.split(command, posix=True)\n        except ValueError: reject(\"malformed command quoting\")\n        if not parsed: reject(\"empty command\")\n        if args is not None and parsed != args: reject(\"command/arguments mismatch\")\n        if args is None: args = parsed\n    if args is None: reject(\"entry has neither command nor arguments\")\n    return args\n\ndef parse_macro_switches(argv):\n    found=[]; i=0\n    while i < len(argv):\n        token=argv[i]\n        if token in (\"-D\", \"-U\"):\n            if i+1 >= len(argv): reject(\"missing -D/-U operand\")\n            op=token; value=argv[i+1]; i+=2\n        elif token.startswith(\"-D\") and len(token)>2:\n            op=\"-D\"; value=token[2:]; i+=1\n        elif token.startswith(\"-U\") and len(token)>2:\n            op=\"-U\"; value=token[2:]; i+=1\n        else:\n            i+=1; continue\n        match=re.fullmatch(r\"([A-Za-z_][A-Za-z0-9_]*)(\\([^)]*\\))?(?:=(.*))?\", value)\n        if not match: reject(\"malformed macro switch\")\n        if match.group(1)==OBSERVER:\n            found.append((op, match.group(2), match.group(3), value))\n    return found\n\ndef output_operand(argv):\n    found=[]; i=0\n    while i < len(argv):\n        token=argv[i]\n        if token == \"-o\":\n            if i+1 >= len(argv) or not argv[i+1] or argv[i+1].startswith(\"-\"): reject(\"missing -o operand\")\n            found.append(argv[i+1]); i+=2; continue\n        if token.startswith(\"-o\") and len(token)>2 and not token.startswith(\"-open\"):\n            found.append(token[2:])\n        i+=1\n    if len(found)!=1: reject(\"exactly one -o output is required\")\n    return found[0]\n\ndef target_from_output(path):\n    parts=path.parts; names=[]\n    for i,part in enumerate(parts[:-1]):\n        if part==\"CMakeFiles\" and parts[i+1].endswith(\".dir\"):\n            names.append(parts[i+1][:-4])\n    if len(names)!=1: reject(\"target must derive uniquely from actual -o path\")\n    return names[0]\n\ndef validate_compile_database(data, profile, worktree_root, build_root):\n    if profile not in EXPECTED: reject(\"unknown profile\")\n    if not isinstance(data, list) or not data: reject(\"compile database must be nonempty array\")\n    source_root=Path(worktree_root).resolve(strict=True)\n    build=Path(build_root).resolve(strict=True)\n    if not source_root.is_dir() or not build.is_dir(): reject(\"trusted roots must be existing directories\")\n    if under(source_root, build) or under(build, source_root): reject(\"source/build roots must be disjoint\")\n    seen=set(); pairs=set(); counts={}\n    for entry in data:\n        argv=tokens_for(entry)\n        if os.path.realpath(argv[0]) != COMPILER: reject(\"compiler realpath mismatch\")\n        if any(x.startswith(\"@\") for x in argv): reject(\"response files are unsupported\")\n        if any(x == \"-Wp\" or x.startswith(\"-Wp,\") or x == \"-Xpreprocessor\" for x in argv): reject(\"forwarded preprocessor controls are unsupported\")\n        if any(x in {\"-ansi\", \"-traditional\", \"-traditional-cpp\", \"-x\", \"-include\", \"-imacros\", \"-specs\", \"-B\", \"-wrapper\", \"-fplugin\"} or x.startswith((\"-include\", \"-imacros\", \"-specs=\", \"-B\", \"-fplugin=\")) for x in argv):\n            reject(\"compiler mode/toolchain override is unsupported\")\n        directory=resolved(entry[\"directory\"], Path.cwd(), strict=True)\n        if not directory.is_dir() or not under(directory, build): reject(\"compile directory escapes build root\")\n        source=resolved(entry[\"file\"], directory, strict=True)\n        if not source.is_file() or not under(source, source_root): reject(\"source escapes pinned worktree\")\n        source_args=[x for x in argv[1:] if x.endswith(SOURCE_SUFFIXES)]\n        if len(source_args)!=1 or resolved(source_args[0],directory,strict=True)!=source: reject(\"exactly one source operand must match entry file\")\n        if argv.count(\"-c\")!=1: reject(\"exactly one -c is required\")\n        out=resolved(output_operand(argv),directory,strict=False)\n        if not under(out,build): reject(\"actual -o output escapes build root\")\n        metadata=entry.get(\"output\")\n        if metadata is not None:\n            meta=resolved(metadata,directory,strict=False)\n            if meta!=out: reject(\"output metadata disagrees with actual -o\")\n        target=target_from_output(out)\n        if target not in EXPECTED[profile]: reject(\"unexpected target derived from actual output\")\n        standards=[x for x in argv if x.startswith(\"-std=\")]\n        if standards != [\"-std=gnu++20\"]: reject(\"effective language mode must be exactly gnu++20\")\n        optimizations=[x for x in argv if x.startswith(\"-O\")]\n        if not optimizations or any(x!=\"-O0\" for x in optimizations): reject(\"effective optimization must be -O0\")\n        if not REQUIRED.issubset(set(argv)): reject(\"required pinned project/debug flags missing\")\n        macros=parse_macro_switches(argv)\n        if profile==\"default\" and macros: reject(\"observer macro defined or undefined in default profile\")\n        if profile==\"diagnostic\" and target in {\"Common\",\"ng039_gw_q0_q1_d2\"}:\n            if macros != [(\"-D\", None, \"1\", OBSERVER+\"=1\")]: reject(\"diagnostic observer must be exactly one object-like =1 define\")\n        pair=(target,str(out))\n        if pair in pairs: reject(\"duplicate target/output compile entry\")\n        pairs.add(pair); seen.add(target); counts[target]=counts.get(target,0)+1\n    if seen != EXPECTED[profile]: reject(\"target census mismatch\")\n    return {\"profile\":profile,\"targets\":sorted(seen),\"target_counts\":counts,\"entry_count\":len(data)}\n\nif __name__ == \"__main__\":\n    try:\n        with open(sys.argv[1],encoding=\"utf-8\") as f: db=json.load(f)\n        print(json.dumps(validate_compile_database(db,sys.argv[2],sys.argv[3],sys.argv[4]),sort_keys=True))\n    except Exception as exc:\n        print(\"compile check failed: \"+str(exc),file=sys.stderr); raise SystemExit(2)",
      "compile_action_cardinality": "exactly one -c, exactly one source operand matching entry.file, and exactly one actual -o output per entry",
      "compile_database_sha256_and_target_counts_recorded": true,
      "compiler_realpath_required": "/usr/bin/x86_64-linux-gnu-g++-13",
      "critical_flags": {
        "observer_macro": "parse joined/split -D/-U by base name including function-like spelling; default profile has none; diagnostic Common/harness each have exactly one object-like =1 define",
        "optimization": "one or more -O0 tokens; no other -O token",
        "standard": "exactly one -std=gnu++20"
      },
      "expected_targets": {
        "basis": "CMakeLists.txt at immutable baseline; existing standalone targets are behind NG_ENABLE_LEGACY_STANDALONE=OFF. Diagnostic graph is a future exact two-target root-listfile early-return graph, not present in the baseline.",
        "default_off": [
          "Common",
          "ContentApp",
          "IoTTestApp",
          "NBTestApp",
          "PGCS",
          "NRNCS"
        ],
        "diagnostic_forbidden": [
          "ContentApp",
          "GIRS",
          "HTS",
          "IoTTestApp",
          "NBTestApp",
          "PGCS",
          "PSS",
          "NRNCS"
        ],
        "diagnostic_on": [
          "Common",
          "ng039_gw_q0_q1_d2"
        ],
        "exact": true,
        "legacy_absent_default_off": [
          "GIRS",
          "HTS",
          "PSS"
        ]
      },
      "metadata_binding": "If entry.output exists, normalized output must equal actual -o. command and arguments, when both present, must tokenize identically.",
      "negative_fixtures": [
        "source outside worktree",
        "output outside build root",
        "output metadata mismatch",
        "duplicate -c",
        "wrong compiler realpath",
        "-Wp observer forwarding",
        "-Xpreprocessor observer forwarding",
        "default-profile observer undefine",
        "-ansi alternate dialect",
        "later non-C++20 standard",
        "non--O0 optimization",
        "target census mismatch",
        "diagnostic function-like observer define",
        "joined -include observer override",
        "joined -imacros observer override"
      ],
      "not_executed_in_r07": false,
      "planned_command": "env -i PATH=/usr/bin:/bin HOME=\"$D2_SCRATCH/home\" TMPDIR=\"$D2_SCRATCH/tmp\" LANG=C LC_ALL=C /usr/bin/python3 -c <compile-checker> \"$DB\" \"$PROFILE\" \"$D2_WORKTREE\" \"$BUILD_ROOT\"",
      "r07_python_optimize_probe": "PASS after B1/B2 amendment: literal commands ran under normal Python and PYTHONOPTIMIZE=1.",
      "r07_synthetic_execution": "PASS after B1/B2 amendment: 2 positive profiles and 15 distinct negative mutations rejected under normal Python and PYTHONOPTIMIZE=1 (30 negative executions); exact entrypoint missing-operand rejection covered separately.",
      "reject_bypasses": [
        "@response files",
        "-Wp and -Wp,*",
        "-Xpreprocessor",
        "-ansi",
        "-traditional",
        "-traditional-cpp",
        "-x",
        "-include",
        "-imacros",
        "-specs",
        "-B",
        "-wrapper",
        "-fplugin"
      ],
      "required": true,
      "required_project_switches": [
        "-O0",
        "-Wall",
        "-Wno-deprecated",
        "-fmessage-length=0",
        "-fno-omit-frame-pointer",
        "-g",
        "-g3",
        "-lpthread",
        "-lrt",
        "-pthread"
      ],
      "root_rules": [
        "Both roots must resolve to existing directories.",
        "Roots must be disjoint.",
        "Each compile directory and actual -o output must resolve under build_root.",
        "Each source path must resolve to a regular file under worktree_root.",
        "Resolve existing symlink components before containment checks."
      ],
      "target_identity_source": "Derive only from the actual -o path CMakeFiles/<target>.dir segment; never from arbitrary command text or metadata.",
      "trusted_root_inputs": [
        "worktree_root",
        "build_root"
      ]
    },
    "compiler": "/usr/bin/x86_64-linux-gnu-g++-13",
    "compiler_version": "GNU C++ 13.3.0",
    "configure_common_flags": "-G \"Unix Makefiles\" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_COMPILER=/usr/bin/x86_64-linux-gnu-g++-13 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_EXTENSIONS=ON -DCMAKE_CXX_FLAGS=-O0 '-DCMAKE_CXX_FLAGS_DEBUG=-O0 -g'",
    "configure_generator": "Unix Makefiles",
    "configured_target_census": {
      "basis": "CMakeLists.txt at immutable baseline; existing standalone targets are behind NG_ENABLE_LEGACY_STANDALONE=OFF. Diagnostic graph is a future exact two-target root-listfile early-return graph, not present in the baseline.",
      "default_off": [
        "Common",
        "ContentApp",
        "IoTTestApp",
        "NBTestApp",
        "PGCS",
        "NRNCS"
      ],
      "diagnostic_forbidden": [
        "ContentApp",
        "GIRS",
        "HTS",
        "IoTTestApp",
        "NBTestApp",
        "PGCS",
        "PSS",
        "NRNCS"
      ],
      "diagnostic_on": [
        "Common",
        "ng039_gw_q0_q1_d2"
      ],
      "exact": true,
      "legacy_absent_default_off": [
        "GIRS",
        "HTS",
        "PSS"
      ]
    },
    "cxx_extensions": "ON",
    "cxx_standard": "20 (gnu++20)",
    "default_build_command": "env -i PATH=/usr/bin:/bin HOME=\"$D2_SCRATCH/home\" TMPDIR=\"$D2_SCRATCH/tmp\" LANG=C LC_ALL=C /usr/bin/cmake --build \"$D2_SCRATCH/build-default\" --target Common --parallel 2",
    "default_configure_command": "env -i PATH=/usr/bin:/bin HOME=\"$D2_SCRATCH/home\" TMPDIR=\"$D2_SCRATCH/tmp\" LANG=C LC_ALL=C /usr/bin/cmake -S \"$D2_WORKTREE\" -B \"$D2_SCRATCH/build-default\" -G \"Unix Makefiles\" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_COMPILER=/usr/bin/x86_64-linux-gnu-g++-13 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_EXTENSIONS=ON -DCMAKE_CXX_FLAGS=-O0 '-DCMAKE_CXX_FLAGS_DEBUG=-O0 -g' -DNG_ENABLE_NG039_GW_Q0_Q1_D2=OFF -DNG_ENABLE_LEGACY_STANDALONE=OFF",
    "default_value": "OFF",
    "diagnostic_build_command": "env -i PATH=/usr/bin:/bin HOME=\"$D2_SCRATCH/home\" TMPDIR=\"$D2_SCRATCH/tmp\" LANG=C LC_ALL=C /usr/bin/cmake --build \"$D2_SCRATCH/build-diagnostic\" --target ng039_gw_q0_q1_d2 --parallel 2",
    "diagnostic_configure_command": "env -i PATH=/usr/bin:/bin HOME=\"$D2_SCRATCH/home\" TMPDIR=\"$D2_SCRATCH/tmp\" LANG=C LC_ALL=C /usr/bin/cmake -S \"$D2_WORKTREE\" -B \"$D2_SCRATCH/build-diagnostic\" -G \"Unix Makefiles\" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_COMPILER=/usr/bin/x86_64-linux-gnu-g++-13 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_EXTENSIONS=ON -DCMAKE_CXX_FLAGS=-O0 '-DCMAKE_CXX_FLAGS_DEBUG=-O0 -g' -DNG_ENABLE_NG039_GW_Q0_Q1_D2=ON -DNG_ENABLE_LEGACY_STANDALONE=OFF",
    "diagnostic_option": "NG_ENABLE_NG039_GW_Q0_Q1_D2",
    "diagnostic_value": "ON",
    "environment_contract": {
      "applies_to": [
        "cmake configure",
        "cmake build",
        "compile checker",
        "link checker",
        "finalizer"
      ],
      "directory_setup": "Create HOME and TMPDIR beneath D2_SCRATCH before configure using the authorized parent process; reject symlinks and overlap.",
      "launcher": "env -i; no inherited variable is passed to child processes",
      "not_executed_in_r07": true,
      "recording": "Record the exact expanded five name/value pairs and working directory in the build/run identity before configure; HOME and TMPDIR must be distinct children of D2_SCRATCH.",
      "retained": {
        "HOME": "${D2_SCRATCH}/home",
        "LANG": "C",
        "LC_ALL": "C",
        "PATH": "/usr/bin:/bin",
        "TMPDIR": "${D2_SCRATCH}/tmp"
      },
      "unset_inherited": "All variables not explicitly retained above are absent by construction; no per-variable denylist."
    },
    "export_compile_commands": true,
    "legacy_option": "NG_ENABLE_LEGACY_STANDALONE",
    "legacy_value_in_both_fresh_configs": "OFF",
    "link_command_verification": {
      "binding_control": "A valid named link.txt passes when stdin is deliberately supplied adversarial text; checker reads the file, not stdin.",
      "checker_logic": "\\\nimport hashlib, os, shlex, sys\nfrom pathlib import Path\nCOMPILER=os.path.realpath(\"/usr/bin/x86_64-linux-gnu-g++-13\")\nREQUIRED=(\"-static\",\"-static-libgcc\",\"-static-libstdc++\")\nDIRECT_DYNAMIC={\"-shared\",\"-dynamic\",\"-Bdynamic\",\"-pie\",\"-static-pie\",\"-rdynamic\",\"--dynamic-linker\"}\ndef reject(message): raise ValueError(message)\ndef validate_link_file(link_file,build_root,target):\n    root=Path(build_root).resolve(strict=True)\n    expected=(root/\"CMakeFiles\"/(target+\".dir\")/\"link.txt\").resolve(strict=True)\n    supplied=Path(link_file)\n    if supplied.is_symlink() or not supplied.is_file(): reject(\"link.txt must be a regular non-symlink file\")\n    actual=supplied.resolve(strict=True)\n    if actual!=expected: reject(\"link.txt path is not the expected target file\")\n    try: actual.relative_to(root)\n    except ValueError: reject(\"link.txt escapes build root\")\n    content=actual.read_text(encoding=\"utf-8\")\n    try: argv=shlex.split(content,posix=True)\n    except ValueError: reject(\"malformed link command quoting\")\n    if not argv or os.path.realpath(argv[0])!=COMPILER: reject(\"linker compiler identity mismatch\")\n    if any(x.startswith(\"@\") for x in argv): reject(\"response files unsupported\")\n    if any(x==\"-Wl\" or x.startswith(\"-Wl,\") or x==\"-Xlinker\" or x.startswith(\"-Xlinker\") for x in argv): reject(\"forwarded linker controls unsupported for this target\")\n    if any(x in DIRECT_DYNAMIC or x.startswith(\"--dynamic-linker=\") for x in argv): reject(\"direct dynamic-link override\")\n    if any(argv.count(flag)!=1 for flag in REQUIRED): reject(\"each static-link token must occur exactly once\")\n    return {\"status\":\"PASS\",\"link_file_sha256\":hashlib.sha256(actual.read_bytes()).hexdigest(),\"compiler\":argv[0]}\nif __name__==\"__main__\":\n    try: print(validate_link_file(sys.argv[1],sys.argv[2],sys.argv[3]))\n    except Exception as exc: print(\"link check failed: \"+str(exc),file=sys.stderr); raise SystemExit(2)",
      "compiler_realpath_required": "/usr/bin/x86_64-linux-gnu-g++-13",
      "failure_behavior": "Explicit ValueError/nonzero exit; never Python assert.",
      "file_binding": "Open the named link.txt directly; require a regular non-symlink file whose canonical path equals build_root/CMakeFiles/ng039_gw_q0_q1_d2.dir/link.txt.",
      "forwarding_policy": "Reject every -Wl/-Xlinker spelling for this target instead of normalizing arbitrary forwarding syntax.",
      "modes_to_test": [
        "normal",
        "PYTHONOPTIMIZE=1"
      ],
      "negative_fixtures": [
        "wrong link.txt path",
        "missing standalone -static",
        "duplicate standalone -static",
        "wrong linker compiler realpath",
        "@response file",
        "-Wl,-shared",
        "split -Xlinker -B -Xlinker dynamic",
        "symlinked link.txt"
      ],
      "not_executed_in_r07": false,
      "planned_command": "env -i PATH=/usr/bin:/bin HOME=\"$D2_SCRATCH/home\" TMPDIR=\"$D2_SCRATCH/tmp\" LANG=C LC_ALL=C /usr/bin/python3 -c <link-checker> \"$LINK_TXT\" \"$BUILD_ROOT\" ng039_gw_q0_q1_d2",
      "r07_python_optimize_probe": "PASS after B1/B2 amendment: literal link command ran under normal Python and PYTHONOPTIMIZE=1; positive stdin-binding control and all 8 targeted negatives exercised.",
      "r07_synthetic_execution": "PASS after B1/B2 amendment: 1 stdin-binding positive and 8 distinct negatives rejected in normal Python and PYTHONOPTIMIZE=1 (16 negative executions); exact entrypoint missing-target rejection covered separately.",
      "reject_dynamic_controls": [
        "-shared",
        "-dynamic",
        "-Bdynamic",
        "-pie",
        "-static-pie",
        "-rdynamic",
        "--dynamic-linker",
        "any -Wl form",
        "any -Xlinker form",
        "@response file"
      ],
      "required": true,
      "required_exact_arguments": [
        "-static",
        "-static-libgcc",
        "-static-libstdc++"
      ],
      "tokenizer": "shlex.split(posix=True) on the bytes read from that actual file; stdin is never used."
    },
    "macro": "NG_GW_QUEUE_TEST_OBSERVATION",
    "macro_scope": "Common PUBLIC + harness PRIVATE only in isolated option-ON graph",
    "project_appended_cxx_flags": [
      "-O0",
      "-g3",
      "-Wall",
      "-fmessage-length=0",
      "-pthread",
      "-Wno-deprecated",
      "-lpthread",
      "-lrt",
      "-fno-omit-frame-pointer"
    ],
    "project_executable_link_flags": [
      "-static",
      "-static-libgcc",
      "-static-libstdc++"
    ],
    "root_return_before_production_targets": true,
    "target_graph_guard_required": true
  },
  "evidence_recovery": {
    "d1h_binary_at_recorded_path_available_now": false,
    "d1h_build_log_at_recorded_path_available_now": false,
    "d1h_evidence_status": "Original D1-H binary/log remain unavailable and their historical hashes cannot be rechecked. A fresh compile/link-only rebuild was performed 2026-10-10 outside .hermes; its different binary/log hashes are preserved in Specs/RESULTS-SPEC-039/d1h-rebuild-20261010.json. Runtime remains unexecuted.",
    "d1h_fresh_rebuild": {
      "binary_sha256": "060266a24546bc9a8e77ec7b0af409cb480bae770e56f65e95f2727877e5cc7d",
      "build_exit_code": 0,
      "build_log_sha256": "658cdeec0de9d743438c2683f0bca710b3a7b53302374303793a7838d3246e98",
      "configure_exit_code": 0,
      "configure_log_sha256": "65fb7c5ef0f1613c7aa181794678203c1d70d0c1313f8b20a51b471ef8330feb",
      "interpretation": "New evidence; do not conflate with historical recorded D1-H hashes.",
      "receipt_path": "Specs/RESULTS-SPEC-039/d1h-rebuild-20261010.json",
      "receipt_sha256": "3a71e39a80241332429c9298d687b1bbedc8078f1d6f00b37df7c6e5602e9f28",
      "runtime_executed": false,
      "worktree_head": "1af604dad66148618cc0e578c28adb4a9829e607"
    },
    "d1h_manifest_sha256": "9a7c4cda741e81192642053c4bdc60c3a5c0a46afcfa6c658ff2e2324f24225a",
    "d1h_manifest_sha256_rechecked": true,
    "d1h_reported_binary_sha256": "ddf06f6d47e37564b9e52866c3d272ce3cc6cd8b433700c864c4499d676c31ab",
    "d1h_reported_build_exit_code": 0,
    "d1h_reported_build_log_sha256": "095009a9e4fb7097aef005300f7562ab8ce941c691538a88a4064f944f4d19aa",
    "historical_v1_8_synthetic_results": {
      "compile_checker": {
        "negative_cases": 26,
        "positive_profiles": 2
      },
      "link_checker": {
        "cases": 15,
        "modes": [
          "normal",
          "python3 -O",
          "PYTHONOPTIMIZE=1"
        ],
        "negative": 12,
        "positive": 3
      },
      "not_real_build_or_runtime_evidence": true,
      "run_record_fixtures": {
        "invalid": 14,
        "valid": 4
      },
      "schemas_meta_valid": 3,
      "source": "NG-039/NG-040 task notes and prior-session record, not a retained standalone validator run bundle"
    },
    "r05_archive_manifest_sha256_rechecked": "27eabdd3b469b7044f9ed4935058ffe509f3a92712371924cc73b76dd619266a",
    "r05_raw_sse_sha256_rechecked": "3d375e4cc4551793dff18a299e3e82ded99bf631a5fdfb0aed195a4dae24dc31",
    "r05_response_sha256_rechecked": "9b0fa2aa410d6055e50da31134e42af29d97ece12aac77d971299ece7e3851dc",
    "r07_synthetic_validation_receipt": {
      "binding_policy": "Generate/refresh the receipt only after the packet and manifest bytes are frozen; receipt records their exact input hashes. Record the receipt SHA-256 out-of-band in NG-039/NG-040. Do not embed a receipt hash in this manifest, avoiding a hash cycle.",
      "path": "Specs/RESULTS-SPEC-039/r07-synthetic-validation-20261010.json",
      "semantic_scope": "Synthetic checker/schema checks only; no executable cross-artifact finalizer exists yet."
    }
  },
  "evidence_schemas": {
    "collection_responsibility": {
      "artifact_name_uniqueness": "Normal cleanup requires each of eight evidence artifacts exactly once, optional diagnostics at most once, and run-record separately. Abnormal evidence-only does not require a complete bundle; missing_artifacts is exact absent required names, unavailable_artifacts is present but unsafe/unreadable/over-cap required names, and the sets are disjoint.",
      "failure_summary_binding": "The finalizer recomputes code, phase, last_event_sequence, inventory_availability, cleanup_outcome and missing_artifacts. ASSERTION_FAILED is valid only for an observed correctness-predicate failure with complete safe evidence; contradictory summary fields finalize INCONCLUSIVE/EVIDENCE_INVALID.",
      "finalizer_bounds": "10 s timeout + 5 s kill grace; RLIMIT_AS 128 MiB, CPU 10 s, FSIZE 4 MiB, NOFILE 32, CORE 0.",
      "finalizer_failure": "No valid finalized record means INCONCLUSIVE and cannot PASS.",
      "finalizer_semantic_authority": "The same allowlisted finalizer uses common lease/supervisor/result-directory gates. Normal mode performs strict completeness/schema/replay checks and exact cleanup. Abnormal mode strictly validates pending record and supervisor receipt only, raw-preserves bounded evidence, records actual cleanup history, and never performs IPC, cleanup, journal mutation or retry.",
      "harness_run": "Only after dedicated UID launcher/lease verified: create pending record/journal, create/reserve candidates and collect evidence. After all worker joins and Process/GW destruction, append/fsync one RUN_SHUTDOWN_COMPLETE before any cleanup intent. The supervisor later emits supervisor-termination.json after process wait/log closure.",
      "missing_or_invalid_staging": "Metadata-only INCONCLUSIVE; assertions NOT_RUN, measurements null, inventories unavailable, exact cleanup status unknown, missing artifact names; no invented values.",
      "outer_finalizer": "The finalizer requires valid lease, result-directory identity, supervisor receipt, complete process reaping and closed logs for either mode. If common authority fails, leave pending and report BLOCKED/INCONCLUSIVE. Normal mode may clean only after durable shutdown proof; abnormal mode records INCONCLUSIVE without live IPC or new cleanup.",
      "reservation_journal_artifact": "reservation-journal.jsonl is required, final, max 256 KiB/128 events, append-only strict JSONL; each record is fsynced and its artifact hash is bound in run-record.",
      "runtime_manifest_file": "none; q0_q1_d2.manifest.json is plan/schema input, not part of runtime bundle.",
      "shell_redirection": "The outer supervisor captures stdout/stderr under D2_RESULTS and closes descriptors before finalization.",
      "supervisor_termination_receipt": "Trusted launcher writes/fsyncs supervisor-termination.json after waiting/reaping every recorded process PID and closing stdout/stderr. The finalizer binds the receipt PID set to execution_identity.process_tree_pids, decodes raw waitpid statuses, checks closed-log hashes and validates launcher/lease identity."
    },
    "ipc_inventory": {
      "$id": "urn:ng039:ipc-inventory:v4",
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "additionalProperties": false,
      "properties": {
        "complete": {
          "type": "boolean"
        },
        "dev_shm_entries": {
          "items": {
            "additionalProperties": false,
            "properties": {
              "device": {
                "minimum": 0,
                "type": "integer"
              },
              "gid": {
                "minimum": 0,
                "type": "integer"
              },
              "inode": {
                "minimum": 0,
                "type": "integer"
              },
              "kind": {
                "enum": [
                  "regular",
                  "symlink",
                  "other"
                ]
              },
              "mode": {
                "minimum": 0,
                "type": "integer"
              },
              "name": {
                "maxLength": 255,
                "minLength": 1,
                "pattern": "^[^/]+$",
                "type": "string"
              },
              "nlink": {
                "minimum": 0,
                "type": "integer"
              },
              "size_bytes": {
                "maximum": 9223372036854775807,
                "minimum": 0,
                "type": "integer"
              },
              "uid": {
                "minimum": 0,
                "type": "integer"
              }
            },
            "required": [
              "name",
              "kind",
              "size_bytes",
              "device",
              "inode",
              "uid",
              "gid",
              "mode",
              "nlink"
            ],
            "type": "object"
          },
          "maxItems": 4096,
          "type": "array"
        },
        "error": {
          "maxLength": 512,
          "type": [
            "string",
            "null"
          ]
        },
        "is_empty": {
          "type": [
            "boolean",
            "null"
          ]
        },
        "phase": {
          "enum": [
            "pre",
            "post",
            "final"
          ]
        },
        "proc_sysvipc_raw_b64": {
          "additionalProperties": false,
          "properties": {
            "msg": {
              "contentEncoding": "base64",
              "maxLength": 1398104,
              "pattern": "^(?:[A-Za-z0-9+/]{4})*(?:[A-Za-z0-9+/]{2}==|[A-Za-z0-9+/]{3}=)?$",
              "type": "string"
            },
            "sem": {
              "contentEncoding": "base64",
              "maxLength": 1398104,
              "pattern": "^(?:[A-Za-z0-9+/]{4})*(?:[A-Za-z0-9+/]{2}==|[A-Za-z0-9+/]{3}=)?$",
              "type": "string"
            },
            "shm": {
              "contentEncoding": "base64",
              "maxLength": 1398104,
              "pattern": "^(?:[A-Za-z0-9+/]{4})*(?:[A-Za-z0-9+/]{2}==|[A-Za-z0-9+/]{3}=)?$",
              "type": "string"
            }
          },
          "required": [
            "shm",
            "sem",
            "msg"
          ],
          "type": "object"
        },
        "proc_sysvipc_sha256": {
          "additionalProperties": false,
          "properties": {
            "msg": {
              "pattern": "^[0-9a-f]{64}$",
              "type": [
                "string",
                "null"
              ]
            },
            "sem": {
              "pattern": "^[0-9a-f]{64}$",
              "type": [
                "string",
                "null"
              ]
            },
            "shm": {
              "pattern": "^[0-9a-f]{64}$",
              "type": [
                "string",
                "null"
              ]
            }
          },
          "required": [
            "shm",
            "sem",
            "msg"
          ],
          "type": "object"
        },
        "run_id": {
          "maxLength": 64,
          "minLength": 16,
          "type": "string"
        },
        "schema_version": {
          "const": "ng039-ipc-inventory/v4"
        },
        "sysv_ids": {
          "additionalProperties": false,
          "properties": {
            "msg": {
              "items": {
                "minimum": 0,
                "type": "integer"
              },
              "maxItems": 4096,
              "type": "array",
              "uniqueItems": true
            },
            "sem": {
              "items": {
                "minimum": 0,
                "type": "integer"
              },
              "maxItems": 4096,
              "type": "array",
              "uniqueItems": true
            },
            "shm": {
              "items": {
                "minimum": 0,
                "type": "integer"
              },
              "maxItems": 4096,
              "type": "array",
              "uniqueItems": true
            }
          },
          "required": [
            "shm",
            "sem",
            "msg"
          ],
          "type": "object"
        },
        "sysv_shm_segments": {
          "items": {
            "additionalProperties": false,
            "properties": {
              "creator_pid": {
                "minimum": 0,
                "type": "integer"
              },
              "creator_uid": {
                "minimum": 0,
                "type": "integer"
              },
              "id": {
                "minimum": 0,
                "type": "integer"
              },
              "key": {
                "type": [
                  "integer",
                  "null"
                ]
              },
              "mode": {
                "minimum": 0,
                "type": "integer"
              },
              "owner_uid": {
                "minimum": 0,
                "type": "integer"
              },
              "size_bytes": {
                "maximum": 9223372036854775807,
                "minimum": 0,
                "type": "integer"
              }
            },
            "required": [
              "id",
              "size_bytes",
              "key",
              "creator_pid",
              "creator_uid",
              "owner_uid",
              "mode"
            ],
            "type": "object"
          },
          "maxItems": 4096,
          "type": "array",
          "uniqueItems": true
        },
        "sysv_shm_total_bytes": {
          "maximum": 9223372036854775807,
          "minimum": 0,
          "type": [
            "integer",
            "null"
          ]
        }
      },
      "required": [
        "schema_version",
        "run_id",
        "phase",
        "complete",
        "is_empty",
        "sysv_ids",
        "dev_shm_entries",
        "error",
        "sysv_shm_segments",
        "sysv_shm_total_bytes",
        "proc_sysvipc_sha256",
        "proc_sysvipc_raw_b64"
      ],
      "type": "object"
    },
    "observer_events": {
      "$id": "urn:ng039:observer-events:v2",
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "additionalProperties": false,
      "properties": {
        "events": {
          "items": {
            "additionalProperties": false,
            "properties": {
              "candidate_was_popped": {
                "type": [
                  "boolean",
                  "null"
                ]
              },
              "correlation_status": {
                "enum": [
                  "PENDING",
                  "MATCHED",
                  "DECISION_NOT_REACHED",
                  "HEAD_MISMATCH",
                  "INVALID",
                  "OVERFLOW"
                ]
              },
              "counter_overflow": {
                "type": "boolean"
              },
              "decision_candidate_id": {
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "decision_candidate_time": {
                "type": [
                  "number",
                  "null"
                ]
              },
              "decision_due_branch": {
                "type": [
                  "boolean",
                  "null"
                ]
              },
              "decision_iteration_id": {
                "maximum": 18446744073709551615,
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "decision_sequence": {
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "decision_was_popped": {
                "type": [
                  "boolean",
                  "null"
                ]
              },
              "empty_queue_observation_count": {
                "minimum": 0,
                "type": "integer"
              },
              "event": {
                "enum": [
                  "None",
                  "QueueSnapshot",
                  "Admission",
                  "WaitReturn",
                  "PopDecision",
                  "RunEntry",
                  "RunReturn",
                  "Stop"
                ]
              },
              "event_message_id": {
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "first_target_wait_depth": {
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "first_target_wait_head_id": {
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "first_target_wait_head_time": {
                "type": [
                  "number",
                  "null"
                ]
              },
              "first_target_wait_iteration_id": {
                "maximum": 18446744073709551615,
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "first_target_wait_message_id": {
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "first_target_wait_returned_by_predicate": {
                "type": [
                  "boolean",
                  "null"
                ]
              },
              "first_target_wait_sequence": {
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "first_target_wait_timeout_ms": {
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "first_target_wait_valid": {
                "type": "boolean"
              },
              "gateway_iteration_id": {
                "maximum": 18446744073709551615,
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "gateway_started": {
                "type": "boolean"
              },
              "has_head": {
                "type": "boolean"
              },
              "head_message_id": {
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "head_time": {
                "type": [
                  "number",
                  "null"
                ]
              },
              "queue_depth": {
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "registered_target_id": {
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "scheduled_time_is_due": {
                "type": [
                  "boolean",
                  "null"
                ]
              },
              "sequence": {
                "minimum": 0,
                "type": "integer"
              },
              "stop_requested": {
                "type": "boolean"
              },
              "target_admission_count": {
                "minimum": 0,
                "type": "integer"
              },
              "target_non_due_count": {
                "minimum": 0,
                "type": "integer"
              },
              "target_pop_count": {
                "minimum": 0,
                "type": "integer"
              },
              "target_run_entry_count": {
                "minimum": 0,
                "type": "integer"
              },
              "target_run_return_count": {
                "minimum": 0,
                "type": "integer"
              },
              "valid": {
                "type": "boolean"
              },
              "wait_returned_by_predicate": {
                "type": [
                  "boolean",
                  "null"
                ]
              },
              "wait_timeout_ms": {
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              }
            },
            "required": [
              "sequence",
              "event",
              "valid",
              "gateway_started",
              "registered_target_id",
              "event_message_id",
              "queue_depth",
              "has_head",
              "head_message_id",
              "head_time",
              "wait_timeout_ms",
              "wait_returned_by_predicate",
              "scheduled_time_is_due",
              "candidate_was_popped",
              "empty_queue_observation_count",
              "target_admission_count",
              "target_non_due_count",
              "target_pop_count",
              "target_run_entry_count",
              "target_run_return_count",
              "first_target_wait_valid",
              "first_target_wait_sequence",
              "first_target_wait_message_id",
              "first_target_wait_head_id",
              "first_target_wait_head_time",
              "first_target_wait_depth",
              "first_target_wait_timeout_ms",
              "first_target_wait_returned_by_predicate",
              "stop_requested",
              "counter_overflow",
              "gateway_iteration_id",
              "first_target_wait_iteration_id",
              "decision_sequence",
              "decision_iteration_id",
              "decision_candidate_id",
              "decision_candidate_time",
              "decision_due_branch",
              "decision_was_popped",
              "correlation_status"
            ],
            "type": "object"
          },
          "maxItems": 4096,
          "type": "array"
        },
        "run_id": {
          "maxLength": 64,
          "minLength": 16,
          "type": "string"
        },
        "schema_version": {
          "const": "ng039-observer-events/v2"
        }
      },
      "required": [
        "schema_version",
        "run_id",
        "events"
      ],
      "type": "object"
    },
    "reservation_journal_event": {
      "$id": "urn:ng039:reservation-journal-event:v2",
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "additionalProperties": false,
      "properties": {
        "candidate_key": {
          "minimum": 1,
          "type": [
            "integer",
            "null"
          ]
        },
        "error_number": {
          "minimum": 0,
          "type": [
            "integer",
            "null"
          ]
        },
        "event": {
          "enum": [
            "JOURNAL_OPENED",
            "CREATE_INTENT",
            "CREATED",
            "CREATE_FAILED",
            "RUN_SHUTDOWN_COMPLETE",
            "CLEANUP_INTENT",
            "REMOVED",
            "CLEANUP_FAILED",
            "JOURNAL_CLOSED"
          ]
        },
        "identity": {
          "additionalProperties": false,
          "properties": {
            "candidate_key": {
              "minimum": 1,
              "type": [
                "integer",
                "null"
              ]
            },
            "creator_pid": {
              "minimum": 2,
              "type": [
                "integer",
                "null"
              ]
            },
            "creator_uid": {
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            },
            "device": {
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            },
            "gid": {
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            },
            "inode": {
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            },
            "libc_path": {
              "maxLength": 1024,
              "type": [
                "string",
                "null"
              ]
            },
            "libc_sha256": {
              "pattern": "^[0-9a-f]{64}$",
              "type": [
                "string",
                "null"
              ]
            },
            "libc_version": {
              "maxLength": 64,
              "type": [
                "string",
                "null"
              ]
            },
            "map_device_major": {
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            },
            "map_device_minor": {
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            },
            "map_end": {
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            },
            "map_inode": {
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            },
            "map_start": {
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            },
            "mapping_line": {
              "maxLength": 4096,
              "type": [
                "string",
                "null"
              ]
            },
            "mode": {
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            },
            "nlink": {
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            },
            "owner_uid": {
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            },
            "path": {
              "maxLength": 512,
              "type": [
                "string",
                "null"
              ]
            },
            "sem_t_address": {
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            },
            "shmid": {
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            },
            "size_bytes": {
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            },
            "source_name": {
              "maxLength": 255,
              "type": [
                "string",
                "null"
              ]
            },
            "uid": {
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            }
          },
          "type": [
            "object",
            "null"
          ]
        },
        "monotonic_ns": {
          "minimum": 0,
          "type": "integer"
        },
        "pid": {
          "minimum": 2,
          "type": "integer"
        },
        "resource_type": {
          "enum": [
            "sysv_shm",
            "posix_semaphore",
            "journal",
            null
          ],
          "type": [
            "string",
            "null"
          ]
        },
        "run_id": {
          "maxLength": 64,
          "minLength": 16,
          "type": "string"
        },
        "schema_version": {
          "const": "ng039-reservation-journal-event/v2"
        },
        "sequence": {
          "maximum": 128,
          "minimum": 1,
          "type": "integer"
        },
        "source_name": {
          "maxLength": 255,
          "type": [
            "string",
            "null"
          ]
        },
        "uid": {
          "minimum": 0,
          "type": "integer"
        }
      },
      "required": [
        "schema_version",
        "run_id",
        "sequence",
        "event",
        "monotonic_ns",
        "pid",
        "uid",
        "resource_type",
        "candidate_key",
        "source_name",
        "error_number",
        "identity"
      ],
      "type": "object"
    },
    "run_record": {
      "$defs": {
        "assertion_status": {
          "enum": [
            "PASS",
            "FAIL",
            "INCONCLUSIVE",
            "NOT_RUN"
          ]
        }
      },
      "$id": "urn:ng039:run-record:v5",
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "additionalProperties": false,
      "allOf": [
        {
          "if": {
            "properties": {
              "record_finalized": {
                "const": false
              }
            },
            "required": [
              "record_finalized"
            ]
          },
          "then": {
            "properties": {
              "failure": {
                "type": "null"
              },
              "finalization_mode": {
                "type": "null"
              },
              "status": {
                "const": "INCONCLUSIVE"
              }
            },
            "required": [
              "status",
              "failure",
              "finalization_mode"
            ]
          }
        },
        {
          "if": {
            "properties": {
              "status": {
                "const": "PASS"
              }
            },
            "required": [
              "status"
            ]
          },
          "then": {
            "properties": {
              "artifacts": {
                "allOf": [
                  {
                    "contains": {
                      "properties": {
                        "name": {
                          "const": "stdout.txt"
                        }
                      },
                      "required": [
                        "name"
                      ],
                      "type": "object"
                    },
                    "minContains": 1
                  },
                  {
                    "contains": {
                      "properties": {
                        "name": {
                          "const": "stderr.txt"
                        }
                      },
                      "required": [
                        "name"
                      ],
                      "type": "object"
                    },
                    "minContains": 1
                  },
                  {
                    "contains": {
                      "properties": {
                        "name": {
                          "const": "ipc-pre.json"
                        }
                      },
                      "required": [
                        "name"
                      ],
                      "type": "object"
                    },
                    "minContains": 1
                  },
                  {
                    "contains": {
                      "properties": {
                        "name": {
                          "const": "ipc-post.json"
                        }
                      },
                      "required": [
                        "name"
                      ],
                      "type": "object"
                    },
                    "minContains": 1
                  },
                  {
                    "contains": {
                      "properties": {
                        "name": {
                          "const": "ipc-final.json"
                        }
                      },
                      "required": [
                        "name"
                      ],
                      "type": "object"
                    },
                    "minContains": 1
                  },
                  {
                    "contains": {
                      "properties": {
                        "name": {
                          "const": "observer-events.json"
                        }
                      },
                      "required": [
                        "name"
                      ],
                      "type": "object"
                    },
                    "minContains": 1
                  }
                ]
              },
              "assertions": {
                "properties": {
                  "bootstrap_queue_drained": {
                    "const": "PASS"
                  },
                  "candidate_keys_absent_final": {
                    "const": "PASS"
                  },
                  "candidate_keys_absent_pre": {
                    "const": "PASS"
                  },
                  "candidate_resources_attributable": {
                    "const": "PASS"
                  },
                  "candidate_resources_removed": {
                    "const": "PASS"
                  },
                  "candidate_resources_reserved": {
                    "const": "PASS"
                  },
                  "evidence_complete": {
                    "const": "PASS"
                  },
                  "final_inventory_complete": {
                    "const": "PASS"
                  },
                  "final_inventory_matches_pre": {
                    "const": "PASS"
                  },
                  "finalization_complete": {
                    "const": "PASS"
                  },
                  "first_target_wait_fresh": {
                    "const": "PASS"
                  },
                  "gateway_joined": {
                    "const": "PASS"
                  },
                  "host_inventory_preflight": {
                    "const": "PASS"
                  },
                  "output_worker_joined": {
                    "const": "PASS"
                  },
                  "post_inventory_complete": {
                    "const": "PASS"
                  },
                  "pre_inventory_complete": {
                    "const": "PASS"
                  },
                  "process_blocks_verified": {
                    "const": "PASS"
                  },
                  "q0_empty_observation_delta": {
                    "const": "PASS"
                  },
                  "q0_zero_controlled_offers": {
                    "const": "PASS"
                  },
                  "scalar_run_identity_valid": {
                    "const": "PASS"
                  },
                  "target_admitted": {
                    "const": "PASS"
                  },
                  "target_delete_false_before_gateway": {
                    "const": "PASS"
                  },
                  "target_erased_once": {
                    "const": "PASS"
                  },
                  "target_future_at_end": {
                    "const": "PASS"
                  },
                  "target_is_head": {
                    "const": "PASS"
                  },
                  "target_not_popped": {
                    "const": "PASS"
                  },
                  "target_not_run": {
                    "const": "PASS"
                  },
                  "wait_decision_correlated": {
                    "const": "PASS"
                  }
                },
                "required": [
                  "bootstrap_queue_drained",
                  "evidence_complete",
                  "final_inventory_complete",
                  "final_inventory_matches_pre",
                  "finalization_complete",
                  "first_target_wait_fresh",
                  "gateway_joined",
                  "host_inventory_preflight",
                  "output_worker_joined",
                  "post_inventory_complete",
                  "candidate_resources_attributable",
                  "candidate_resources_removed",
                  "pre_inventory_complete",
                  "candidate_keys_absent_pre",
                  "process_blocks_verified",
                  "q0_empty_observation_delta",
                  "q0_zero_controlled_offers",
                  "scalar_run_identity_valid",
                  "target_admitted",
                  "target_delete_false_before_gateway",
                  "target_erased_once",
                  "target_future_at_end",
                  "target_is_head",
                  "target_not_popped",
                  "target_not_run",
                  "wait_decision_correlated",
                  "candidate_resources_reserved",
                  "candidate_keys_absent_final"
                ]
              },
              "cleanup": {
                "properties": {
                  "candidate_resources_removed": {
                    "const": true
                  },
                  "candidate_resources_reserved": {
                    "const": true
                  },
                  "complete": {
                    "const": true
                  },
                  "final_inventory_complete": {
                    "const": true
                  },
                  "final_inventory_matches_pre": {
                    "const": true
                  },
                  "finalizer_complete": {
                    "const": true
                  },
                  "gateway_joined": {
                    "const": true
                  },
                  "gateway_stop_requested": {
                    "const": true
                  },
                  "host_inventory_preflight": {
                    "const": true
                  },
                  "output_worker_joined": {
                    "const": true
                  },
                  "post_candidate_resources_attributable": {
                    "const": true
                  },
                  "process_destroyed": {
                    "const": true
                  },
                  "target_deleted_once": {
                    "const": true
                  },
                  "target_detached": {
                    "const": true
                  }
                },
                "required": [
                  "complete",
                  "final_inventory_complete",
                  "finalizer_complete",
                  "gateway_joined",
                  "gateway_stop_requested",
                  "output_worker_joined",
                  "post_candidate_resources_attributable",
                  "candidate_resources_removed",
                  "process_destroyed",
                  "target_deleted_once",
                  "target_detached",
                  "host_inventory_preflight",
                  "candidate_resources_reserved",
                  "final_inventory_matches_pre"
                ]
              },
              "execution_identity": {
                "properties": {
                  "dedicated_uid_verified": {
                    "const": true
                  },
                  "interactive_login_disabled": {
                    "const": true
                  },
                  "launcher_path": {
                    "minLength": 1,
                    "type": "string"
                  },
                  "launcher_sha256": {
                    "pattern": "^[0-9a-f]{64}$",
                    "type": "string"
                  },
                  "launcher_verified": {
                    "const": true
                  },
                  "lease_acquired": {
                    "const": true
                  },
                  "lease_held_through_finalizer": {
                    "const": true
                  },
                  "no_concurrent_uid_process": {
                    "const": true
                  }
                },
                "required": [
                  "dedicated_uid_verified",
                  "launcher_verified",
                  "launcher_path",
                  "launcher_sha256",
                  "lease_acquired",
                  "lease_held_through_finalizer",
                  "no_concurrent_uid_process",
                  "interactive_login_disabled"
                ],
                "type": "object"
              },
              "exit": {
                "properties": {
                  "harness_code": {
                    "const": 0
                  },
                  "outer_code": {
                    "const": 0
                  },
                  "signal": {
                    "type": "null"
                  },
                  "timeout_fired": {
                    "const": false
                  }
                },
                "required": [
                  "harness_code",
                  "outer_code",
                  "signal",
                  "timeout_fired"
                ]
              },
              "failure": {
                "type": "null"
              },
              "finalization_mode": {
                "const": "normal_cleanup"
              },
              "inventories": {
                "properties": {
                  "final": {
                    "properties": {
                      "available": {
                        "const": true
                      },
                      "complete": {
                        "const": true
                      }
                    },
                    "required": [
                      "available",
                      "complete"
                    ]
                  },
                  "post": {
                    "properties": {
                      "available": {
                        "const": true
                      },
                      "complete": {
                        "const": true
                      }
                    },
                    "required": [
                      "available",
                      "complete"
                    ]
                  },
                  "pre": {
                    "properties": {
                      "available": {
                        "const": true
                      },
                      "complete": {
                        "const": true
                      }
                    },
                    "required": [
                      "available",
                      "complete"
                    ]
                  }
                },
                "required": [
                  "pre",
                  "post",
                  "final"
                ]
              },
              "observations": {
                "properties": {
                  "counter_overflow": {
                    "const": false
                  },
                  "q1": {
                    "properties": {
                      "correlation_status": {
                        "const": "MATCHED"
                      }
                    },
                    "required": [
                      "correlation_status"
                    ]
                  }
                },
                "required": [
                  "counter_overflow",
                  "q1"
                ]
              },
              "record_finalized": {
                "const": true
              },
              "reservations": {
                "properties": {
                  "candidate_keys": {
                    "maxItems": 5,
                    "minItems": 5,
                    "type": "array"
                  },
                  "d2_process_key": {
                    "maximum": 2147483643,
                    "minimum": 1,
                    "type": "integer"
                  },
                  "exclusive_reservation_complete": {
                    "const": true
                  },
                  "journal_artifact_name": {
                    "const": "reservation-journal.jsonl"
                  },
                  "journal_closed": {
                    "const": true
                  },
                  "journal_event_count": {
                    "maximum": 128,
                    "minimum": 1,
                    "type": "integer"
                  },
                  "journal_fsynced": {
                    "const": true
                  },
                  "journal_sha256": {
                    "pattern": "^[0-9a-f]{64}$",
                    "type": "string"
                  },
                  "libc_identity": {
                    "properties": {
                      "provider_path": {
                        "minLength": 1,
                        "type": "string"
                      },
                      "sha256": {
                        "pattern": "^[0-9a-f]{64}$",
                        "type": "string"
                      },
                      "version": {
                        "minLength": 4,
                        "type": "string"
                      }
                    },
                    "required": [
                      "provider_path",
                      "sha256",
                      "version"
                    ]
                  },
                  "libc_version": {
                    "minLength": 4,
                    "type": "string"
                  },
                  "posix_semaphores": {
                    "maxItems": 5,
                    "minItems": 5
                  },
                  "reservation_umask": {
                    "const": 63
                  },
                  "selected_attempt": {
                    "maximum": 31,
                    "minimum": 0,
                    "type": "integer"
                  },
                  "semaphore_name_mapping_verified": {
                    "const": true
                  },
                  "sysv_shm": {
                    "maxItems": 5,
                    "minItems": 5
                  }
                },
                "required": [
                  "exclusive_reservation_complete",
                  "journal_fsynced",
                  "libc_version",
                  "semaphore_name_mapping_verified",
                  "reservation_umask",
                  "sysv_shm",
                  "posix_semaphores",
                  "journal_artifact_name",
                  "journal_closed",
                  "journal_event_count",
                  "journal_sha256",
                  "libc_identity",
                  "candidate_keys",
                  "d2_process_key",
                  "selected_attempt"
                ]
              }
            },
            "required": [
              "record_finalized",
              "failure",
              "exit",
              "assertions",
              "artifacts",
              "inventories",
              "cleanup",
              "observations",
              "reservations",
              "execution_identity",
              "finalization_mode"
            ]
          }
        },
        {
          "if": {
            "properties": {
              "status": {
                "const": "FAIL"
              }
            },
            "required": [
              "status"
            ]
          },
          "then": {
            "properties": {
              "artifacts": {
                "allOf": [
                  {
                    "contains": {
                      "properties": {
                        "name": {
                          "const": "stdout.txt"
                        }
                      },
                      "required": [
                        "name"
                      ],
                      "type": "object"
                    },
                    "minContains": 1
                  },
                  {
                    "contains": {
                      "properties": {
                        "name": {
                          "const": "stderr.txt"
                        }
                      },
                      "required": [
                        "name"
                      ],
                      "type": "object"
                    },
                    "minContains": 1
                  },
                  {
                    "contains": {
                      "properties": {
                        "name": {
                          "const": "ipc-pre.json"
                        }
                      },
                      "required": [
                        "name"
                      ],
                      "type": "object"
                    },
                    "minContains": 1
                  },
                  {
                    "contains": {
                      "properties": {
                        "name": {
                          "const": "ipc-post.json"
                        }
                      },
                      "required": [
                        "name"
                      ],
                      "type": "object"
                    },
                    "minContains": 1
                  },
                  {
                    "contains": {
                      "properties": {
                        "name": {
                          "const": "ipc-final.json"
                        }
                      },
                      "required": [
                        "name"
                      ],
                      "type": "object"
                    },
                    "minContains": 1
                  },
                  {
                    "contains": {
                      "properties": {
                        "name": {
                          "const": "observer-events.json"
                        }
                      },
                      "required": [
                        "name"
                      ],
                      "type": "object"
                    },
                    "minContains": 1
                  }
                ]
              },
              "assertions": {
                "allOf": [
                  {
                    "anyOf": [
                      {
                        "properties": {
                          "bootstrap_queue_drained": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "bootstrap_queue_drained"
                        ]
                      },
                      {
                        "properties": {
                          "evidence_complete": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "evidence_complete"
                        ]
                      },
                      {
                        "properties": {
                          "final_inventory_complete": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "final_inventory_complete"
                        ]
                      },
                      {
                        "properties": {
                          "final_inventory_matches_pre": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "final_inventory_matches_pre"
                        ]
                      },
                      {
                        "properties": {
                          "finalization_complete": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "finalization_complete"
                        ]
                      },
                      {
                        "properties": {
                          "first_target_wait_fresh": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "first_target_wait_fresh"
                        ]
                      },
                      {
                        "properties": {
                          "gateway_joined": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "gateway_joined"
                        ]
                      },
                      {
                        "properties": {
                          "host_inventory_preflight": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "host_inventory_preflight"
                        ]
                      },
                      {
                        "properties": {
                          "output_worker_joined": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "output_worker_joined"
                        ]
                      },
                      {
                        "properties": {
                          "post_inventory_complete": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "post_inventory_complete"
                        ]
                      },
                      {
                        "properties": {
                          "candidate_resources_attributable": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "candidate_resources_attributable"
                        ]
                      },
                      {
                        "properties": {
                          "candidate_resources_removed": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "candidate_resources_removed"
                        ]
                      },
                      {
                        "properties": {
                          "pre_inventory_complete": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "pre_inventory_complete"
                        ]
                      },
                      {
                        "properties": {
                          "candidate_keys_absent_pre": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "candidate_keys_absent_pre"
                        ]
                      },
                      {
                        "properties": {
                          "process_blocks_verified": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "process_blocks_verified"
                        ]
                      },
                      {
                        "properties": {
                          "q0_empty_observation_delta": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "q0_empty_observation_delta"
                        ]
                      },
                      {
                        "properties": {
                          "q0_zero_controlled_offers": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "q0_zero_controlled_offers"
                        ]
                      },
                      {
                        "properties": {
                          "scalar_run_identity_valid": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "scalar_run_identity_valid"
                        ]
                      },
                      {
                        "properties": {
                          "target_admitted": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "target_admitted"
                        ]
                      },
                      {
                        "properties": {
                          "target_delete_false_before_gateway": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "target_delete_false_before_gateway"
                        ]
                      },
                      {
                        "properties": {
                          "target_erased_once": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "target_erased_once"
                        ]
                      },
                      {
                        "properties": {
                          "target_future_at_end": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "target_future_at_end"
                        ]
                      },
                      {
                        "properties": {
                          "target_is_head": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "target_is_head"
                        ]
                      },
                      {
                        "properties": {
                          "target_not_popped": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "target_not_popped"
                        ]
                      },
                      {
                        "properties": {
                          "target_not_run": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "target_not_run"
                        ]
                      },
                      {
                        "properties": {
                          "wait_decision_correlated": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "wait_decision_correlated"
                        ]
                      }
                    ]
                  },
                  {
                    "properties": {
                      "evidence_complete": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "evidence_complete"
                    ]
                  },
                  {
                    "properties": {
                      "final_inventory_complete": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "final_inventory_complete"
                    ]
                  },
                  {
                    "properties": {
                      "final_inventory_matches_pre": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "final_inventory_matches_pre"
                    ]
                  },
                  {
                    "properties": {
                      "finalization_complete": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "finalization_complete"
                    ]
                  },
                  {
                    "properties": {
                      "gateway_joined": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "gateway_joined"
                    ]
                  },
                  {
                    "properties": {
                      "host_inventory_preflight": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "host_inventory_preflight"
                    ]
                  },
                  {
                    "properties": {
                      "output_worker_joined": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "output_worker_joined"
                    ]
                  },
                  {
                    "properties": {
                      "post_inventory_complete": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "post_inventory_complete"
                    ]
                  },
                  {
                    "properties": {
                      "candidate_resources_attributable": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "candidate_resources_attributable"
                    ]
                  },
                  {
                    "properties": {
                      "candidate_resources_removed": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "candidate_resources_removed"
                    ]
                  },
                  {
                    "properties": {
                      "pre_inventory_complete": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "pre_inventory_complete"
                    ]
                  },
                  {
                    "properties": {
                      "candidate_keys_absent_pre": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "candidate_keys_absent_pre"
                    ]
                  },
                  {
                    "properties": {
                      "process_blocks_verified": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "process_blocks_verified"
                    ]
                  },
                  {
                    "properties": {
                      "scalar_run_identity_valid": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "scalar_run_identity_valid"
                    ]
                  },
                  {
                    "properties": {
                      "target_erased_once": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "target_erased_once"
                    ]
                  },
                  {
                    "properties": {
                      "wait_decision_correlated": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "wait_decision_correlated"
                    ]
                  }
                ],
                "properties": {
                  "bootstrap_queue_drained": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "candidate_keys_absent_final": {
                    "const": "PASS"
                  },
                  "candidate_keys_absent_pre": {
                    "const": "PASS"
                  },
                  "candidate_resources_attributable": {
                    "const": "PASS"
                  },
                  "candidate_resources_removed": {
                    "const": "PASS"
                  },
                  "candidate_resources_reserved": {
                    "const": "PASS"
                  },
                  "evidence_complete": {
                    "const": "PASS"
                  },
                  "final_inventory_complete": {
                    "const": "PASS"
                  },
                  "final_inventory_matches_pre": {
                    "const": "PASS"
                  },
                  "finalization_complete": {
                    "const": "PASS"
                  },
                  "first_target_wait_fresh": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "gateway_joined": {
                    "const": "PASS"
                  },
                  "host_inventory_preflight": {
                    "const": "PASS"
                  },
                  "output_worker_joined": {
                    "const": "PASS"
                  },
                  "post_inventory_complete": {
                    "const": "PASS"
                  },
                  "pre_inventory_complete": {
                    "const": "PASS"
                  },
                  "process_blocks_verified": {
                    "const": "PASS"
                  },
                  "q0_empty_observation_delta": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "q0_zero_controlled_offers": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "scalar_run_identity_valid": {
                    "const": "PASS"
                  },
                  "target_admitted": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "target_delete_false_before_gateway": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "target_erased_once": {
                    "const": "PASS"
                  },
                  "target_future_at_end": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "target_is_head": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "target_not_popped": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "target_not_run": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "wait_decision_correlated": {
                    "const": "PASS"
                  }
                },
                "required": [
                  "bootstrap_queue_drained",
                  "evidence_complete",
                  "final_inventory_complete",
                  "final_inventory_matches_pre",
                  "finalization_complete",
                  "first_target_wait_fresh",
                  "gateway_joined",
                  "host_inventory_preflight",
                  "output_worker_joined",
                  "post_inventory_complete",
                  "candidate_resources_attributable",
                  "candidate_resources_removed",
                  "pre_inventory_complete",
                  "candidate_keys_absent_pre",
                  "process_blocks_verified",
                  "q0_empty_observation_delta",
                  "q0_zero_controlled_offers",
                  "scalar_run_identity_valid",
                  "target_admitted",
                  "target_delete_false_before_gateway",
                  "target_erased_once",
                  "target_future_at_end",
                  "target_is_head",
                  "target_not_popped",
                  "target_not_run",
                  "wait_decision_correlated",
                  "candidate_resources_reserved",
                  "candidate_keys_absent_final"
                ]
              },
              "cleanup": {
                "properties": {
                  "candidate_resources_removed": {
                    "const": true
                  },
                  "candidate_resources_reserved": {
                    "const": true
                  },
                  "complete": {
                    "const": true
                  },
                  "final_inventory_complete": {
                    "const": true
                  },
                  "final_inventory_matches_pre": {
                    "const": true
                  },
                  "finalizer_complete": {
                    "const": true
                  },
                  "gateway_joined": {
                    "const": true
                  },
                  "gateway_stop_requested": {
                    "const": true
                  },
                  "host_inventory_preflight": {
                    "const": true
                  },
                  "output_worker_joined": {
                    "const": true
                  },
                  "post_candidate_resources_attributable": {
                    "const": true
                  },
                  "process_destroyed": {
                    "const": true
                  },
                  "target_deleted_once": {
                    "const": true
                  },
                  "target_detached": {
                    "const": true
                  }
                },
                "required": [
                  "complete",
                  "final_inventory_complete",
                  "finalizer_complete",
                  "gateway_joined",
                  "gateway_stop_requested",
                  "output_worker_joined",
                  "post_candidate_resources_attributable",
                  "candidate_resources_removed",
                  "process_destroyed",
                  "target_deleted_once",
                  "target_detached",
                  "host_inventory_preflight",
                  "candidate_resources_reserved",
                  "final_inventory_matches_pre"
                ]
              },
              "execution_identity": {
                "properties": {
                  "dedicated_uid_verified": {
                    "const": true
                  },
                  "interactive_login_disabled": {
                    "const": true
                  },
                  "launcher_path": {
                    "minLength": 1,
                    "type": "string"
                  },
                  "launcher_sha256": {
                    "pattern": "^[0-9a-f]{64}$",
                    "type": "string"
                  },
                  "launcher_verified": {
                    "const": true
                  },
                  "lease_acquired": {
                    "const": true
                  },
                  "lease_held_through_finalizer": {
                    "const": true
                  },
                  "no_concurrent_uid_process": {
                    "const": true
                  }
                },
                "required": [
                  "dedicated_uid_verified",
                  "launcher_verified",
                  "launcher_path",
                  "launcher_sha256",
                  "lease_acquired",
                  "lease_held_through_finalizer",
                  "no_concurrent_uid_process",
                  "interactive_login_disabled"
                ],
                "type": "object"
              },
              "exit": {
                "properties": {
                  "harness_code": {
                    "maximum": 255,
                    "minimum": 0,
                    "type": "integer"
                  },
                  "outer_code": {
                    "maximum": 123,
                    "minimum": 0,
                    "type": "integer"
                  },
                  "signal": {
                    "type": "null"
                  },
                  "timeout_fired": {
                    "const": false
                  }
                },
                "required": [
                  "harness_code",
                  "outer_code",
                  "signal",
                  "timeout_fired"
                ]
              },
              "failure": {
                "properties": {
                  "code": {
                    "const": "ASSERTION_FAILED"
                  }
                },
                "required": [
                  "code"
                ],
                "type": "object"
              },
              "finalization_mode": {
                "const": "normal_cleanup"
              },
              "inventories": {
                "properties": {
                  "final": {
                    "properties": {
                      "available": {
                        "const": true
                      },
                      "complete": {
                        "const": true
                      }
                    },
                    "required": [
                      "available",
                      "complete"
                    ]
                  },
                  "post": {
                    "properties": {
                      "available": {
                        "const": true
                      },
                      "complete": {
                        "const": true
                      }
                    },
                    "required": [
                      "available",
                      "complete"
                    ]
                  },
                  "pre": {
                    "properties": {
                      "available": {
                        "const": true
                      },
                      "complete": {
                        "const": true
                      }
                    },
                    "required": [
                      "available",
                      "complete"
                    ]
                  }
                },
                "required": [
                  "pre",
                  "post",
                  "final"
                ]
              },
              "observations": {
                "properties": {
                  "counter_overflow": {
                    "const": false
                  },
                  "q1": {
                    "properties": {
                      "correlation_status": {
                        "const": "MATCHED"
                      }
                    },
                    "required": [
                      "correlation_status"
                    ]
                  }
                },
                "required": [
                  "counter_overflow",
                  "q1"
                ]
              },
              "record_finalized": {
                "const": true
              },
              "reservations": {
                "properties": {
                  "candidate_keys": {
                    "maxItems": 5,
                    "minItems": 5,
                    "type": "array"
                  },
                  "d2_process_key": {
                    "maximum": 2147483643,
                    "minimum": 1,
                    "type": "integer"
                  },
                  "exclusive_reservation_complete": {
                    "const": true
                  },
                  "journal_artifact_name": {
                    "const": "reservation-journal.jsonl"
                  },
                  "journal_closed": {
                    "const": true
                  },
                  "journal_event_count": {
                    "maximum": 128,
                    "minimum": 1,
                    "type": "integer"
                  },
                  "journal_fsynced": {
                    "const": true
                  },
                  "journal_sha256": {
                    "pattern": "^[0-9a-f]{64}$",
                    "type": "string"
                  },
                  "libc_identity": {
                    "properties": {
                      "provider_path": {
                        "minLength": 1,
                        "type": "string"
                      },
                      "sha256": {
                        "pattern": "^[0-9a-f]{64}$",
                        "type": "string"
                      },
                      "version": {
                        "minLength": 4,
                        "type": "string"
                      }
                    },
                    "required": [
                      "provider_path",
                      "sha256",
                      "version"
                    ]
                  },
                  "libc_version": {
                    "minLength": 4,
                    "type": "string"
                  },
                  "posix_semaphores": {
                    "maxItems": 5,
                    "minItems": 5
                  },
                  "reservation_umask": {
                    "const": 63
                  },
                  "selected_attempt": {
                    "maximum": 31,
                    "minimum": 0,
                    "type": "integer"
                  },
                  "semaphore_name_mapping_verified": {
                    "const": true
                  },
                  "sysv_shm": {
                    "maxItems": 5,
                    "minItems": 5
                  }
                },
                "required": [
                  "exclusive_reservation_complete",
                  "journal_fsynced",
                  "libc_version",
                  "semaphore_name_mapping_verified",
                  "reservation_umask",
                  "sysv_shm",
                  "posix_semaphores",
                  "journal_artifact_name",
                  "journal_closed",
                  "journal_event_count",
                  "journal_sha256",
                  "libc_identity",
                  "candidate_keys",
                  "d2_process_key",
                  "selected_attempt"
                ]
              }
            },
            "required": [
              "record_finalized",
              "failure",
              "exit",
              "assertions",
              "artifacts",
              "inventories",
              "cleanup",
              "observations",
              "reservations",
              "execution_identity",
              "finalization_mode"
            ]
          }
        },
        {
          "if": {
            "properties": {
              "record_finalized": {
                "const": true
              },
              "status": {
                "const": "INCONCLUSIVE"
              }
            },
            "required": [
              "record_finalized",
              "status"
            ]
          },
          "then": {
            "properties": {
              "failure": {
                "type": "object"
              }
            },
            "required": [
              "failure"
            ]
          }
        },
        {
          "if": {
            "properties": {
              "status": {
                "const": "PASS"
              }
            },
            "required": [
              "status"
            ]
          },
          "then": {
            "properties": {
              "assertions": {
                "properties": {
                  "candidate_keys_absent_final": {
                    "const": "PASS"
                  },
                  "candidate_keys_absent_pre": {
                    "const": "PASS"
                  },
                  "candidate_resources_attributable": {
                    "const": "PASS"
                  },
                  "candidate_resources_removed": {
                    "const": "PASS"
                  },
                  "candidate_resources_reserved": {
                    "const": "PASS"
                  },
                  "evidence_complete": {
                    "const": "PASS"
                  },
                  "final_inventory_complete": {
                    "const": "PASS"
                  },
                  "final_inventory_matches_pre": {
                    "const": "PASS"
                  },
                  "finalization_complete": {
                    "const": "PASS"
                  },
                  "gateway_joined": {
                    "const": "PASS"
                  },
                  "host_inventory_preflight": {
                    "const": "PASS"
                  },
                  "output_worker_joined": {
                    "const": "PASS"
                  },
                  "post_inventory_complete": {
                    "const": "PASS"
                  },
                  "pre_inventory_complete": {
                    "const": "PASS"
                  },
                  "process_blocks_verified": {
                    "const": "PASS"
                  },
                  "scalar_run_identity_valid": {
                    "const": "PASS"
                  },
                  "target_erased_once": {
                    "const": "PASS"
                  },
                  "wait_decision_correlated": {
                    "const": "PASS"
                  }
                },
                "required": [
                  "host_inventory_preflight",
                  "pre_inventory_complete",
                  "candidate_keys_absent_pre",
                  "candidate_resources_reserved",
                  "process_blocks_verified",
                  "gateway_joined",
                  "output_worker_joined",
                  "target_erased_once",
                  "post_inventory_complete",
                  "candidate_resources_attributable",
                  "candidate_resources_removed",
                  "final_inventory_complete",
                  "candidate_keys_absent_final",
                  "final_inventory_matches_pre",
                  "finalization_complete",
                  "evidence_complete",
                  "wait_decision_correlated",
                  "scalar_run_identity_valid"
                ]
              },
              "cleanup": {
                "properties": {
                  "candidate_resources_removed": {
                    "const": true
                  },
                  "candidate_resources_reserved": {
                    "const": true
                  },
                  "complete": {
                    "const": true
                  },
                  "final_inventory_complete": {
                    "const": true
                  },
                  "final_inventory_matches_pre": {
                    "const": true
                  },
                  "finalizer_complete": {
                    "const": true
                  },
                  "gateway_joined": {
                    "const": true
                  },
                  "host_inventory_preflight": {
                    "const": true
                  },
                  "output_worker_joined": {
                    "const": true
                  },
                  "process_destroyed": {
                    "const": true
                  },
                  "target_deleted_once": {
                    "const": true
                  },
                  "target_detached": {
                    "const": true
                  }
                },
                "required": [
                  "complete",
                  "host_inventory_preflight",
                  "candidate_resources_reserved",
                  "candidate_resources_removed",
                  "final_inventory_complete",
                  "final_inventory_matches_pre",
                  "finalizer_complete",
                  "gateway_joined",
                  "output_worker_joined",
                  "process_destroyed",
                  "target_deleted_once",
                  "target_detached"
                ]
              },
              "execution_identity": {
                "properties": {
                  "dedicated_uid_verified": {
                    "const": true
                  },
                  "interactive_login_disabled": {
                    "const": true
                  },
                  "launcher_path": {
                    "minLength": 1,
                    "type": "string"
                  },
                  "launcher_sha256": {
                    "pattern": "^[0-9a-f]{64}$",
                    "type": "string"
                  },
                  "launcher_verified": {
                    "const": true
                  },
                  "lease_acquired": {
                    "const": true
                  },
                  "lease_held_through_finalizer": {
                    "const": true
                  },
                  "no_concurrent_uid_process": {
                    "const": true
                  }
                },
                "required": [
                  "dedicated_uid_verified",
                  "launcher_verified",
                  "launcher_path",
                  "launcher_sha256",
                  "lease_acquired",
                  "lease_held_through_finalizer",
                  "no_concurrent_uid_process",
                  "interactive_login_disabled"
                ],
                "type": "object"
              },
              "finalization_mode": {
                "const": "normal_cleanup"
              },
              "observations": {
                "properties": {
                  "q0": {
                    "properties": {
                      "controlled_application_offers": {
                        "const": 0
                      },
                      "empty_observation_delta": {
                        "minimum": 1,
                        "type": "integer"
                      },
                      "end_sequence": {
                        "type": "integer"
                      },
                      "start_sequence": {
                        "type": "integer"
                      }
                    },
                    "required": [
                      "start_sequence",
                      "end_sequence",
                      "controlled_application_offers",
                      "empty_observation_delta"
                    ]
                  },
                  "q1": {
                    "properties": {
                      "actual_due_branch": {
                        "const": false
                      },
                      "actual_popped": {
                        "const": false
                      },
                      "correlation_status": {
                        "const": "MATCHED"
                      },
                      "decision_candidate_id": {
                        "minimum": 1,
                        "type": "integer"
                      },
                      "decision_candidate_time": {
                        "type": "number"
                      },
                      "decision_iteration_id": {
                        "minimum": 1,
                        "type": "integer"
                      },
                      "decision_sequence": {
                        "minimum": 0,
                        "type": "integer"
                      },
                      "end_sequence": {
                        "minimum": 0,
                        "type": "integer"
                      },
                      "first_target_wait_sequence": {
                        "minimum": 0,
                        "type": "integer"
                      },
                      "gateway_iteration_id": {
                        "minimum": 1,
                        "type": "integer"
                      },
                      "head_time_at_end": {
                        "type": "number"
                      },
                      "process_time_at_end": {
                        "type": "number"
                      },
                      "start_sequence": {
                        "minimum": 0,
                        "type": "integer"
                      },
                      "target_message_id": {
                        "minimum": 1,
                        "type": "integer"
                      },
                      "target_pop_count": {
                        "const": 0
                      },
                      "target_run_entry_count": {
                        "const": 0
                      },
                      "target_run_return_count": {
                        "const": 0
                      },
                      "wait_depth": {
                        "minimum": 1,
                        "type": "integer"
                      },
                      "wait_head_id": {
                        "minimum": 1,
                        "type": "integer"
                      },
                      "wait_head_time": {
                        "type": "number"
                      },
                      "wait_iteration_id": {
                        "minimum": 1,
                        "type": "integer"
                      },
                      "wait_predicate_return": {
                        "type": "boolean"
                      },
                      "wait_sequence": {
                        "minimum": 0,
                        "type": "integer"
                      },
                      "wait_target_id": {
                        "minimum": 1,
                        "type": "integer"
                      },
                      "wait_timeout_ms": {
                        "minimum": 0,
                        "type": "integer"
                      }
                    },
                    "required": [
                      "target_message_id",
                      "start_sequence",
                      "end_sequence",
                      "first_target_wait_sequence",
                      "wait_sequence",
                      "decision_sequence",
                      "gateway_iteration_id",
                      "wait_iteration_id",
                      "decision_iteration_id",
                      "wait_target_id",
                      "wait_head_id",
                      "decision_candidate_id",
                      "wait_depth",
                      "wait_timeout_ms",
                      "wait_head_time",
                      "decision_candidate_time",
                      "head_time_at_end",
                      "process_time_at_end",
                      "wait_predicate_return",
                      "actual_due_branch",
                      "actual_popped",
                      "target_pop_count",
                      "target_run_entry_count",
                      "target_run_return_count",
                      "correlation_status"
                    ]
                  }
                },
                "required": [
                  "q0",
                  "q1"
                ]
              },
              "reservations": {
                "properties": {
                  "candidate_keys": {
                    "maxItems": 5,
                    "minItems": 5,
                    "type": "array"
                  },
                  "d2_process_key": {
                    "maximum": 2147483643,
                    "minimum": 1,
                    "type": "integer"
                  },
                  "exclusive_reservation_complete": {
                    "const": true
                  },
                  "journal_artifact_name": {
                    "const": "reservation-journal.jsonl"
                  },
                  "journal_closed": {
                    "const": true
                  },
                  "journal_event_count": {
                    "maximum": 128,
                    "minimum": 1,
                    "type": "integer"
                  },
                  "journal_fsynced": {
                    "const": true
                  },
                  "journal_sha256": {
                    "pattern": "^[0-9a-f]{64}$",
                    "type": "string"
                  },
                  "libc_identity": {
                    "properties": {
                      "provider_path": {
                        "minLength": 1,
                        "type": "string"
                      },
                      "sha256": {
                        "pattern": "^[0-9a-f]{64}$",
                        "type": "string"
                      },
                      "version": {
                        "minLength": 4,
                        "type": "string"
                      }
                    },
                    "required": [
                      "provider_path",
                      "sha256",
                      "version"
                    ]
                  },
                  "libc_version": {
                    "minLength": 4,
                    "type": "string"
                  },
                  "posix_semaphores": {
                    "maxItems": 5,
                    "minItems": 5
                  },
                  "reservation_umask": {
                    "const": 63
                  },
                  "selected_attempt": {
                    "maximum": 31,
                    "minimum": 0,
                    "type": "integer"
                  },
                  "semaphore_name_mapping_verified": {
                    "const": true
                  },
                  "sysv_shm": {
                    "maxItems": 5,
                    "minItems": 5
                  }
                },
                "required": [
                  "exclusive_reservation_complete",
                  "journal_fsynced",
                  "libc_version",
                  "semaphore_name_mapping_verified",
                  "reservation_umask",
                  "sysv_shm",
                  "posix_semaphores",
                  "journal_artifact_name",
                  "journal_closed",
                  "journal_event_count",
                  "journal_sha256",
                  "libc_identity",
                  "candidate_keys",
                  "d2_process_key",
                  "selected_attempt"
                ]
              }
            },
            "required": [
              "reservations",
              "execution_identity",
              "finalization_mode"
            ]
          }
        },
        {
          "if": {
            "properties": {
              "status": {
                "const": "FAIL"
              }
            },
            "required": [
              "status"
            ]
          },
          "then": {
            "properties": {
              "artifacts": {
                "minItems": 6
              },
              "assertions": {
                "properties": {
                  "candidate_keys_absent_final": {
                    "const": "PASS"
                  },
                  "candidate_keys_absent_pre": {
                    "const": "PASS"
                  },
                  "candidate_resources_attributable": {
                    "const": "PASS"
                  },
                  "candidate_resources_removed": {
                    "const": "PASS"
                  },
                  "candidate_resources_reserved": {
                    "const": "PASS"
                  },
                  "evidence_complete": {
                    "const": "PASS"
                  },
                  "final_inventory_complete": {
                    "const": "PASS"
                  },
                  "final_inventory_matches_pre": {
                    "const": "PASS"
                  },
                  "finalization_complete": {
                    "const": "PASS"
                  },
                  "gateway_joined": {
                    "const": "PASS"
                  },
                  "host_inventory_preflight": {
                    "const": "PASS"
                  },
                  "output_worker_joined": {
                    "const": "PASS"
                  },
                  "post_inventory_complete": {
                    "const": "PASS"
                  },
                  "pre_inventory_complete": {
                    "const": "PASS"
                  },
                  "process_blocks_verified": {
                    "const": "PASS"
                  },
                  "scalar_run_identity_valid": {
                    "const": "PASS"
                  },
                  "target_erased_once": {
                    "const": "PASS"
                  },
                  "wait_decision_correlated": {
                    "const": "PASS"
                  }
                },
                "required": [
                  "host_inventory_preflight",
                  "pre_inventory_complete",
                  "candidate_keys_absent_pre",
                  "candidate_resources_reserved",
                  "process_blocks_verified",
                  "gateway_joined",
                  "output_worker_joined",
                  "target_erased_once",
                  "post_inventory_complete",
                  "candidate_resources_attributable",
                  "candidate_resources_removed",
                  "final_inventory_complete",
                  "candidate_keys_absent_final",
                  "final_inventory_matches_pre",
                  "finalization_complete",
                  "evidence_complete",
                  "wait_decision_correlated",
                  "scalar_run_identity_valid"
                ]
              },
              "cleanup": {
                "properties": {
                  "candidate_resources_removed": {
                    "const": true
                  },
                  "candidate_resources_reserved": {
                    "const": true
                  },
                  "complete": {
                    "const": true
                  },
                  "final_inventory_complete": {
                    "const": true
                  },
                  "final_inventory_matches_pre": {
                    "const": true
                  },
                  "finalizer_complete": {
                    "const": true
                  },
                  "gateway_joined": {
                    "const": true
                  },
                  "gateway_stop_requested": {
                    "const": true
                  },
                  "host_inventory_preflight": {
                    "const": true
                  },
                  "output_worker_joined": {
                    "const": true
                  },
                  "post_candidate_resources_attributable": {
                    "const": true
                  },
                  "process_destroyed": {
                    "const": true
                  },
                  "target_deleted_once": {
                    "const": true
                  },
                  "target_detached": {
                    "const": true
                  }
                },
                "required": [
                  "complete",
                  "host_inventory_preflight",
                  "candidate_resources_reserved",
                  "candidate_resources_removed",
                  "final_inventory_complete",
                  "final_inventory_matches_pre",
                  "finalizer_complete",
                  "gateway_joined",
                  "output_worker_joined",
                  "process_destroyed",
                  "target_deleted_once",
                  "target_detached"
                ]
              },
              "execution_identity": {
                "properties": {
                  "dedicated_uid_verified": {
                    "const": true
                  },
                  "interactive_login_disabled": {
                    "const": true
                  },
                  "launcher_path": {
                    "minLength": 1,
                    "type": "string"
                  },
                  "launcher_sha256": {
                    "pattern": "^[0-9a-f]{64}$",
                    "type": "string"
                  },
                  "launcher_verified": {
                    "const": true
                  },
                  "lease_acquired": {
                    "const": true
                  },
                  "lease_held_through_finalizer": {
                    "const": true
                  },
                  "no_concurrent_uid_process": {
                    "const": true
                  }
                },
                "required": [
                  "dedicated_uid_verified",
                  "launcher_verified",
                  "launcher_path",
                  "launcher_sha256",
                  "lease_acquired",
                  "lease_held_through_finalizer",
                  "no_concurrent_uid_process",
                  "interactive_login_disabled"
                ],
                "type": "object"
              },
              "failure": {
                "properties": {
                  "cleanup_outcome": {
                    "const": "complete"
                  },
                  "code": {
                    "const": "ASSERTION_FAILED"
                  },
                  "inventory_availability": {
                    "properties": {
                      "final": {
                        "const": true
                      },
                      "post": {
                        "const": true
                      },
                      "pre": {
                        "const": true
                      }
                    },
                    "required": [
                      "pre",
                      "post",
                      "final"
                    ]
                  },
                  "missing_artifacts": {
                    "const": []
                  }
                },
                "required": [
                  "code",
                  "cleanup_outcome",
                  "missing_artifacts",
                  "inventory_availability"
                ]
              },
              "finalization_mode": {
                "const": "normal_cleanup"
              },
              "inventories": {
                "properties": {
                  "final": {
                    "properties": {
                      "available": {
                        "const": true
                      },
                      "complete": {
                        "const": true
                      }
                    },
                    "required": [
                      "available",
                      "complete"
                    ]
                  },
                  "post": {
                    "properties": {
                      "available": {
                        "const": true
                      },
                      "complete": {
                        "const": true
                      }
                    },
                    "required": [
                      "available",
                      "complete"
                    ]
                  },
                  "pre": {
                    "properties": {
                      "available": {
                        "const": true
                      },
                      "complete": {
                        "const": true
                      }
                    },
                    "required": [
                      "available",
                      "complete"
                    ]
                  }
                },
                "required": [
                  "pre",
                  "post",
                  "final"
                ]
              },
              "reservations": {
                "properties": {
                  "candidate_keys": {
                    "maxItems": 5,
                    "minItems": 5,
                    "type": "array"
                  },
                  "d2_process_key": {
                    "maximum": 2147483643,
                    "minimum": 1,
                    "type": "integer"
                  },
                  "exclusive_reservation_complete": {
                    "const": true
                  },
                  "journal_artifact_name": {
                    "const": "reservation-journal.jsonl"
                  },
                  "journal_closed": {
                    "const": true
                  },
                  "journal_event_count": {
                    "maximum": 128,
                    "minimum": 1,
                    "type": "integer"
                  },
                  "journal_fsynced": {
                    "const": true
                  },
                  "journal_sha256": {
                    "pattern": "^[0-9a-f]{64}$",
                    "type": "string"
                  },
                  "libc_identity": {
                    "properties": {
                      "provider_path": {
                        "minLength": 1,
                        "type": "string"
                      },
                      "sha256": {
                        "pattern": "^[0-9a-f]{64}$",
                        "type": "string"
                      },
                      "version": {
                        "minLength": 4,
                        "type": "string"
                      }
                    },
                    "required": [
                      "provider_path",
                      "sha256",
                      "version"
                    ]
                  },
                  "libc_version": {
                    "minLength": 4,
                    "type": "string"
                  },
                  "posix_semaphores": {
                    "maxItems": 5,
                    "minItems": 5
                  },
                  "reservation_umask": {
                    "const": 63
                  },
                  "selected_attempt": {
                    "maximum": 31,
                    "minimum": 0,
                    "type": "integer"
                  },
                  "semaphore_name_mapping_verified": {
                    "const": true
                  },
                  "sysv_shm": {
                    "maxItems": 5,
                    "minItems": 5
                  }
                },
                "required": [
                  "exclusive_reservation_complete",
                  "journal_fsynced",
                  "libc_version",
                  "semaphore_name_mapping_verified",
                  "reservation_umask",
                  "sysv_shm",
                  "posix_semaphores",
                  "journal_artifact_name",
                  "journal_closed",
                  "journal_event_count",
                  "journal_sha256",
                  "libc_identity",
                  "candidate_keys",
                  "d2_process_key",
                  "selected_attempt"
                ]
              }
            },
            "required": [
              "artifacts",
              "failure",
              "assertions",
              "cleanup",
              "inventories",
              "reservations",
              "execution_identity",
              "finalization_mode"
            ]
          }
        },
        {
          "if": {
            "properties": {
              "record_finalized": {
                "const": true
              }
            },
            "required": [
              "record_finalized"
            ]
          },
          "then": {
            "properties": {
              "artifacts": {
                "contains": {
                  "properties": {
                    "name": {
                      "const": "supervisor-termination.json"
                    }
                  },
                  "required": [
                    "name"
                  ],
                  "type": "object"
                },
                "maxContains": 1,
                "minContains": 1
              },
              "finalization_mode": {
                "enum": [
                  "normal_cleanup",
                  "abnormal_evidence_only"
                ]
              }
            },
            "required": [
              "finalization_mode",
              "artifacts"
            ]
          }
        },
        {
          "if": {
            "properties": {
              "finalization_mode": {
                "const": "abnormal_evidence_only"
              }
            },
            "required": [
              "finalization_mode"
            ]
          },
          "then": {
            "properties": {
              "cleanup": {
                "properties": {
                  "candidate_resources_removed": {
                    "enum": [
                      false,
                      null
                    ]
                  },
                  "complete": {
                    "const": false
                  },
                  "final_inventory_complete": {
                    "enum": [
                      false,
                      null
                    ]
                  },
                  "final_inventory_matches_pre": {
                    "enum": [
                      false,
                      null
                    ]
                  },
                  "finalizer_complete": {
                    "const": true
                  },
                  "gateway_joined": {
                    "enum": [
                      false,
                      null
                    ]
                  },
                  "output_worker_joined": {
                    "enum": [
                      false,
                      null
                    ]
                  },
                  "process_destroyed": {
                    "enum": [
                      false,
                      null
                    ]
                  }
                },
                "required": [
                  "complete",
                  "candidate_resources_removed",
                  "finalizer_complete",
                  "gateway_joined",
                  "output_worker_joined",
                  "process_destroyed",
                  "final_inventory_complete",
                  "final_inventory_matches_pre"
                ],
                "type": "object"
              },
              "failure": {
                "properties": {
                  "cleanup_outcome": {
                    "enum": [
                      "not_started",
                      "partial",
                      "complete",
                      "unknown"
                    ]
                  }
                },
                "required": [
                  "cleanup_outcome",
                  "missing_artifacts",
                  "unavailable_artifacts"
                ],
                "type": "object"
              },
              "inventories": {
                "properties": {
                  "final": {
                    "properties": {
                      "available": {
                        "const": false
                      },
                      "complete": {
                        "enum": [
                          false,
                          null
                        ]
                      },
                      "is_empty": {
                        "type": "null"
                      },
                      "sha256": {
                        "type": "null"
                      }
                    },
                    "required": [
                      "available",
                      "complete",
                      "is_empty",
                      "sha256"
                    ],
                    "type": "object"
                  },
                  "post": {
                    "properties": {
                      "available": {
                        "const": false
                      },
                      "complete": {
                        "enum": [
                          false,
                          null
                        ]
                      },
                      "is_empty": {
                        "type": "null"
                      },
                      "sha256": {
                        "type": "null"
                      }
                    },
                    "required": [
                      "available",
                      "complete",
                      "is_empty",
                      "sha256"
                    ],
                    "type": "object"
                  }
                },
                "required": [
                  "post",
                  "final"
                ],
                "type": "object"
              },
              "record_finalized": {
                "const": true
              },
              "status": {
                "const": "INCONCLUSIVE"
              }
            },
            "required": [
              "record_finalized",
              "status",
              "failure",
              "cleanup",
              "inventories",
              "artifacts"
            ]
          }
        }
      ],
      "properties": {
        "artifacts": {
          "items": {
            "additionalProperties": false,
            "properties": {
              "bytes": {
                "maximum": 4194304,
                "minimum": 0,
                "type": "integer"
              },
              "name": {
                "enum": [
                  "stdout.txt",
                  "stderr.txt",
                  "ipc-pre.json",
                  "ipc-post.json",
                  "ipc-final.json",
                  "observer-events.json",
                  "reservation-journal.jsonl",
                  "harness-diagnostics.txt",
                  "supervisor-termination.json"
                ]
              },
              "sha256": {
                "pattern": "^[0-9a-f]{64}$",
                "type": "string"
              }
            },
            "required": [
              "name",
              "bytes",
              "sha256"
            ],
            "type": "object"
          },
          "maxItems": 9,
          "type": "array",
          "uniqueItems": true
        },
        "assertions": {
          "additionalProperties": false,
          "properties": {
            "bootstrap_queue_drained": {
              "$ref": "#/$defs/assertion_status"
            },
            "candidate_keys_absent_final": {
              "$ref": "#/$defs/assertion_status"
            },
            "candidate_keys_absent_pre": {
              "$ref": "#/$defs/assertion_status"
            },
            "candidate_resources_attributable": {
              "$ref": "#/$defs/assertion_status"
            },
            "candidate_resources_removed": {
              "$ref": "#/$defs/assertion_status"
            },
            "candidate_resources_reserved": {
              "$ref": "#/$defs/assertion_status"
            },
            "evidence_complete": {
              "$ref": "#/$defs/assertion_status"
            },
            "final_inventory_complete": {
              "$ref": "#/$defs/assertion_status"
            },
            "final_inventory_matches_pre": {
              "$ref": "#/$defs/assertion_status"
            },
            "finalization_complete": {
              "$ref": "#/$defs/assertion_status"
            },
            "first_target_wait_fresh": {
              "$ref": "#/$defs/assertion_status"
            },
            "gateway_joined": {
              "$ref": "#/$defs/assertion_status"
            },
            "host_inventory_preflight": {
              "$ref": "#/$defs/assertion_status"
            },
            "output_worker_joined": {
              "$ref": "#/$defs/assertion_status"
            },
            "post_inventory_complete": {
              "$ref": "#/$defs/assertion_status"
            },
            "pre_inventory_complete": {
              "$ref": "#/$defs/assertion_status"
            },
            "process_blocks_verified": {
              "$ref": "#/$defs/assertion_status"
            },
            "q0_empty_observation_delta": {
              "$ref": "#/$defs/assertion_status"
            },
            "q0_zero_controlled_offers": {
              "$ref": "#/$defs/assertion_status"
            },
            "scalar_run_identity_valid": {
              "$ref": "#/$defs/assertion_status"
            },
            "target_admitted": {
              "$ref": "#/$defs/assertion_status"
            },
            "target_delete_false_before_gateway": {
              "$ref": "#/$defs/assertion_status"
            },
            "target_erased_once": {
              "$ref": "#/$defs/assertion_status"
            },
            "target_future_at_end": {
              "$ref": "#/$defs/assertion_status"
            },
            "target_is_head": {
              "$ref": "#/$defs/assertion_status"
            },
            "target_not_popped": {
              "$ref": "#/$defs/assertion_status"
            },
            "target_not_run": {
              "$ref": "#/$defs/assertion_status"
            },
            "wait_decision_correlated": {
              "$ref": "#/$defs/assertion_status"
            }
          },
          "required": [
            "host_inventory_preflight",
            "candidate_keys_absent_pre",
            "process_blocks_verified",
            "target_delete_false_before_gateway",
            "bootstrap_queue_drained",
            "q0_zero_controlled_offers",
            "q0_empty_observation_delta",
            "target_admitted",
            "target_is_head",
            "first_target_wait_fresh",
            "target_not_popped",
            "target_not_run",
            "target_future_at_end",
            "gateway_joined",
            "output_worker_joined",
            "target_erased_once",
            "final_inventory_matches_pre",
            "evidence_complete",
            "pre_inventory_complete",
            "post_inventory_complete",
            "candidate_resources_attributable",
            "candidate_resources_removed",
            "final_inventory_complete",
            "finalization_complete",
            "wait_decision_correlated",
            "scalar_run_identity_valid",
            "candidate_resources_reserved",
            "candidate_keys_absent_final"
          ],
          "type": "object"
        },
        "baseline_commit": {
          "const": "1af604dad66148618cc0e578c28adb4a9829e607"
        },
        "baseline_tree": {
          "const": "5a3ebb9fbdf61185f05b18e58b8799ced40eb60b"
        },
        "cleanup": {
          "additionalProperties": false,
          "properties": {
            "candidate_resources_removed": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "candidate_resources_reserved": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "complete": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "final_inventory_complete": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "final_inventory_matches_pre": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "finalizer_complete": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "gateway_joined": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "gateway_stop_requested": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "host_inventory_preflight": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "output_worker_joined": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "post_candidate_resources_attributable": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "process_destroyed": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "target_deleted_once": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "target_detached": {
              "type": [
                "boolean",
                "null"
              ]
            }
          },
          "required": [
            "gateway_stop_requested",
            "gateway_joined",
            "output_worker_joined",
            "target_detached",
            "process_destroyed",
            "target_deleted_once",
            "complete",
            "post_candidate_resources_attributable",
            "candidate_resources_removed",
            "final_inventory_complete",
            "finalizer_complete",
            "host_inventory_preflight",
            "candidate_resources_reserved",
            "final_inventory_matches_pre"
          ],
          "type": "object"
        },
        "configured_targets": {
          "const": [
            "Common",
            "ng039_gw_q0_q1_d2"
          ]
        },
        "document_head": {
          "const": "756f493b7049434ab67cf2382486a48f606a470c"
        },
        "execution_identity": {
          "additionalProperties": false,
          "properties": {
            "dedicated_uid_verified": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "gid": {
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            },
            "interactive_login_disabled": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "launcher_device": {
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            },
            "launcher_inode": {
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            },
            "launcher_path": {
              "maxLength": 1024,
              "type": [
                "string",
                "null"
              ]
            },
            "launcher_sha256": {
              "pattern": "^[0-9a-f]{64}$",
              "type": [
                "string",
                "null"
              ]
            },
            "launcher_verified": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "lease_acquired": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "lease_device": {
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            },
            "lease_held_through_finalizer": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "lease_inode": {
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            },
            "lease_path": {
              "maxLength": 1024,
              "type": [
                "string",
                "null"
              ]
            },
            "no_concurrent_uid_process": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "process_tree_pids": {
              "items": {
                "minimum": 2,
                "type": "integer"
              },
              "maxItems": 32,
              "type": [
                "array",
                "null"
              ],
              "uniqueItems": true
            },
            "uid": {
              "minimum": 1,
              "type": [
                "integer",
                "null"
              ]
            }
          },
          "required": [
            "dedicated_uid_verified",
            "uid",
            "gid",
            "interactive_login_disabled",
            "launcher_verified",
            "launcher_path",
            "launcher_sha256",
            "launcher_device",
            "launcher_inode",
            "lease_path",
            "lease_device",
            "lease_inode",
            "lease_acquired",
            "lease_held_through_finalizer",
            "no_concurrent_uid_process",
            "process_tree_pids"
          ],
          "type": "object"
        },
        "exit": {
          "additionalProperties": false,
          "properties": {
            "harness_code": {
              "type": [
                "integer",
                "null"
              ]
            },
            "outer_code": {
              "type": [
                "integer",
                "null"
              ]
            },
            "signal": {
              "maxLength": 64,
              "type": [
                "string",
                "null"
              ]
            },
            "timeout_fired": {
              "type": "boolean"
            }
          },
          "required": [
            "outer_code",
            "harness_code",
            "signal",
            "timeout_fired"
          ],
          "type": "object"
        },
        "failure": {
          "oneOf": [
            {
              "type": "null"
            },
            {
              "additionalProperties": false,
              "properties": {
                "cleanup_outcome": {
                  "enum": [
                    "complete",
                    "partial",
                    "not_started",
                    "unknown"
                  ]
                },
                "code": {
                  "enum": [
                    "ASSERTION_FAILED",
                    "HOST_INVENTORY_FAILED",
                    "PRE_INVENTORY_FAILED",
                    "PARTIAL_CONSTRUCTION",
                    "IDENTITY_MISMATCH",
                    "COUNTER_OVERFLOW",
                    "TIMEOUT",
                    "SIGNAL",
                    "JOIN_BLOCKED",
                    "OWNERSHIP_UNCERTAIN",
                    "INVENTORY_INCOMPLETE",
                    "EVIDENCE_MISSING",
                    "EVIDENCE_INVALID",
                    "RESOURCE_LIMIT",
                    "OTHER",
                    "CANDIDATE_IDENTITY_UNCERTAIN",
                    "CANDIDATE_CLEANUP_FAILED",
                    "FINALIZER_FAILED"
                  ]
                },
                "inventory_availability": {
                  "additionalProperties": false,
                  "properties": {
                    "final": {
                      "type": "boolean"
                    },
                    "post": {
                      "type": "boolean"
                    },
                    "pre": {
                      "type": "boolean"
                    }
                  },
                  "required": [
                    "pre",
                    "post",
                    "final"
                  ],
                  "type": "object"
                },
                "last_event_sequence": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "message": {
                  "maxLength": 1024,
                  "type": "string"
                },
                "missing_artifacts": {
                  "items": {
                    "enum": [
                      "stdout.txt",
                      "stderr.txt",
                      "ipc-pre.json",
                      "ipc-post.json",
                      "ipc-final.json",
                      "observer-events.json",
                      "reservation-journal.jsonl",
                      "supervisor-termination.json"
                    ]
                  },
                  "maxItems": 8,
                  "type": "array",
                  "uniqueItems": true
                },
                "phase": {
                  "enum": [
                    "preflight",
                    "reservation",
                    "pre_inventory",
                    "construction",
                    "q0",
                    "q1",
                    "shutdown",
                    "post_inventory",
                    "cleanup",
                    "final_inventory",
                    "capture",
                    "validation",
                    "unknown",
                    "finalize"
                  ]
                },
                "unavailable_artifacts": {
                  "items": {
                    "enum": [
                      "stdout.txt",
                      "stderr.txt",
                      "ipc-pre.json",
                      "ipc-post.json",
                      "ipc-final.json",
                      "observer-events.json",
                      "reservation-journal.jsonl",
                      "supervisor-termination.json"
                    ]
                  },
                  "maxItems": 8,
                  "type": "array",
                  "uniqueItems": true
                }
              },
              "required": [
                "code",
                "phase",
                "message",
                "last_event_sequence",
                "inventory_availability",
                "cleanup_outcome",
                "missing_artifacts",
                "unavailable_artifacts"
              ],
              "type": "object"
            }
          ]
        },
        "finalization_mode": {
          "enum": [
            "normal_cleanup",
            "abnormal_evidence_only",
            null
          ],
          "type": [
            "string",
            "null"
          ]
        },
        "inventories": {
          "additionalProperties": false,
          "properties": {
            "final": {
              "additionalProperties": false,
              "properties": {
                "available": {
                  "type": "boolean"
                },
                "complete": {
                  "type": [
                    "boolean",
                    "null"
                  ]
                },
                "is_empty": {
                  "type": [
                    "boolean",
                    "null"
                  ]
                },
                "reason": {
                  "maxLength": 512,
                  "type": [
                    "string",
                    "null"
                  ]
                },
                "sha256": {
                  "pattern": "^[0-9a-f]{64}$",
                  "type": [
                    "string",
                    "null"
                  ]
                }
              },
              "required": [
                "available",
                "is_empty",
                "sha256",
                "reason",
                "complete"
              ],
              "type": "object"
            },
            "post": {
              "additionalProperties": false,
              "properties": {
                "available": {
                  "type": "boolean"
                },
                "complete": {
                  "type": [
                    "boolean",
                    "null"
                  ]
                },
                "is_empty": {
                  "type": [
                    "boolean",
                    "null"
                  ]
                },
                "reason": {
                  "maxLength": 512,
                  "type": [
                    "string",
                    "null"
                  ]
                },
                "sha256": {
                  "pattern": "^[0-9a-f]{64}$",
                  "type": [
                    "string",
                    "null"
                  ]
                }
              },
              "required": [
                "available",
                "is_empty",
                "sha256",
                "reason",
                "complete"
              ],
              "type": "object"
            },
            "pre": {
              "additionalProperties": false,
              "properties": {
                "available": {
                  "type": "boolean"
                },
                "complete": {
                  "type": [
                    "boolean",
                    "null"
                  ]
                },
                "is_empty": {
                  "type": [
                    "boolean",
                    "null"
                  ]
                },
                "reason": {
                  "maxLength": 512,
                  "type": [
                    "string",
                    "null"
                  ]
                },
                "sha256": {
                  "pattern": "^[0-9a-f]{64}$",
                  "type": [
                    "string",
                    "null"
                  ]
                }
              },
              "required": [
                "available",
                "is_empty",
                "sha256",
                "reason",
                "complete"
              ],
              "type": "object"
            }
          },
          "required": [
            "pre",
            "post",
            "final"
          ],
          "type": "object"
        },
        "observations": {
          "additionalProperties": false,
          "properties": {
            "counter_overflow": {
              "type": "boolean"
            },
            "q0": {
              "additionalProperties": false,
              "properties": {
                "controlled_application_offers": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "empty_observation_delta": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "end_sequence": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "start_sequence": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                }
              },
              "required": [
                "start_sequence",
                "end_sequence",
                "controlled_application_offers",
                "empty_observation_delta"
              ],
              "type": "object"
            },
            "q1": {
              "additionalProperties": false,
              "properties": {
                "actual_due_branch": {
                  "type": [
                    "boolean",
                    "null"
                  ]
                },
                "actual_popped": {
                  "type": [
                    "boolean",
                    "null"
                  ]
                },
                "correlation_status": {
                  "enum": [
                    "PENDING",
                    "MATCHED",
                    "DECISION_NOT_REACHED",
                    "HEAD_MISMATCH",
                    "INVALID",
                    "OVERFLOW",
                    null
                  ]
                },
                "decision_candidate_id": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "decision_candidate_time": {
                  "type": [
                    "number",
                    "null"
                  ]
                },
                "decision_iteration_id": {
                  "maximum": 18446744073709551615,
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "decision_sequence": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "end_sequence": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "first_target_wait_sequence": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "gateway_iteration_id": {
                  "maximum": 18446744073709551615,
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "head_time_at_end": {
                  "type": [
                    "number",
                    "null"
                  ]
                },
                "process_time_at_end": {
                  "type": [
                    "number",
                    "null"
                  ]
                },
                "start_sequence": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "target_message_id": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "target_pop_count": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "target_run_entry_count": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "target_run_return_count": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "wait_depth": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "wait_head_id": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "wait_head_time": {
                  "type": [
                    "number",
                    "null"
                  ]
                },
                "wait_iteration_id": {
                  "maximum": 18446744073709551615,
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "wait_predicate_return": {
                  "type": [
                    "boolean",
                    "null"
                  ]
                },
                "wait_sequence": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "wait_target_id": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "wait_timeout_ms": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                }
              },
              "required": [
                "start_sequence",
                "end_sequence",
                "target_message_id",
                "first_target_wait_sequence",
                "target_pop_count",
                "target_run_entry_count",
                "target_run_return_count",
                "head_time_at_end",
                "process_time_at_end",
                "gateway_iteration_id",
                "wait_sequence",
                "wait_iteration_id",
                "wait_target_id",
                "wait_head_id",
                "wait_head_time",
                "wait_depth",
                "wait_timeout_ms",
                "wait_predicate_return",
                "decision_sequence",
                "decision_iteration_id",
                "decision_candidate_id",
                "decision_candidate_time",
                "actual_due_branch",
                "actual_popped",
                "correlation_status"
              ],
              "type": "object"
            },
            "target_message_id": {
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            }
          },
          "required": [
            "target_message_id",
            "q0",
            "q1",
            "counter_overflow"
          ],
          "type": "object"
        },
        "process_identity": {
          "additionalProperties": false,
          "properties": {
            "ipc_key": {
              "const": 11
            },
            "name": {
              "const": "NG039TestProcess"
            }
          },
          "required": [
            "name",
            "ipc_key"
          ],
          "type": "object"
        },
        "record_finalized": {
          "type": "boolean"
        },
        "reservations": {
          "additionalProperties": false,
          "properties": {
            "candidate_keys": {
              "items": {
                "maximum": 2147483647,
                "minimum": 1,
                "type": "integer"
              },
              "maxItems": 5,
              "minItems": 0,
              "type": "array",
              "uniqueItems": true
            },
            "d2_process_key": {
              "maximum": 2147483643,
              "minimum": 1,
              "type": [
                "integer",
                "null"
              ]
            },
            "exclusive_reservation_complete": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "harness_pid": {
              "minimum": 2,
              "type": "integer"
            },
            "journal_artifact_name": {
              "const": "reservation-journal.jsonl"
            },
            "journal_closed": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "journal_event_count": {
              "maximum": 128,
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            },
            "journal_fsynced": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "journal_sha256": {
              "pattern": "^[0-9a-f]{64}$",
              "type": [
                "string",
                "null"
              ]
            },
            "libc_identity": {
              "additionalProperties": false,
              "properties": {
                "build_id": {
                  "maxLength": 256,
                  "type": [
                    "string",
                    "null"
                  ]
                },
                "device": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "inode": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "provider_path": {
                  "maxLength": 1024,
                  "type": [
                    "string",
                    "null"
                  ]
                },
                "sha256": {
                  "pattern": "^[0-9a-f]{64}$",
                  "type": [
                    "string",
                    "null"
                  ]
                },
                "version": {
                  "maxLength": 64,
                  "type": [
                    "string",
                    "null"
                  ]
                }
              },
              "required": [
                "provider_path",
                "device",
                "inode",
                "build_id",
                "sha256",
                "version"
              ],
              "type": "object"
            },
            "libc_version": {
              "maxLength": 64,
              "minLength": 4,
              "type": [
                "string",
                "null"
              ]
            },
            "posix_semaphores": {
              "items": {
                "additionalProperties": false,
                "properties": {
                  "created_exclusively": {
                    "const": true
                  },
                  "device": {
                    "minimum": 0,
                    "type": "integer"
                  },
                  "inode": {
                    "minimum": 1,
                    "type": "integer"
                  },
                  "key": {
                    "minimum": 1,
                    "type": "integer"
                  },
                  "map_device_major": {
                    "minimum": 0,
                    "type": [
                      "integer",
                      "null"
                    ]
                  },
                  "map_device_minor": {
                    "minimum": 0,
                    "type": [
                      "integer",
                      "null"
                    ]
                  },
                  "map_end": {
                    "minimum": 0,
                    "type": [
                      "integer",
                      "null"
                    ]
                  },
                  "map_inode": {
                    "minimum": 0,
                    "type": [
                      "integer",
                      "null"
                    ]
                  },
                  "map_start": {
                    "minimum": 0,
                    "type": [
                      "integer",
                      "null"
                    ]
                  },
                  "mapping_line": {
                    "maxLength": 4096,
                    "type": [
                      "string",
                      "null"
                    ]
                  },
                  "mode": {
                    "const": 384,
                    "type": "integer"
                  },
                  "name": {
                    "pattern": "^[0-9]+$",
                    "type": "string"
                  },
                  "path": {
                    "pattern": "^/dev/shm/sem\\.[0-9]+$",
                    "type": "string"
                  },
                  "sem_t_address": {
                    "minimum": 0,
                    "type": [
                      "integer",
                      "null"
                    ]
                  },
                  "uid": {
                    "minimum": 0,
                    "type": "integer"
                  }
                },
                "required": [
                  "created_exclusively",
                  "device",
                  "inode",
                  "key",
                  "mode",
                  "name",
                  "path",
                  "uid",
                  "sem_t_address",
                  "map_start",
                  "map_end",
                  "map_device_major",
                  "map_device_minor",
                  "map_inode",
                  "mapping_line"
                ],
                "type": "object"
              },
              "maxItems": 5,
              "type": "array"
            },
            "reservation_umask": {
              "enum": [
                63,
                null
              ],
              "type": [
                "integer",
                "null"
              ]
            },
            "run_id": {
              "maxLength": 64,
              "minLength": 16,
              "type": "string"
            },
            "selected_attempt": {
              "maximum": 31,
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            },
            "semaphore_name_mapping_verified": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "sysv_shm": {
              "items": {
                "additionalProperties": false,
                "properties": {
                  "created_exclusively": {
                    "const": true
                  },
                  "creator_pid": {
                    "minimum": 2,
                    "type": "integer"
                  },
                  "creator_uid": {
                    "minimum": 0,
                    "type": "integer"
                  },
                  "id": {
                    "minimum": 0,
                    "type": "integer"
                  },
                  "key": {
                    "minimum": 1,
                    "type": "integer"
                  },
                  "mode": {
                    "const": 384,
                    "type": "integer"
                  },
                  "owner_uid": {
                    "minimum": 0,
                    "type": "integer"
                  },
                  "size_bytes": {
                    "maximum": 33554532,
                    "minimum": 1,
                    "type": "integer"
                  }
                },
                "required": [
                  "created_exclusively",
                  "creator_pid",
                  "creator_uid",
                  "id",
                  "key",
                  "mode",
                  "owner_uid",
                  "size_bytes"
                ],
                "type": "object"
              },
              "maxItems": 5,
              "type": "array"
            }
          },
          "required": [
            "run_id",
            "harness_pid",
            "d2_process_key",
            "selected_attempt",
            "candidate_keys",
            "exclusive_reservation_complete",
            "journal_fsynced",
            "libc_version",
            "semaphore_name_mapping_verified",
            "reservation_umask",
            "sysv_shm",
            "posix_semaphores",
            "journal_artifact_name",
            "journal_event_count",
            "journal_closed",
            "journal_sha256",
            "libc_identity"
          ],
          "type": "object"
        },
        "run_id": {
          "maxLength": 64,
          "minLength": 16,
          "pattern": "^NG039-D2-[A-Za-z0-9][A-Za-z0-9._-]{7,47}$",
          "type": "string"
        },
        "schema_version": {
          "const": "ng039-d2-run-record/v5"
        },
        "spec_sha256": {
          "const": "0cda2743e797cfbed952a1bad7c30e8e0f15d37c57e51d70e3388b9453103115"
        },
        "status": {
          "enum": [
            "PASS",
            "FAIL",
            "INCONCLUSIVE"
          ]
        },
        "task_id": {
          "const": "NG-039"
        }
      },
      "required": [
        "schema_version",
        "task_id",
        "run_id",
        "spec_sha256",
        "baseline_commit",
        "baseline_tree",
        "document_head",
        "configured_targets",
        "process_identity",
        "status",
        "exit",
        "assertions",
        "observations",
        "inventories",
        "cleanup",
        "failure",
        "artifacts",
        "record_finalized",
        "execution_identity",
        "finalization_mode"
      ],
      "title": "SPEC-039 D2 v5 normal/abnormal final run record",
      "type": "object"
    },
    "supervisor_termination": {
      "$id": "urn:ng039:supervisor-termination:v1",
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "additionalProperties": false,
      "properties": {
        "captured_monotonic_ns": {
          "minimum": 0,
          "type": "integer"
        },
        "launcher_sha256": {
          "pattern": "^[0-9a-f]{64}$",
          "type": "string"
        },
        "lease_device": {
          "minimum": 0,
          "type": "integer"
        },
        "lease_inode": {
          "minimum": 1,
          "type": "integer"
        },
        "lease_path": {
          "maxLength": 1024,
          "minLength": 1,
          "type": "string"
        },
        "process_tree_pids": {
          "items": {
            "minimum": 2,
            "type": "integer"
          },
          "maxItems": 32,
          "minItems": 1,
          "type": "array",
          "uniqueItems": true
        },
        "run_id": {
          "maxLength": 64,
          "minLength": 16,
          "type": "string"
        },
        "schema_version": {
          "const": "ng039-supervisor-termination/v1"
        },
        "stderr_closed": {
          "type": "boolean"
        },
        "stderr_sha256": {
          "pattern": "^[0-9a-f]{64}$",
          "type": [
            "string",
            "null"
          ]
        },
        "stdout_closed": {
          "type": "boolean"
        },
        "stdout_sha256": {
          "pattern": "^[0-9a-f]{64}$",
          "type": [
            "string",
            "null"
          ]
        },
        "timeout_fired": {
          "type": "boolean"
        },
        "wait_receipts": {
          "items": {
            "additionalProperties": false,
            "properties": {
              "pid": {
                "minimum": 2,
                "type": "integer"
              },
              "raw_wait_status": {
                "maximum": 65535,
                "minimum": 0,
                "type": "integer"
              }
            },
            "required": [
              "pid",
              "raw_wait_status"
            ],
            "type": "object"
          },
          "maxItems": 32,
          "minItems": 1,
          "type": "array",
          "uniqueItems": true
        }
      },
      "required": [
        "schema_version",
        "run_id",
        "launcher_sha256",
        "lease_path",
        "lease_device",
        "lease_inode",
        "process_tree_pids",
        "wait_receipts",
        "stdout_closed",
        "stderr_closed",
        "stdout_sha256",
        "stderr_sha256",
        "timeout_fired",
        "captured_monotonic_ns"
      ],
      "title": "SPEC-039 trusted supervisor termination receipt v1",
      "type": "object"
    }
  },
  "fixture": {
    "bootstrap_deadline_s": 5,
    "expected_block_count": 2,
    "future_horizon_s": 10,
    "harness_must_not_call_newblock_for_ht_or_gw": true,
    "key": 11,
    "process_constructor_creates_blocks_in_order": [
      "HT",
      "GW"
    ],
    "process_name": "NG039TestProcess",
    "q0_application_offers": 0,
    "q0_empty_observation_delta_required": true,
    "q0_window_ms": 100,
    "q1_window_ms": 100,
    "target_message": {
      "action_match": "none; neither exact registered action nor 0.1 fallback",
      "admit_after_q0_via": "GW::PushToInputQueue",
      "command_lines": [
        "-ng039 --probe 0.2",
        "-ng039 --probe 0.2",
        "-ng039 --probe 0.2"
      ],
      "creation": "Process::NewMessage before Gateway thread starts",
      "has_payload": false,
      "initial_delete_flag": false,
      "owner": "Process",
      "time_expression": "Process::GetTime()+10.0",
      "type": 0
    }
  },
  "local_path_audit": {
    "d1h_rebuild_root": "${WORKSPACE_ROOT}/ng039-d1h-rebuild-20261010",
    "d1h_rebuild_root_outside_hermes": true,
    "d2_aliases_must_be_resolved_and_pairwise_checked_before_any_execution": true,
    "d2_build_directories_created": false,
    "d2_execution_leaf_must_not_be_created_before_positive_review_and_separate_owner_gate": true,
    "d2_harness_or_ipc_executed": false,
    "d2_worktree_created": true,
    "r07_manifest_created_before_packet_freeze": true,
    "r07_outputs_were_not_present_at_pre-freeze_source_snapshot": false,
    "r07_packet_created_after_manifest_freeze": true,
    "r08_existing_draft_files_refreshed_in_place": true,
    "r08_manifest_created_before_packet_freeze": true,
    "r08_outputs_not_present_at_pre_freeze_source_snapshot": false,
    "r08_packet_created_after_manifest_freeze": true,
    "review_directory": "${NG_REPO_ROOT}/Specs/RESULTS-SPEC-039",
    "review_directory_is_symlink": false
  },
  "local_validation_this_freeze": {
    "abnormal_partial_evidence_fixture": "PASS: INCONCLUSIVE record accepted with present raw post/final artifacts and a separate unavailable-artifact name; no non-control parsing required",
    "astra_submitted": false,
    "cleanup_history_outcome_fixtures": "PASS: a bounded offline classifier accepted only schema-valid, ordered, complete journal streams; not_started requires no-intent/no-CREATED proof; failed intent=partial; complete requires exactly one REMOVED outcome for every successful CREATED resource; empty/unknown/malformed/reordered/truncated or created-without-any-cleanup streams=unknown",
    "d2_d3_measurement": false,
    "harness_execution": false,
    "interpretation": "Documentary contract only. Continuous lease and stable result-directory authority remain required for either finalization mode; if they fail, preserve pending evidence. Launcher is NOT_PROVISIONED; D2 blocked until separate gates.",
    "ipc_or_vm_activity": false,
    "json_schema_meta_validation": "PASS: five Draft 2020-12 JSON Schemas meta-validated; collection_responsibility is manifest policy data, not a schema",
    "lease_loss_policy_static_check": "PASS: the manifest contract says common prerequisite failure blocks finalizer/re-entry and leaves pending evidence; no lease-loss runtime/finalizer fixture was executed",
    "manifest_generation": "PASS: r12 generated from immutable submitted r11 manifest; r11 packet/response/raw/receipt preserved",
    "missing_unavailable_set_fixtures": "PASS: exact absent set, exact unavailable set and disjointness checked; optional diagnostics rejected from missing_artifacts",
    "owner_exact_sha_consent_for_r12": false,
    "runtime_finalizer_execution": false,
    "spec_sha256": "0cda2743e797cfbed952a1bad7c30e8e0f15d37c57e51d70e3388b9453103115",
    "supervisor_pid_set_cross_artifact_validation": "NOT EXECUTED: semantic finalizer absent; not claimed by schema tests"
  },
  "observer": {
    "correlation_rule": [
      "wait and decision iteration_id equal",
      "decision.sequence greater than wait.sequence",
      "wait target_id and head_id equal decision candidate_id",
      "only then correlation_status=MATCHED; otherwise preserve PENDING/DECISION_NOT_REACHED/HEAD_MISMATCH/INVALID/OVERFLOW"
    ],
    "decision_capture_fields": [
      "sequence",
      "iteration_id",
      "candidate_id",
      "candidate_time",
      "actual_due_branch",
      "actual_popped"
    ],
    "event_sequence_bits": 64,
    "gateway_iteration_id_one_per_loop": true,
    "gateway_started_one_way": true,
    "iteration_id_bits": 64,
    "iteration_id_saturates": true,
    "run_identity_capture": "Copy Message::InstantiationNumber to a scalar immediately before each production Run; use the copy for RunEntry/RunReturn before DeleteMarkedMessages; never dereference a Message after reclamation.",
    "sequence_and_counters": "saturate; set overflow flag; fail inconclusive",
    "sticky_counts": [
      "admission",
      "non_due",
      "pop",
      "Run_entry",
      "Run_return",
      "empty_queue"
    ],
    "sticky_decision_capture": true,
    "sticky_first_target_wait_record": true,
    "stores_message_pointer": false,
    "target_wait_fields": [
      "sequence",
      "iteration_id",
      "target_id",
      "head_id",
      "head_time",
      "depth",
      "timeout_ms",
      "wait_return"
    ],
    "update_sites": [
      {
        "event": "target admission after valid NoCL>2 insertion under InputQueueMutex",
        "file": "Common/src/GW.cpp",
        "function": "GW::PushToInputQueue",
        "lines": [
          219,
          267
        ]
      },
      {
        "event": "one-way gateway_started and locked queue-head/empty snapshot",
        "file": "Common/src/GW.cpp",
        "function": "GW::Gateway",
        "lines": [
          451,
          467
        ]
      },
      {
        "event": "saturating gateway_iteration_id increment once at loop-iteration start",
        "file": "Common/src/GW.cpp",
        "function": "GW::Gateway",
        "lines": [
          451,
          452
        ]
      },
      {
        "event": "actual wait_for result; sticky first target wait if target is head",
        "file": "Common/src/GW.cpp",
        "function": "GW::Gateway",
        "lines": [
          470,
          480
        ]
      },
      {
        "event": "strict due decision and actual pop/non-due outcome under the queue mutex",
        "file": "Common/src/GW.cpp",
        "function": "GW::Gateway",
        "lines": [
          482,
          503
        ]
      },
      {
        "event": "scalar target Run entry/return around production Run call before reclamation",
        "file": "Common/src/GW.cpp",
        "function": "GW::Gateway",
        "lines": [
          506,
          597
        ]
      },
      {
        "event": "stop-request observation outside queue-lock recursion",
        "file": "Common/src/GW.cpp",
        "function": "GW::SetStopGatewayFlag",
        "lines": [
          1331,
          1337
        ]
      }
    ]
  },
  "parent_task_id": "NG-039",
  "path_aliases": {
    "D1H_REBUILD_ROOT": "${WORKSPACE_ROOT}/ng039-d1h-rebuild-20261010",
    "D1H_WORKTREE": "isolated historical D1-H source worktree at the immutable functional baseline",
    "D2_RESULTS": "${NG_REPO_ROOT}/Specs/RESULTS-SPEC-039/ng039-d2-v1.10-20261010",
    "D2_SCRATCH": "${WORKSPACE_ROOT}/ng039-d2-scratch-v1.10-1af604d",
    "D2_WORKTREE": "${WORKSPACE_ROOT}/novagenesis-ng039-d2-v1.10-1af604d",
    "NG_REPO_PARENT": "parent of the active AIOPT5 checkout",
    "NG_REPO_ROOT": "active AIOPT5 checkout root",
    "WORKSPACE_ROOT": "workspace root; outside .hermes",
    "absolute_resolution_required_before_execution": true,
    "containment_rule": "Resolve each alias once before any command; reject symlink/overlap and require the worktree, build roots, HOME, TMPDIR and result leaf to remain under their named roots; all are outside .hermes."
  },
  "planned_results": {
    "artifacts_not_generated": true,
    "binary_sha256": null,
    "compile_commands_sha256": null,
    "default_binary_sha256": null,
    "diagnostic_binary_sha256": null,
    "finalizer_exit_code": null,
    "inventory_hashes": null,
    "process_exit_code": null,
    "run_id": null,
    "run_record_sha256": null,
    "status": null
  },
  "prior_review": {
    "derived_response_sha256": "43425d2dff3c639667edf50d01deed5e3e789cad04223ba09a0852276e597852",
    "manifest_sha256": "1a90346598f5799565bf1f39610ccb45f43afdb8c7acba08d82616e747a6a70d",
    "one_call_consent_consumed": true,
    "packet_sha256": "ffc71bf48dd0f2c2bea7a3448923a9e11ba5d3a16ebbea19ff81a9725e8f46af",
    "raw_sse_sha256": "cd069756e08fb194b01eb55777e56777eb13b59471d156da45b8efaa93561cc1",
    "response_id": "resp_00f0490433d54fc0016acb8d33fcd481918764717d05d7e1d7",
    "scope": "B1/B2/B3 documentary closure only; no implementation/runtime authority",
    "spec_sha256": "f7742629a6069810bade1df295277dfcb2c0ff6c47d614236b4b1e29ca630f26",
    "verdict": "APPROVE — documentary closure only"
  },
  "r06_review": {
    "archive_sha256": "7d57b56e73546c15ca0b437439f361223acbb19baf76890d6be381f01ed89209",
    "authority": "advisory textual inspection only; no implementation/configure/build/runtime/IPC/VM/D2/D3 authority",
    "manifest_sha256": "3bf3a4284c163b35c2e08ffcb196b9bd19adb5aed9dd9b5c0f0b501454f360b2",
    "must_fixes": [
      "compile source/output/build-root containment and exactly one -c",
      "freeze a deterministic environment for configure/build/checkers",
      "read and bind the exact link.txt; reject forwarded linker controls",
      "prevent schema-valid PASS/FAIL contradictions using local schema constraints plus a bounded semantic finalizer"
    ],
    "one_call_consent_consumed": true,
    "packet_sha256": "cb3cfa8cee77ba51b74070d49872d4946eac5123ddbca74506572dd3a3acd711",
    "parsed_response_sha256": "9058bc8a6490b1774d0bd0fb8f185dbaa8669c73353faffffd87d8ba2cd75a2e",
    "raw_sse_sha256": "66a73a0ddb4884b591dbe2d3f78aed696da4d1c1a2fb206085e29e1fbc767cc0",
    "response_id": "resp_0e96e100afdf6c5d016aca6a11615c81919e900a168d341cf9",
    "spec_sha256": "2477ac94b67ecf6e81665fe2c8899cbfdff31edec08830dc2b60ebbaa7b0d0d0",
    "verdict": "AMEND"
  },
  "r08_review": {
    "consents_consumed": 2,
    "followup_raw_sse_sha256": "fa3348e5a680811772bd5c472cc24feff55c2b080a09f8b6e50e451475ea084d",
    "followup_response_sha256": "6ff9bd65cab5cadb535aeb428781ede9b17ab357a6cdf6121e358af70630ff7b",
    "followup_verdict": "AMEND; B1 withdrawn as sender-envelope typo; B2 and B3 remain blockers",
    "initial_response_sha256": "32ab51a41285516ec725462e14bd9c9bec11ffc215b113bc588c2cd1c13d5a85",
    "initial_verdict": "AMEND",
    "manifest_sha256": "d0518b500343fe80fef834b4b9a3f819396224e8da9ec601690100749d819301",
    "packet_sha256": "3295a1bffd234673aaa82e2e7e254f3d5bad6c1e747f4d18123f702510445272",
    "scope": "documentary review only; no implementation, configure/build, runtime, IPC, D3, commit or push authority",
    "spec_sha256": "bc735702fff9adde69a00705715e4025cedca5f3e1314eb7200da29712822992"
  },
  "r09_freeze_local_validation": {
    "astra_submitted": false,
    "d2_d3_measurement": false,
    "harness_execution": false,
    "interpretation": "Documentary contract only; dedicated launcher is unprovisioned and D2 remains blocked.",
    "ipc_or_vm_activity": false,
    "json_duplicate_key_check": "PASS: strict source parse",
    "json_schema_meta_validation": "PASS: all five Draft 2020-12 schemas meta-validated with /usr/bin/python3 jsonschema",
    "manifest_generation": "PASS: generated as new r09 from immutable r08 manifest; r08 manifest/packet and review records preserved",
    "owner_exact_sha_consent_for_r09": false,
    "pass_execution_gate_structure": "PASS: PASS/FAIL schema branches require verified dedicated identity/lease; structural check only, no runtime fixture",
    "raw_table_digest_probe": "PASS: exact bytes accepted when SHA matches; deliberately mismatched digest rejected by local contract probe",
    "runtime_finalizer_execution": false,
    "spec_sha256": "a2feea01cb80b60256da3a7abd5e8c2196954d6e008d6f488cd4fb37f2058af1",
    "synthetic_schema_instances": "PASS: one raw inventory and one journal event instance accepted; no finalizer/runtime fixture was executed"
  },
  "r09_review": {
    "manifest_sha256": "613c5843f503ff75f704aef6fd9c7b68fb994029d5f822691223212bfbf7bc9a",
    "one_call_consent_consumed": true,
    "optional_advisories_not_adopted_as_blockers": [
      "require mapping_line rather than an evidence-equivalent structured mapping projection",
      "separate historical fixture counts from newly required negative classes",
      "clarify retained journal bytes do not independently prove historic fsync calls"
    ],
    "packet_sha256": "560ee5654d9a8859e3e21aa916a1dca9f0e564fe1f8eac41fb4ee8f4bd3397a6",
    "raw_sse_path": "Specs/RESULTS-SPEC-039/astra-review-v1.12-r09-20261012.raw.sse",
    "raw_sse_sha256": "93f78263662d085a45fde57ea4377c8420b381c958f556a0a80f6a70a089f80e",
    "response_path": "Specs/RESULTS-SPEC-039/astra-review-v1.12-r09-20261012.md",
    "response_sha256": "4b52a1759768714492d4c2bc5099fc9a3626a334e1a33c65e627f1725494fd01",
    "spec_sha256": "a2feea01cb80b60256da3a7abd5e8c2196954d6e008d6f488cd4fb37f2058af1",
    "submission_receipt_path": "Specs/RESULTS-SPEC-039/astra-review-v1.12-r09-20261012-submission.json",
    "submission_receipt_sha256": "20e62f856c5909c0f3c9bc99d065316b4047ebf8e295bdcb680a840b6139ab6b",
    "verdict": "AMEND — documentary contract only; B1 finalizer IPC boundary is the only blocker; B2/B3 substantially addressed"
  },
  "r10_freeze_local_validation": {
    "astra_submitted": false,
    "d2_d3_measurement": false,
    "finalizer_boundary_consistency": "PASS: allowed operations, prohibited actions, owner/sequence and fail-closed rule present in manifest; textual SPEC contradiction absent",
    "harness_execution": false,
    "interpretation": "Documentary contract only; launcher/lease remains NOT_PROVISIONED, D2 blocked, and exact-SHA consent for r10 not yet requested/granted.",
    "ipc_or_vm_activity": false,
    "json_duplicate_key_check": "PASS: strict source parse",
    "json_schema_meta_validation": "PASS: all five Draft 2020-12 schemas meta-validated with jsonschema",
    "manifest_generation": "PASS: new r10 copied from immutable submitted r09 manifest; r09 packet/manifest/review evidence preserved",
    "owner_exact_sha_consent_for_r10": false,
    "runtime_finalizer_execution": false,
    "spec_sha256": "b4e1c0c2db88a0d1a8f02ab98c934b7b33ec23fd2162c8e1b2921b72afbaf88d",
    "synthetic_schema_instances": "PASS: empty inventory and JOURNAL_OPENED shape fixtures accepted; no finalizer/runtime fixture executed"
  },
  "r10_review": {
    "manifest_sha256": "5707467a0c4e41d254dfe64c30b5d34b74da165bb755f9d8d792f209fda28a13",
    "must_fix": "Separate normal cleanup (durable shutdown event plus wait/log evidence) from abnormal evidence-only finalization (confirmed process termination/log closure, INCONCLUSIVE, no IPC inspection or destructive cleanup).",
    "one_call_consent_consumed": true,
    "packet_sha256": "55141aab788839b0346d7ec1bda46c9e4cf86e9d33bc64ce67e242fcb2ff7146",
    "raw_sse_path": "Specs/RESULTS-SPEC-039/astra-review-v1.13-r10-20261012.raw.sse",
    "raw_sse_sha256": "4c35170e7d37953daf9bbc7082e0e92cd73b6b45067521a151c3e7654d6f2704",
    "response_path": "Specs/RESULTS-SPEC-039/astra-review-v1.13-r10-20261012.md",
    "response_sha256": "6aa0669c3cec1b11c2516d6c361d78460663cde9f097cc4c5de8264557e64c2e",
    "spec_sha256": "b4e1c0c2db88a0d1a8f02ab98c934b7b33ec23fd2162c8e1b2921b72afbaf88d",
    "submission_receipt_path": "Specs/RESULTS-SPEC-039/astra-review-v1.13-r10-20261012-submission.json",
    "submission_receipt_sha256": "046afc37a36e7441ee341d6c663f3718fa0bf8ca76717cc98791f58f95373ae8",
    "verdict": "AMEND — documentary scope only; the r09 IPC boundary is resolved and r08 B2/B3 remain intact; sole new B1 is abnormal finalization conflicting with unconditional normal shutdown prerequisites"
  },
  "r11_freeze_local_validation": {
    "abnormal_mode_negative_probes": "PASS: schema rejected PASS claim, cleanup claim, optional diagnostics listed as missing and empty wait receipt; set-equality probe detected omitted missing evidence; RUN_SHUTDOWN_COMPLETE order checked",
    "abnormal_run_record_positive_instance": "PASS: finalized INCONCLUSIVE record accepted with unavailable inventories and present but unverified ipc-post/ipc-final files; missing_artifacts exactly matched absent evidence names",
    "astra_submitted": false,
    "d2_d3_measurement": false,
    "harness_execution": false,
    "interpretation": "Documentary contract only; launcher/lease is NOT_PROVISIONED; D2 blocked. Fresh exact-SHA consent for r11 not yet requested or granted.",
    "ipc_or_vm_activity": false,
    "json_duplicate_key_check": "PASS: strict JSON parse",
    "json_schema_meta_validation": "PASS: five Draft 2020-12 JSON Schemas meta-validated; collection_responsibility is manifest policy data, not a schema",
    "manifest_generation": "PASS: created r11 from immutable submitted r10 manifest; r10 packet/manifest/response/raw/receipt preserved",
    "owner_exact_sha_consent_for_r11": false,
    "runtime_finalizer_execution": false,
    "spec_sha256": "bbb26a1331d832ab2dcbf37f1d243d7b9239e6ff1d1932319ad1141923baa609",
    "supervisor_pid_set_cross_artifact_validation": "CONTRACT_ONLY_NOT_EXECUTED: requires semantic finalizer, not JSON Schema",
    "supervisor_receipt_positive_instance": "PASS: schema-valid receipt with raw waitpid status accepted"
  },
  "r11_review": {
    "consent_message_id": "1559215860005081219",
    "manifest_sha256": "c7d9598d1827b6ee91ab0e6ad9d15ebeaa04e73ddf181b363eb5f2f04f1e589b",
    "must_fix": "Keep abnormal finalization behind common lease/supervisor/result-directory authority; derive historical cleanup as not_started/partial/complete/unknown from evidence and never retry; scope strict bundle parsing to normal mode.",
    "one_call_consent_consumed": true,
    "packet_sha256": "feb69f164ec879e1b567c44f83e7e56811d2a347fbe904a16337a7a8e3bad829",
    "raw_sse_path": "Specs/RESULTS-SPEC-039/astra-review-v1.14-r11-20261012.raw.sse",
    "raw_sse_sha256": "5c9ffcebbb2bd90535465bc04a54fa13155531c267bf9ab69b69f8e830174acb",
    "response_path": "Specs/RESULTS-SPEC-039/astra-review-v1.14-r11-20261012.md",
    "response_sha256": "9ef48bdfafbfd9818613d9356f9b011972f6efc0af1d2433f7a039c3ba260ed7",
    "spec_sha256": "bbb26a1331d832ab2dcbf37f1d243d7b9239e6ff1d1932319ad1141923baa609",
    "submission_receipt_path": "Specs/RESULTS-SPEC-039/astra-review-v1.14-r11-20261012-submission.json",
    "submission_receipt_sha256": "4fa8e4828ea27ffd94185ff00d3a5d3364bdf152090b4d093569863991c6aafd",
    "verdict": "AMEND — documentary scope only; r10 B1 was resolved, but r11 identified two remaining blockers: lease loss could select abnormal finalization despite a both-modes lease prerequisite; cleanup history after a cleanup intent could be misreported as not_started. Optional advice: scope strict full-bundle parsing/completeness to normal mode."
  },
  "review_artifacts": {
    "manifest_path": "Specs/RESULTS-SPEC-039/precode-manifest-v1.14-r12.json",
    "packet_path": "Specs/RESULTS-SPEC-039/precode-packet-v1.14-amend-closure-20261012-sanitized-draft-r12.md",
    "packet_sha256": "computed after packet freeze; not embedded",
    "prior_r08_followup_response_sha256": "6ff9bd65cab5cadb535aeb428781ede9b17ab357a6cdf6121e358af70630ff7b",
    "prior_r08_packet_sha256": "3295a1bffd234673aaa82e2e7e254f3d5bad6c1e747f4d18123f702510445272",
    "prior_r09_packet_sha256": "560ee5654d9a8859e3e21aa916a1dca9f0e564fe1f8eac41fb4ee8f4bd3397a6",
    "prior_r09_raw_sse_sha256": "93f78263662d085a45fde57ea4377c8420b381c958f556a0a80f6a70a089f80e",
    "prior_r09_response_sha256": "4b52a1759768714492d4c2bc5099fc9a3626a334e1a33c65e627f1725494fd01",
    "prior_r10_packet_sha256": "55141aab788839b0346d7ec1bda46c9e4cf86e9d33bc64ce67e242fcb2ff7146",
    "prior_r10_raw_sse_sha256": "4c35170e7d37953daf9bbc7082e0e92cd73b6b45067521a151c3e7654d6f2704",
    "prior_r10_response_sha256": "6aa0669c3cec1b11c2516d6c361d78460663cde9f097cc4c5de8264557e64c2e",
    "prior_r10_submission_receipt_sha256": "046afc37a36e7441ee341d6c663f3718fa0bf8ca76717cc98791f58f95373ae8",
    "prior_r11_packet_sha256": "feb69f164ec879e1b567c44f83e7e56811d2a347fbe904a16337a7a8e3bad829",
    "prior_r11_raw_sse_sha256": "5c9ffcebbb2bd90535465bc04a54fa13155531c267bf9ab69b69f8e830174acb",
    "prior_r11_response_sha256": "9ef48bdfafbfd9818613d9356f9b011972f6efc0af1d2433f7a039c3ba260ed7",
    "prior_r11_submission_receipt_sha256": "4fa8e4828ea27ffd94185ff00d3a5d3364bdf152090b4d093569863991c6aafd",
    "submission_state": "Unsubmitted r12 documentary working packet; fresh owner consent bound to exact final packet SHA required before one Astra review; no operational authority."
  },
  "review_date": "2026-10-12",
  "review_revision": "r12",
  "review_scope": "Same-SPEC v1.14 documentary response to Astra r11 B1/B2 and evidence-preservation advice. No code, configure/build, harness/finalizer execution, IPC, D3, commit or push. Fresh exact-SHA owner consent is required for one r12 Astra review. D2 remains blocked until dedicated launcher/lease is provisioned and a separate owner execution decision is recorded. Abnormal evidence-only writes a final record only while the trusted launcher, execution/result-directory identities, supervisor proof and continuous lease remain valid; otherwise preserve pending evidence. It records historical cleanup outcome and never retries.",
  "runtime_bundle": {
    "abnormal_evidence_policy": "No complete-bundle requirement or non-control JSON/JSONL schema/replay in abnormal_evidence_only. Preserve/hash safe raw files under per-file/32 MiB aggregate read caps; classify absent required names as missing and present unsafe/unreadable/over-cap required names as unavailable; optional diagnostics is never required or listed missing.",
    "candidate_object_mode_octal": "0600",
    "host_dev_shm_inventory_entries": 4096,
    "host_inventory_unrelated_segment_bytes_cap": 9223372036854775807,
    "host_sysv_inventory_entries_per_table": 4096,
    "max_artifact_bytes": 4194304,
    "max_artifacts_excluding_run_record": 9,
    "max_candidate_sysv_shm_segment_bytes": 33554532,
    "max_candidate_sysv_shm_segments": 5,
    "max_candidate_sysv_shm_total_bytes": 167772660,
    "max_files_total": 10,
    "max_run_record_bytes": 262144,
    "max_total_bytes": 33554432,
    "optional_files": [
      "harness-diagnostics.txt"
    ],
    "proc_sysvipc_raw_tables": {
      "digest": "SHA-256 over exact decoded bytes",
      "encoding": "canonical base64 in each ipc-pre/post/final.json",
      "finalizer": "strictly decode, recompute digest, parse pinned Linux table grammar, recompute normalized rows/IDs and compare; mismatch is INCONCLUSIVE",
      "max_decoded_bytes_per_table": 1048576,
      "tables": [
        "shm",
        "sem",
        "msg"
      ]
    },
    "required_files": [
      "run-record.json",
      "stdout.txt",
      "stderr.txt",
      "ipc-pre.json",
      "ipc-post.json",
      "ipc-final.json",
      "observer-events.json",
      "reservation-journal.jsonl",
      "supervisor-termination.json"
    ],
    "required_libc_identity": "record exact sem_open provider path/device/inode/build-id/hash/version and prove returned sem_t* address-to-/proc/self/maps shared mapping whose device/inode matches exact /dev/shm semaphore object",
    "reservation_journal": {
      "event_order": [
        "JOURNAL_OPENED",
        "CREATE_INTENT",
        "CREATED|CREATE_FAILED",
        "RUN_SHUTDOWN_COMPLETE",
        "CLEANUP_INTENT",
        "REMOVED|CLEANUP_FAILED",
        "JOURNAL_CLOSED"
      ],
      "file_name": "reservation-journal.jsonl",
      "final_bundle_binding": "listed exactly once in run-record artifacts and hashed after JOURNAL_CLOSED; final run-record hash is stored only in outer archive index",
      "format": "strict JSONL, one canonical object per newline-terminated line",
      "fsync": "fsync file after every event and containing directory after exclusive create",
      "max_bytes": 262144,
      "max_events": 128,
      "required": true,
      "schema_id": "urn:ng039:reservation-journal-event:v2",
      "schema_version": "ng039-reservation-journal-event/v2"
    },
    "reservation_umask_octal": "0077",
    "run_record_sha256_location": "outer archive index; not self-referenced",
    "staging_file": {
      "deleted_after_finalization": true,
      "included_in_final_bundle": false,
      "maximum_bytes": 262144,
      "name": "run-record.pending.json"
    },
    "supervisor_termination_receipt": {
      "abnormal_rule": "signaled/timed-out but fully reaped processes may enter abnormal_evidence_only; no cleanup or live IPC inspection",
      "file_name": "supervisor-termination.json",
      "log_binding": "stdout_closed/stderr_closed are true only after corresponding FDs close; non-null log hashes bind to the closed file artifacts",
      "pid_binding": "process_tree_pids and wait_receipts.pid sets must equal execution_identity.process_tree_pids exactly; no duplicate or omitted PID",
      "producer": "trusted dedicated-UID launcher after wait/reap of the harness process tree and closure of stdout/stderr",
      "required": true,
      "schema_version": "ng039-supervisor-termination/v1",
      "wait_status": "retain raw waitpid status integers; finalizer decodes each status itself, never trusts a normalized success boolean"
    },
    "tracked_plan_manifest_is_not_runtime_file": true
  },
  "sandbox": {
    "abnormal_evidence_preservation": "Open only allowlisted no-follow regular files after supervisor proof; hash raw bytes within per-file/aggregate limits. Missing required names and present-but-unavailable required names are disjoint arrays. Unsafe common authority/lease means pending BLOCKED/INCONCLUSIVE, not finalization.",
    "abnormal_mode_live_inventory_capture": false,
    "candidate_key_algorithm": "For attempt n=0..31 compute base=0x40000000 + (uint32_be(SHA256(ASCII(run_id + ':' + decimal(n)))[0:4]) mod 0x3FFFFFF8); select first free contiguous base..base+3, exclude key 11 separately, and record n/base. All values are positive signed 32-bit key_t.",
    "candidate_keys": "11 and D2_PROCESS_KEY through D2_PROCESS_KEY+3",
    "candidate_keys_absent_pre_required": true,
    "candidate_object_mode": "0600 (effective SHM and POSIX semaphore permissions must match exactly)",
    "candidate_posix_semaphores": {
      "creation": "sem_open exact source name with O_CREAT|O_EXCL, mode 0600; record exact /dev/shm path device/inode/UID/mode",
      "max_names": 5,
      "mode": "0600"
    },
    "candidate_sysv_shm": {
      "creation": "shmget IPC_CREAT|IPC_EXCL|0600; verify IPC_STAT creator PID/key/UID/mode/size",
      "max_segments": 5,
      "segment_bytes": 33554532,
      "total_bytes": 167772660
    },
    "cleanup_authority": "Destructive cleanup only in normal_cleanup after durable shutdown proof and full common gates. Abnormal evidence-only performs no IPC/cleanup/journal mutation; it requires the same continuous lease and result-directory authority through final-record closure.",
    "cwd": "$D2_RESULTS",
    "dev_shm_inventory_bounds": {
      "max_entries": 4096,
      "max_entry_bytes": 9223372036854775807,
      "max_total_bytes": 9223372036854775807
    },
    "diagnostic_sem_open_failure_paths": [
      {
        "baseline_line": 970,
        "path": "Common/src/GW.cpp"
      },
      {
        "baseline_line": 1263,
        "path": "Common/src/GW.cpp"
      },
      {
        "baseline_line": 1322,
        "path": "Common/src/GW.cpp"
      }
    ],
    "diagnostic_sem_open_failure_policy": "With NG_GW_QUEUE_TEST_OBSERVATION enabled, all three sem_open failure branches emit bounded failure evidence and never call sem_unlink; macro-off default behavior remains unchanged. Documentary requirement only; implementation unauthorized.",
    "direct_host_uid_launch_permitted": false,
    "environment_contract_ref": "cmake.environment_contract",
    "exclusive_candidate_reservation_required": true,
    "execution_profile": "host-controlled; BLOCKED until an owner-controlled dedicated non-login UID and serialized single-run launcher/lease are provisioned and verified",
    "finalizer_command": "BLOCKED: must be frozen through the same dedicated-UID launcher/lease before D2 authorization",
    "finalizer_ipc_boundary": {
      "abnormal_evidence_only": {
        "allowed_file_operations": [
          "read pending record and supervisor receipt for strict validation",
          "enumerate allowlisted evidence names without following links",
          "hash safely readable closed raw file bytes within per-file and aggregate read caps",
          "write/fsync one final INCONCLUSIVE run record while continuous lease/result-directory authority holds"
        ],
        "cleanup_outcome_classifier": {
          "complete": "only a complete validated journal records exactly one REMOVED outcome for every successful CREATED resource and no cleanup failure; overall run remains INCONCLUSIVE without final inventory proof",
          "not_started": "only a complete validated journal proves neither CLEANUP_INTENT nor a successful CREATED resource",
          "partial": "validated durable journal evidence proves at least one cleanup operation failed or ended incomplete",
          "unknown": "journal absent/malformed/truncated/conflicting or any intent/outcome is ambiguous"
        },
        "forbidden": [
          "read /proc/sysvipc or /dev/shm",
          "IPC_STAT, IPC_RMID, sem_unlink or any IPC operation",
          "append/repair/close reservation journal",
          "strict-parse/replay non-control evidence schemas or require complete normal bundle",
          "cleanup, retry, PASS/FAIL, infer joins/Process/GW destruction, or call unavailable inventories empty"
        ],
        "missing_artifacts": "Exact required evidence names absent from result directory; excludes run-record.json and optional harness-diagnostics.txt.",
        "required": [
          "trusted dedicated non-login UID launcher and execution identity verified",
          "valid supervisor-termination.json schema/hash; unique wait_receipts PID set equals execution_identity.process_tree_pids",
          "every recorded PID reaped and stdout/stderr closed",
          "result-directory device/inode/owner/mode remains bound; only no-follow allowlisted paths are opened",
          "continuous dedicated-UID lease held through final-record closure",
          "pending run record strictly valid",
          "all opened non-control evidence paths are allowlisted and opened no-follow as regular closed files"
        ],
        "result": "Final status INCONCLUSIVE; cleanup history is evidence-derived, no cleanup is performed/retried; present partial artifacts are preserved as raw bytes and never treated as verified inventories.",
        "unavailable_artifacts": "Required names present but unsafe/unreadable/over-cap; disjoint from missing_artifacts and excludes optional diagnostics."
      },
      "allowed_operations": [
        "read complete /proc/sysvipc/{shm,sem,msg} table bytes and /dev/shm inventory for the required pre/post/final phases",
        "shmctl(exact_recorded_shmid, IPC_STAT, ...) to inspect the exact SysV shared-memory object identity",
        "stat/equivalent metadata inspection of the exact journal-recorded /dev/shm semaphore object, including device/inode/UID/GID/mode/link count",
        "shmctl(exact_recorded_shmid, IPC_RMID, ...) only for a complete successful CREATE receipt whose exact current identity and continuous lease recheck pass",
        "sem_unlink(exact_recorded_name) only for a complete successful CREATE receipt whose exact current identity and continuous lease recheck pass"
      ],
      "failure_rule": "If lease, supervisor/process/log proof, execution identity, pending record, result-directory identity or safe output authority is missing/ambiguous, do not invoke/re-enter the finalizer or write a final record; preserve pending/partial evidence and report BLOCKED/INCONCLUSIVE. A cleanup failure after CLEANUP_INTENT stops all later destructive calls and retries; write INCONCLUSIVE with evidence-derived partial/unknown outcome only while common authority remains valid.",
      "forbidden_operations": [
        "construct Process/GW",
        "create/open candidate IPC resources or attach/use them for message traffic",
        "invoke Gateway production IPC/send/receive paths",
        "delete an object not exactly bound to its successful journal CREATE receipt",
        "retry a removal whose durable outcome is ambiguous",
        "broad/speculative host cleanup"
      ],
      "mode_selection": "Evidence-derived. Select normal_cleanup only with durable RUN_SHUTDOWN_COMPLETE, normal wait statuses and full common prerequisites. Select abnormal_evidence_only only when normal shutdown proof is absent but all common lease/supervisor/result-directory prerequisites remain valid. Lease loss, invalid receipt, unreaped PID, open log or changed/unsafe result directory is not an abnormal-mode trigger: do not invoke/re-enter finalizer; preserve pending evidence and report BLOCKED/INCONCLUSIVE.",
      "normal_cleanup": {
        "allowed": "Only the exact normal-mode operations in allowed_operations, using journal-proven resources and immediate lease/identity recheck.",
        "required": [
          "exactly one durable RUN_SHUTDOWN_COMPLETE after all worker joins and Process/GW destruction",
          "supervisor-termination.json hash/schema valid; wait_receipts PID set exactly equals recorded process_tree_pids",
          "all raw wait statuses decode as reaped normal exits; timeout_fired=false",
          "stdout/stderr closed and present-file hashes match",
          "continuous dedicated-UID lease"
        ]
      },
      "owner_and_sequence": "The launcher binds the result directory and process_tree_pids before execution and writes the supervisor receipt only after reaping every PID and closing logs. The finalizer checks common authority/lease/file-safety gates first. Normal mode strictly validates complete evidence and performs exact journal-authorized cleanup. Abnormal mode strictly validates only pending run-record and supervisor receipt, raw-preserves bounded allowlisted non-control evidence, writes INCONCLUSIVE and records historical cleanup outcome; it performs no IPC, cleanup, journal mutation or retry.",
      "preconditions": {
        "abnormal_evidence_only": [
          "normal shutdown proof may be absent, but every both_modes prerequisite remains true",
          "pending run record and supervisor receipt strictly validate",
          "no requirement for normal joins/destruction or complete evidence bundle"
        ],
        "both_modes": [
          "trusted dedicated non-login UID launcher and execution identity verified",
          "valid supervisor-termination.json schema/hash; unique wait_receipts PID set equals execution_identity.process_tree_pids",
          "every recorded PID reaped and stdout/stderr closed",
          "result-directory device/inode/owner/mode remains bound; only no-follow allowlisted paths are opened",
          "continuous dedicated-UID lease held through final-record closure"
        ],
        "normal_cleanup": [
          "exactly one durable RUN_SHUTDOWN_COMPLETE after Gateway/output joins and Process/GW destruction, before cleanup intents",
          "no timeout/signal/ambiguous normal wait",
          "complete strict normal evidence bundle and inventory/journal validation"
        ]
      }
    },
    "finalizer_kill_grace_s": 5,
    "finalizer_mode_selection": "Evidence-derived. Select normal_cleanup only with durable RUN_SHUTDOWN_COMPLETE, normal wait statuses and full common prerequisites. Select abnormal_evidence_only only when normal shutdown proof is absent but all common lease/supervisor/result-directory prerequisites remain valid. Lease loss, invalid receipt, unreaped PID, open log or changed/unsafe result directory is not an abnormal-mode trigger: do not invoke/re-enter finalizer; preserve pending evidence and report BLOCKED/INCONCLUSIVE.",
    "finalizer_rlimits": {
      "RLIMIT_AS_bytes": 134217728,
      "RLIMIT_CORE_bytes": 0,
      "RLIMIT_CPU_seconds": 10,
      "RLIMIT_FSIZE_bytes": 4194304,
      "RLIMIT_NOFILE": 32
    },
    "finalizer_timeout_s": 10,
    "harness_spawns_children": false,
    "host_controlled": true,
    "host_inventory_may_be_nonempty": true,
    "inventory_requirements_by_mode": {
      "abnormal_evidence_only": "No live post/final inventory capture; do not require complete artifact bundle or parse/replay non-control evidence. Raw-preserve safely readable closed files; report unavailable separately from absent.",
      "normal_cleanup": "Complete pre/post/final inventories, strict journal/inventory parsing and full required artifact set are necessary for PASS/FAIL."
    },
    "kill_grace_s": 5,
    "launch_command": "BLOCKED: dedicated UID launcher and lease are not provisioned or pinned; direct host-UID launch is prohibited",
    "launch_stdout_stderr": "Outer supervisor captures stdout.txt/stderr.txt under D2_RESULTS and closes them before finalization.",
    "launcher_requirement": {
      "dedicated_non_login_uid": true,
      "exclusive_run_lease": true,
      "launcher_in_five_file_code_allowlist": false,
      "lease_held_from_pre_inventory_through_finalizer": true,
      "owner_execution_authorization_required": true,
      "process_census_alone_proves_exclusivity": false,
      "trusted_root_assumption": "host root/administrator is trusted; otherwise shared-host execution remains prohibited"
    },
    "launcher_status": "NOT_PROVISIONED; no D2 configure/build/harness/finalizer or IPC action until separate owner authorization",
    "no_broad_host_cleanup": true,
    "normal_cleanup_full_pre_post_final_inventory_required": true,
    "outer_status_capture": {
      "ambiguous_status": "128-255 without independent signal evidence is INCONCLUSIVE",
      "outer_code": "raw timeout-wrapper exit status",
      "outer_signal": "only from independent unambiguous wait evidence; otherwise null",
      "timeout_fired": "true on supervisor timeout; code 124 conservatively treated as timeout"
    },
    "reservation_umask": "0077 set in harness process before reservation",
    "rlimits": {
      "RLIMIT_AS_bytes": 536870912,
      "RLIMIT_CORE_bytes": 0,
      "RLIMIT_CPU_seconds": 30,
      "RLIMIT_FSIZE_bytes": 4194304,
      "RLIMIT_NOFILE": 64
    },
    "runtime_finalizer_cleanup": "normal_cleanup: strict complete bundle, journal replay, bounded exact identity inspection and journal-authorized cleanup under continuous lease. abnormal_evidence_only: bounded raw evidence preservation and one INCONCLUSIVE record only; no full bundle requirement, parsing/replay of non-control artifacts, IPC inspection, cleanup, journal mutation or retry.",
    "semaphore_name_mapping": "Record exact libc provider identity; prove returned sem_t* lies in a shared /proc/self/maps mapping whose device/inode equals stat of exact /dev/shm/sem.<decimal-name>; retain matching mapping evidence in journal. Missing/mismatch is INCONCLUSIVE before Process/GW construction.",
    "supervisor_termination_receipt": "Required closed artifact supervisor-termination.json v1; contains run_id, pinned launcher/lease identity, exact process_tree_pids, one raw waitpid status per PID, stdout/stderr closure flags/hashes, timeout flag and capture monotonic time. Finalizer requires exact PID-set equality, decodes each wait status, and checks each present log digest.",
    "sysv_inventory_bounds": {
      "max_entries_per_table": 4096,
      "max_reported_bytes": 9223372036854775807
    },
    "timeout_s": 20
  },
  "schema_version": "ng039-spec039-precode-review-manifest/v1.14-r12",
  "scope": {
    "algorithm_change": false,
    "broad_host_ipc_cleanup": false,
    "correctness_only": true,
    "d3_timing": false,
    "host_controlled_shared_ipc": true,
    "ngal_or_wire_time_coverage": false,
    "production_vm_access": false,
    "production_vm_or_host_ipc_fallback": false
  },
  "semantic_finalizer_contract": {
    "abnormal_evidence_only": {
      "cleanup_attempt": "never retry; preserve historical outcome as partial/complete/unknown and remain INCONCLUSIVE",
      "cleanup_outcome": "not_started/partial/complete/unknown derived from complete journal evidence; no full final-inventory verification in abnormal mode",
      "inventories": "post/final summaries unavailable and not empty; present raw files may be preserved; no live inventory capture",
      "journal_mutation": false,
      "lease_loss": "do not invoke/re-enter finalizer or write a final record; preserve pending and report BLOCKED/INCONCLUSIVE",
      "live_ipc_operations": false,
      "missing_artifacts": "exact actually absent required evidence-name set",
      "non_control_validation": "raw bounded preservation only; no strict schema parsing/replay or full-bundle gate",
      "status": "INCONCLUSIVE only",
      "trigger": "partial construction, _Exit, blocked join, timeout/signal, absent/invalid RUN_SHUTDOWN_COMPLETE or other non-normal process termination, only while all common lease/supervisor/result-directory gates remain valid",
      "unavailable_artifacts": "present but unsafe/unreadable/over-cap required names; disjoint from missing"
    },
    "artifact_checks": [
      "Both modes: accept only fixed artifact names; no-follow regular-file checks; preserve/hash only safely readable closed files within declared read bounds.",
      "Both modes: validate supervisor-termination.json and pending run-record before final-record writing.",
      "normal_cleanup only: require all eight evidence artifacts; require unique artifact names; apply complete bundle/file caps; strict-parse/replay journal and schemas; recompute raw inventories and all cross-artifact relationships.",
      "abnormal_evidence_only: do not require the complete evidence bundle; do not strict-parse/replay non-control JSON/JSONL; hash raw bytes. Missing and unavailable evidence-name sets are disjoint; optional diagnostics are excluded from missing_artifacts."
    ],
    "artifact_checks_by_mode": {
      "abnormal_evidence_only": [
        "strict pending-record and supervisor-receipt validation only",
        "no completeness or schema/replay requirement for non-control evidence",
        "raw hash safely readable closed allowlisted files within per-file and aggregate read caps",
        "classify absent required names as missing and present unsafe/unreadable/over-cap required names as unavailable",
        "optional diagnostics may be preserved but are never required or listed missing"
      ],
      "normal_cleanup": [
        "exact required artifact set",
        "optional diagnostics at most once",
        "strict JSON/JSONL/schema/order/run-ID/replay validation",
        "full inventory parsing/recomputation",
        "full bundle byte and count caps"
      ]
    },
    "assertion_to_phase": {
      "bootstrap_queue_drained": "bootstrap",
      "candidate_keys_absent_final": "final_inventory",
      "candidate_keys_absent_pre": "pre_inventory",
      "candidate_resources_attributable": "post_inventory",
      "candidate_resources_removed": "cleanup",
      "candidate_resources_reserved": "reservation",
      "evidence_complete": "finalize",
      "final_inventory_complete": "final_inventory",
      "final_inventory_matches_pre": "final_inventory",
      "finalization_complete": "finalize",
      "first_target_wait_fresh": "q1",
      "gateway_joined": "shutdown",
      "host_inventory_preflight": "pre_inventory",
      "output_worker_joined": "shutdown",
      "post_inventory_complete": "post_inventory",
      "pre_inventory_complete": "pre_inventory",
      "process_blocks_verified": "construction",
      "q0_empty_observation_delta": "q0",
      "q0_zero_controlled_offers": "q0",
      "scalar_run_identity_valid": "reservation",
      "target_admitted": "q1",
      "target_delete_false_before_gateway": "construction",
      "target_erased_once": "shutdown",
      "target_future_at_end": "q1",
      "target_is_head": "q1",
      "target_not_popped": "q1",
      "target_not_run": "q1",
      "wait_decision_correlated": "q1"
    },
    "execution_identity": {
      "failure": "missing/unverified launcher blocks before IPC; lease loss or competing process yields INCONCLUSIVE and no name-based cleanup of disputed resources",
      "lease": "held continuously from before pre-inventory through finalizer completion",
      "required": "dedicated non-login UID and trusted serialized launcher/lease; process census alone insufficient"
    },
    "expected_outcome_rule": "Every negative fixture is rejected as a PASS/FAIL claim and finalized INCONCLUSIVE with the listed bounded reason; a finalizer error/no finalized record is also never PASS.",
    "failure_summary_checks": [
      "failure.code is recomputed from the highest-precedence observed condition; ASSERTION_FAILED is used only for a demonstrated correctness predicate failure after all evidence/safety gates pass.",
      "failure.phase must be the phase mapped to the selected failed predicate or uncertainty condition.",
      "failure.missing_artifacts equals the actually absent required evidence-name set; optional diagnostics and run-record.json are excluded.",
      "failure.inventory_availability is derived from actual inventory-file presence and successful parsing, not caller-provided booleans.",
      "failure.cleanup_outcome derives from stop/join, exclusive reservation receipts, exact unlink/RMID results and final inventory equality; FAIL requires complete.",
      "failure.last_event_sequence equals the greatest validated event sequence, or null only when no valid event exists.",
      "Any contradiction in code/phase/availability/cleanup/missing-artifact fields yields INCONCLUSIVE/EVIDENCE_INVALID.",
      "failure.unavailable_artifacts equals required evidence names that are present but unsafe/unreadable/over-cap; it is disjoint from missing_artifacts, excludes optional diagnostics and run-record.json, and cannot block abnormal INCONCLUSIVE when safe raw preservation is possible."
    ],
    "finalizer_ipc_boundary": {
      "abnormal_evidence_only": {
        "allowed_file_operations": [
          "read pending record and supervisor receipt for strict validation",
          "enumerate allowlisted evidence names without following links",
          "hash safely readable closed raw file bytes within per-file and aggregate read caps",
          "write/fsync one final INCONCLUSIVE run record while continuous lease/result-directory authority holds"
        ],
        "cleanup_outcome_classifier": {
          "complete": "only a complete validated journal records exactly one REMOVED outcome for every successful CREATED resource and no cleanup failure; overall run remains INCONCLUSIVE without final inventory proof",
          "not_started": "only a complete validated journal proves neither CLEANUP_INTENT nor a successful CREATED resource",
          "partial": "validated durable journal evidence proves at least one cleanup operation failed or ended incomplete",
          "unknown": "journal absent/malformed/truncated/conflicting or any intent/outcome is ambiguous"
        },
        "forbidden": [
          "read /proc/sysvipc or /dev/shm",
          "IPC_STAT, IPC_RMID, sem_unlink or any IPC operation",
          "append/repair/close reservation journal",
          "strict-parse/replay non-control evidence schemas or require complete normal bundle",
          "cleanup, retry, PASS/FAIL, infer joins/Process/GW destruction, or call unavailable inventories empty"
        ],
        "missing_artifacts": "Exact required evidence names absent from result directory; excludes run-record.json and optional harness-diagnostics.txt.",
        "required": [
          "trusted dedicated non-login UID launcher and execution identity verified",
          "valid supervisor-termination.json schema/hash; unique wait_receipts PID set equals execution_identity.process_tree_pids",
          "every recorded PID reaped and stdout/stderr closed",
          "result-directory device/inode/owner/mode remains bound; only no-follow allowlisted paths are opened",
          "continuous dedicated-UID lease held through final-record closure",
          "pending run record strictly valid",
          "all opened non-control evidence paths are allowlisted and opened no-follow as regular closed files"
        ],
        "result": "Final status INCONCLUSIVE; cleanup history is evidence-derived, no cleanup is performed/retried; present partial artifacts are preserved as raw bytes and never treated as verified inventories.",
        "unavailable_artifacts": "Required names present but unsafe/unreadable/over-cap; disjoint from missing_artifacts and excludes optional diagnostics."
      },
      "allowed_operations": [
        "read complete /proc/sysvipc/{shm,sem,msg} table bytes and /dev/shm inventory for the required pre/post/final phases",
        "shmctl(exact_recorded_shmid, IPC_STAT, ...) to inspect the exact SysV shared-memory object identity",
        "stat/equivalent metadata inspection of the exact journal-recorded /dev/shm semaphore object, including device/inode/UID/GID/mode/link count",
        "shmctl(exact_recorded_shmid, IPC_RMID, ...) only for a complete successful CREATE receipt whose exact current identity and continuous lease recheck pass",
        "sem_unlink(exact_recorded_name) only for a complete successful CREATE receipt whose exact current identity and continuous lease recheck pass"
      ],
      "failure_rule": "If lease, supervisor/process/log proof, execution identity, pending record, result-directory identity or safe output authority is missing/ambiguous, do not invoke/re-enter the finalizer or write a final record; preserve pending/partial evidence and report BLOCKED/INCONCLUSIVE. A cleanup failure after CLEANUP_INTENT stops all later destructive calls and retries; write INCONCLUSIVE with evidence-derived partial/unknown outcome only while common authority remains valid.",
      "forbidden_operations": [
        "construct Process/GW",
        "create/open candidate IPC resources or attach/use them for message traffic",
        "invoke Gateway production IPC/send/receive paths",
        "delete an object not exactly bound to its successful journal CREATE receipt",
        "retry a removal whose durable outcome is ambiguous",
        "broad/speculative host cleanup"
      ],
      "mode_selection": "Evidence-derived. Select normal_cleanup only with durable RUN_SHUTDOWN_COMPLETE, normal wait statuses and full common prerequisites. Select abnormal_evidence_only only when normal shutdown proof is absent but all common lease/supervisor/result-directory prerequisites remain valid. Lease loss, invalid receipt, unreaped PID, open log or changed/unsafe result directory is not an abnormal-mode trigger: do not invoke/re-enter finalizer; preserve pending evidence and report BLOCKED/INCONCLUSIVE.",
      "normal_cleanup": {
        "allowed": "Only the exact normal-mode operations in allowed_operations, using journal-proven resources and immediate lease/identity recheck.",
        "required": [
          "exactly one durable RUN_SHUTDOWN_COMPLETE after all worker joins and Process/GW destruction",
          "supervisor-termination.json hash/schema valid; wait_receipts PID set exactly equals recorded process_tree_pids",
          "all raw wait statuses decode as reaped normal exits; timeout_fired=false",
          "stdout/stderr closed and present-file hashes match",
          "continuous dedicated-UID lease"
        ]
      },
      "owner_and_sequence": "The launcher binds the result directory and process_tree_pids before execution and writes the supervisor receipt only after reaping every PID and closing logs. The finalizer checks common authority/lease/file-safety gates first. Normal mode strictly validates complete evidence and performs exact journal-authorized cleanup. Abnormal mode strictly validates only pending run-record and supervisor receipt, raw-preserves bounded allowlisted non-control evidence, writes INCONCLUSIVE and records historical cleanup outcome; it performs no IPC, cleanup, journal mutation or retry.",
      "preconditions": {
        "abnormal_evidence_only": [
          "normal shutdown proof may be absent, but every both_modes prerequisite remains true",
          "pending run record and supervisor receipt strictly validate",
          "no requirement for normal joins/destruction or complete evidence bundle"
        ],
        "both_modes": [
          "trusted dedicated non-login UID launcher and execution identity verified",
          "valid supervisor-termination.json schema/hash; unique wait_receipts PID set equals execution_identity.process_tree_pids",
          "every recorded PID reaped and stdout/stderr closed",
          "result-directory device/inode/owner/mode remains bound; only no-follow allowlisted paths are opened",
          "continuous dedicated-UID lease held through final-record closure"
        ],
        "normal_cleanup": [
          "exactly one durable RUN_SHUTDOWN_COMPLETE after Gateway/output joins and Process/GW destruction, before cleanup intents",
          "no timeout/signal/ambiguous normal wait",
          "complete strict normal evidence bundle and inventory/journal validation"
        ]
      }
    },
    "inventory_checks": [
      "schema, run_id and phase agree with record role",
      "is_empty is recomputed but need not be true for host-wide pre/post/final inventories",
      "candidate keys and semaphore names are absent from pre-inventory",
      "post inventory contains no unexplained changes; exact run-created candidates may be present before cleanup",
      "only exact run-created SHM IDs and semaphore names are removed; final inventory equals pre",
      "each raw /proc/sysvipc table is present as canonical base64; finalizer recomputes SHA-256 from decoded bytes and reparses to independently derive normalized summaries",
      "all candidate resources have successful exclusive-reservation journal events, effective mode 0600 and exact kernel/file identity",
      "dedicated non-login UID and exclusive launcher lease verified continuously through finalizer; ordinary host UID is invalid",
      "diagnostic sem_open failure paths cannot issue sem_unlink; all three pinned sites are guarded in the diagnostic build",
      "returned sem_t* address lies in a shared mapping whose device/inode equals exact semaphore path identity under the recorded libc",
      "only exact run-created SHM IDs/semaphore names are removed while lease remains valid; final raw/parsed inventories equal pre",
      "each /proc/sysvipc table is retained as canonical base64; finalizer recomputes SHA-256 and reparses normalized rows/IDs",
      "all candidates have successful journal events, effective mode 0600 and exact identity",
      "dedicated UID and exclusive launcher lease verified continuously; ordinary host UID invalid",
      "diagnostic sem_open failure paths cannot issue sem_unlink",
      "returned sem_t* address lies in shared mapping whose device/inode equals exact semaphore file under recorded libc",
      "only exact run-created SHM IDs/semaphore names removed while lease valid; final raw/parsed inventories equal pre",
      "Finalizer IPC calls are limited exactly to finalizer_ipc_boundary.allowed_operations; every destructive call is preceded by the required continuous-lease and identity recheck.",
      "normal_cleanup requires durable RUN_SHUTDOWN_COMPLETE and supervisor wait/log receipt; abnormal_evidence_only performs no live inventory/IPC read or destructive call and marks post/final summaries unavailable even when unverified artifact files are preserved"
    ],
    "negative_fixtures": [
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "claimed PASS with null Q1 decision identity"
      },
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "claimed PASS with target_pop_count greater than zero"
      },
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "claimed FAIL with unknown/incomplete cleanup"
      },
      {
        "expected_failure_code": "EVIDENCE_MISSING",
        "expected_final_status": "INCONCLUSIVE",
        "input": "claimed FAIL with missing required artifact"
      },
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "failure summary code/phase/availability/cleanup disagrees with observed evidence"
      },
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "record/artifact run_id, hash or sequence mismatch"
      },
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "candidate key/name already present before reservation"
      },
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "exclusive SHM/semaphore reservation collision or partial reservation"
      },
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "reservation journal missing an object or creator/inode identity"
      },
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "candidate post resource identity differs from exclusive reservation receipt"
      },
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "failed exact cleanup or residual candidate resource"
      },
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "final IPC inventory differs from pre-inventory"
      }
    ],
    "normal_cleanup_failure": "After any durable CLEANUP_INTENT, stop subsequent destructive calls and retries on failure. While common authority remains valid, write a final INCONCLUSIVE record with cleanup_outcome=partial/complete/unknown as supported by durable journal evidence and final-inventory state. If lease/file authority is lost or safe final-record writing fails, leave pending recovery evidence; do not re-enter abnormal finalization to claim not_started.",
    "observer_checks": [
      "event sequence strictly increases and no overflow",
      "counters are monotone",
      "Q0 assertion recomputed from offer and empty-observation deltas",
      "Q1 wait/decision iteration IDs match",
      "decision sequence greater than wait sequence",
      "target/head/candidate IDs match",
      "due branch and actual pop are false",
      "target pop/run counters stay zero",
      "future head remains future at window end"
    ],
    "positive_fixtures": [
      "PASS",
      "safe FAIL",
      "finalized INCONCLUSIVE",
      "non-finalized pending INCONCLUSIVE"
    ],
    "reservation_journal": {
      "binding": "required runtime artifact for normal complete bundle; bytes/size/SHA-256 bound in final run-record when present; final run-record SHA stays in outer archive index",
      "crash_rule": "Without RUN_SHUTDOWN_COMPLETE, never perform cleanup. Abnormal evidence-only mode preserves the journal byte-for-byte and never closes/repairs it.",
      "event_order": [
        "JOURNAL_OPENED",
        "CREATE_INTENT",
        "CREATED|CREATE_FAILED",
        "RUN_SHUTDOWN_COMPLETE",
        "CLEANUP_INTENT",
        "REMOVED|CLEANUP_FAILED",
        "JOURNAL_CLOSED"
      ],
      "file_name": "reservation-journal.jsonl",
      "max_bytes": 262144,
      "max_events": 128,
      "r12_cleanup_history": "RUN_SHUTDOWN_COMPLETE precedes every cleanup intent. In abnormal mode, classify cleanup history conservatively; malformed/truncated journal means unknown, never not_started. No journal repair/close or retry.",
      "required_behavior": "exclusive mode 0600, append-only strict JSONL, monotone sequence, same run_id, fsync every event and parent directory after create",
      "schema_id": "urn:ng039:reservation-journal-event:v2",
      "schema_version": "ng039-reservation-journal-event/v2",
      "shutdown_event": "Exactly one durable RUN_SHUTDOWN_COMPLETE after all Gateway/output workers joined and Process/GW destroyed; required before normal-cleanup intent."
    },
    "schema_boundary": "JSON Schema checks shape and simple local status/value implications; the finalizer owns cross-artifact/equality/ordering rules so they are not duplicated.",
    "single_authority": "same allowlisted ng039_gw_q0_q1_d2 binary --finalize mode; no extra runner/helper file",
    "status_precedence": [
      "PASS only if all Q0/Q1 predicates, exclusive reservation, exact cleanup and final inventory equality pass",
      "FAIL only on an observable correctness assertion failure with complete evidence and safe exact cleanup",
      "otherwise INCONCLUSIVE; do not delete any resource lacking an exact reservation identity"
    ],
    "strict_json": [
      "reject duplicate object keys",
      "reject NaN/Infinity",
      "validate run-record, IPC inventory and observer event schemas"
    ]
  },
  "source": {
    "baseline_file_sha256": {
      "CMakeLists.txt": "5f9cd0676b2dcdcd0a500d778f5d5baea312301d6abc382db5eab95f2bbb1d07",
      "Common/src/GW.cpp": "7893766ab808473f53b66d8fb9e3804ae5c579bc37d700d90e78155a2ab2cc5e",
      "Common/src/GW.h": "54a97c54fe75fd76e157afc1c70ebce370f3c6edab33233babab3f3fc68f0b44",
      "Common/src/GWRunInitialization01.cpp": "1656b703c892831a05043a8bf82ae1bed73d701c8ffea7b95af5a02df0c978af",
      "Common/src/Message.cpp": "96f718f16dfa34fb081c02df872a3959a4255180798e6022e61d198905035518",
      "Common/src/Process.cpp": "bd45b77db19adf7a3c0f78bf5970996afce9720b77ea15519f5a4bcf703845d1",
      "Common/src/Process.h": "62731b2736be95b451943336100b0eee6523e7f95b3c0b7a7c99ad518d5a44f4"
    },
    "document_checkout_branch": "AIOPT5",
    "document_checkout_head": "756f493b7049434ab67cf2382486a48f606a470c",
    "document_checkout_relation": "local HEAD is two commits ahead of origin/AIOPT5; review packet is a working-tree artifact, not checked-in",
    "document_checkout_tracking": "origin/AIOPT5",
    "document_checkout_tree": "9adf665c0eb06213a29a6c12d650451948c7a4ab",
    "functional_baseline_commit": "1af604dad66148618cc0e578c28adb4a9829e607",
    "functional_baseline_tree": "5a3ebb9fbdf61185f05b18e58b8799ced40eb60b",
    "host_runtime_identity": {
      "architecture": "x86_64",
      "kernel": "Linux 6.8.0-146-generic",
      "libc": "glibc 2.39",
      "scope": "read-only host identity; no D2 IPC operation"
    },
    "primary_checkout_relation": "AIOPT5 local HEAD is 2 commits ahead of origin/AIOPT5; working SPEC/manifest/packet files are uncommitted review artifacts; unrelated SPEC-062 changes excluded.",
    "primary_checkout_status_at_freeze_input": {
      "spec039_modified": true,
      "spec062_modified_unrelated": true,
      "status_text": "snapshot recorded exactly; unrelated SPEC-062/RESULTS-SPEC-059 paths are excluded from this review",
      "unrelated_changes_included_or_validated": false,
      "untracked_results_spec039_present": true,
      "untracked_results_spec059_unrelated": true
    },
    "reviewed_spec_clean_in_primary_worktree": false,
    "source_for_code_claims": "immutable baseline Git objects only",
    "unrelated_changes_included_or_validated": false,
    "unrelated_primary_worktree_changes_present": true,
    "working_tree_diff_sha256_for_spec039": "fcca3d206346dca10c9f8d2f088b3a67c8a320b4885102baf7191feafba1c22f"
  },
  "source_snapshots_before_packet_freeze": {
    "dashboard_sha256": "7c8aa5a5a9859a919d24bacb113d11822814ed960f013684542e2bf9c3a9db5a",
    "document_head": "756f493b7049434ab67cf2382486a48f606a470c",
    "functional_baseline": "1af604dad66148618cc0e578c28adb4a9829e607",
    "ng039_task_sha256": "6ce51ef1d899d6ba4c5c7115648d0f1841e46d62b749181859541eb99e5b370e",
    "ng040_task_sha256": "30fffe284f7dca822559c8be25d09c185c05466314baf1c2fcee6a7c39c24319",
    "post_freeze_note": "Task/Dashboard record the final manifest and packet hashes only after packet freeze; those later handoff edits are not embedded back into the packet.",
    "snapshot_timing": "2026-10-12 after r11 AMEND/task reconciliation, before r12 manifest/packet freeze"
  },
  "spec": {
    "change_from_v1_11": "Same-SPEC documentary response to r08 B2/B3: block direct ordinary-UID execution; require dedicated single-run UID/launcher/lease; specify diagnostic no-unlink guards for all three GW.cpp sem_open-failure paths; define durable journal, raw inventory retention/recomputation, and returned-sem_t mapping evidence. No implementation/runtime authority.",
    "change_from_v1_12": "Resolve the sole r09 B1 by consistently defining finalizer IPC authority: bounded inventory/current-identity inspection and exact journal-authorized removal under continuous dedicated-UID lease; prohibit Process/GW construction, resource creation, production message traffic, ambiguous retries and broad cleanup. Preserve r09 B2/B3 controls. Documentary only.",
    "change_from_v1_13": "Same-SPEC r12 response to r11 AMEND: require common continuous lease/supervisor/file-authority for both modes and block finalization on loss; derive abnormal cleanup history as not_started/partial/complete/unknown without retry; add unavailable_artifacts; scope full strict evidence parsing/completeness to normal_cleanup and raw bounded preservation to abnormal evidence-only.",
    "path": "Specs/SPEC-039-common-performance-measurement.md",
    "sha256": "0cda2743e797cfbed952a1bad7c30e8e0f15d37c57e51d70e3388b9453103115",
    "status": "In Progress",
    "version": "v1.14"
  },
  "synthetic_validation": {
    "purpose": "r11 offline schema and abnormal-finalization contract probes only; no runtime/finalizer evidence",
    "r08_history": {
      "purpose": "r09 document/schema probes only; no finalizer or runtime evidence",
      "r08_history": {
        "compile_negative_cases": [
          "source outside worktree",
          "output outside build root",
          "output metadata mismatch",
          "duplicate -c",
          "wrong compiler realpath",
          "-Wp observer forwarding",
          "-Xpreprocessor observer forwarding",
          "default-profile observer undefine",
          "-ansi alternate dialect",
          "later non-C++20 standard",
          "non--O0 optimization",
          "target census mismatch",
          "diagnostic function-like observer define",
          "joined -include observer override",
          "joined -imacros observer override"
        ],
        "current_working_amendment_validation": {
          "compile_negative_cases_per_mode": 15,
          "compile_negative_rejections_total": 30,
          "compile_positive_profiles_per_mode": 2,
          "entrypoint_missing_operand_rejections_per_mode": 2,
          "link_negative_cases_per_mode": 8,
          "link_negative_rejections_total": 16,
          "link_positive_stdin_binding_per_mode": 1,
          "modes": [
            "normal",
            "PYTHONOPTIMIZE=1"
          ],
          "result": "PASS: all listed positives accepted and negatives rejected.",
          "scope": "B1/B2 correction only; complete checker matrix rerun synthetically, no D2 build/runtime/finalizer/IPC/VM",
          "total_checks_both_modes": 56
        },
        "harness_execution": false,
        "host_controlled_runtime_preflight": "not run; no IPC resources created",
        "host_controlled_schema_instance_probes": {
          "malformed_cases_rejected": 3,
          "nonempty_host_inventory_accepted": true,
          "semantic_finalizer_executed": false,
          "valid_pass_shaped_records_accepted": 1
        },
        "ipc_vm_d3_or_performance": false,
        "link_negative_cases": [
          "wrong link.txt path",
          "missing standalone -static",
          "duplicate standalone -static",
          "wrong linker compiler realpath",
          "@response file",
          "-Wl,-shared",
          "split -Xlinker -B -Xlinker dynamic",
          "symlinked link.txt"
        ],
        "purpose": "Historical r07 B1/B2/B3 checker/schema probes plus v1.11 host-profile JSON Schema meta/instance probes only; no semantic finalizer test or runtime validation.",
        "python_optimize_mode": "PASS: full amended checker matrix and literal entrypoint checks ran under normal Python and PYTHONOPTIMIZE=1; see the sidecar receipt.",
        "r07_local_preflight": {
          "PYTHONOPTIMIZE": "PASS: /usr/bin/python3 with PYTHONOPTIMIZE=1 exercised the declared synthetic checker matrix; see Specs/RESULTS-SPEC-039/r07-synthetic-validation-20261010.json.",
          "checker_syntax": "PASS 2/2; no Python assert",
          "compile_checker": "PASS: 2 positive profiles; 13 distinct negative fixtures rejected under normal and PYTHONOPTIMIZE=1 (26 negative executions).",
          "cross_artifact_finalizer": "contract specified only; no executable finalizer to test.",
          "finalizer_semantic_fixtures": "expected outcomes frozen; execution pending",
          "finalizer_semantics": "Specification/fixtures frozen, execution not performed because the D2 finalizer is not implemented.",
          "link_checker": "PASS: positive link file and stdin-binding control; 8 distinct negative fixtures rejected under normal and PYTHONOPTIMIZE=1 (16 negative executions).",
          "python_optimize_mode": "PASS with system /usr/bin/python3 and PYTHONOPTIMIZE=1; receipt Specs/RESULTS-SPEC-039/r07-synthetic-validation-20261010.json",
          "r07_checker_entrypoints": "PASS: without an unconsumed -- separator, literal complete compile default/diagnostic and link commands accept correct inputs; missing build-root/target inputs reject in both modes.",
          "r07_compile_checker_fixtures": "PASS: 2 profiles, 15 distinct negatives rejected in each mode; includes joined -include/-imacros.",
          "r07_compile_checker_synthetic": "PASS: 2 positives and 15/15 negatives rejected in both normal Python and PYTHONOPTIMIZE=1 (30 negative executions).",
          "r07_link_checker_fixtures": "PASS: valid link plus stdin-binding control, 8 negatives rejected in each mode; missing target rejects in both modes.",
          "r07_link_checker_synthetic": "PASS: 1 stdin-binding positive and 8/8 negatives rejected in both modes (16 negative executions).",
          "r07_python_optimize_mode": "PASS: B1/B2 exact-command and full checker probes ran with /usr/bin/python3 plus PYTHONOPTIMIZE=1.",
          "real_d2_configure_build_runtime": "not run",
          "run_record_schema": "PASS: valid PASS/safe FAIL; claimed PASS contradiction and unsafe/missing-evidence FAIL cases rejected.",
          "schema_meta_validation": "PASS 3/3"
        },
        "real_compile_database_or_link_txt": false,
        "real_d2_configure_or_build": false,
        "run_record_negative_cases": [
          {
            "expected_failure_code": "EVIDENCE_INVALID",
            "expected_final_status": "INCONCLUSIVE",
            "input": "claimed PASS with null Q1 decision identity"
          },
          {
            "expected_failure_code": "EVIDENCE_INVALID",
            "expected_final_status": "INCONCLUSIVE",
            "input": "claimed PASS with target_pop_count greater than zero"
          },
          {
            "expected_failure_code": "EVIDENCE_INVALID",
            "expected_final_status": "INCONCLUSIVE",
            "input": "claimed FAIL with unknown/incomplete cleanup"
          },
          {
            "expected_failure_code": "EVIDENCE_MISSING",
            "expected_final_status": "INCONCLUSIVE",
            "input": "claimed FAIL with missing required artifact"
          },
          {
            "expected_failure_code": "EVIDENCE_INVALID",
            "expected_final_status": "INCONCLUSIVE",
            "input": "failure summary code/phase/availability/cleanup disagrees with observed evidence"
          },
          {
            "expected_failure_code": "EVIDENCE_INVALID",
            "expected_final_status": "INCONCLUSIVE",
            "input": "record/artifact run_id, hash or sequence mismatch"
          }
        ]
      },
      "r09_documentary_validation": {
        "json_schemas_meta_valid": null,
        "missing_journal_pass_rejected": null,
        "missing_launcher_pass_rejected": null,
        "raw_inventory_digest_mismatch_rejected": null,
        "returned_handle_mapping_mismatch_rejected": null,
        "runtime_or_ipc_executed": false,
        "valid_inconclusive_record_accepted": null
      }
    },
    "r09_documentary_validation": {
      "all_five_schema_meta_valid": true,
      "finalizer_executed": false,
      "pass_fail_identity_lease_schema_gates_present": true,
      "raw_digest_match_and_mismatch_probes_pass": true,
      "runtime_or_ipc_executed": false,
      "valid_inventory_fixture_accepted": true,
      "valid_journal_event_fixture_accepted": true
    },
    "r10_finalizer_boundary_validation": {
      "all_five_schema_meta_valid": true,
      "expected_finalizer_operations_present": true,
      "finalizer_executed": false,
      "forbidden_operations_explicit": true,
      "manifest_scope_probes_pass": true,
      "runtime_or_ipc_executed": false,
      "single_owner_and_sequence_explicit": true,
      "spec_old_contradiction_absent": true,
      "uncertainty_fail_closed": true,
      "valid_inventory_fixture_accepted": true,
      "valid_journal_event_fixture_accepted": true
    },
    "r11_abnormal_finalization_validation": {
      "abnormal_PASS_rejected": true,
      "abnormal_cleanup_rejected": true,
      "abnormal_record_fixture_accepted": true,
      "all_schemas_meta_valid": true,
      "finalizer_executed": false,
      "missing_artifact_omission_detected": true,
      "missing_artifact_set_exactly_matches_absence": true,
      "optional_diagnostics_missing_rejected": true,
      "partial_inventory_files_preserved_without_false_missing": true,
      "run_shutdown_event_order_probed": true,
      "runtime_or_ipc_executed": false,
      "supervisor_pid_mismatch_rejected": false,
      "supervisor_receipt_fixture_accepted": true
    },
    "r12_failure_finalization_validation": {
      "abnormal_malformed_raw_evidence_accepted": true,
      "abnormal_missing_unavailable_sets_disjoint": true,
      "all_schemas_meta_valid": true,
      "cleanup_complete_outcome_classified": true,
      "cleanup_failure_partial_unknown_preserved": true,
      "cleanup_journal_structure_order_completeness_probed": true,
      "cleanup_not_started_requires_no_intent_proof": true,
      "finalizer_executed": false,
      "lease_loss_blocks_finalizer": false,
      "no_retry_after_cleanup_intent": true,
      "runtime_or_ipc_executed": false
    }
  },
  "task_id": "NG-040"
}


===== PRIMARY REVIEW PACKET BEGIN: SHA-256 a88434f223c3b66635d2589140477c01c5142ae94ba0bb839d026c3fde8013bd; bytes 70789 =====
# SPEC-039 v1.14 — r12 response to Astra r11 AMEND

Status: FROZEN CANDIDATE, UNSUBMITTED. Same-SPEC documentary amendment only. Fresh owner consent is required for exactly one review bound to the final r12 packet SHA. No code, build, finalizer, runtime, IPC, VM/SSH, D3, cleanup, commit, push or release authority.

## Frozen identities
- AIOPT5 document HEAD: `756f493b7049434ab67cf2382486a48f606a470c`; tree: `9adf665c0eb06213a29a6c12d650451948c7a4ab`.
- Functional source baseline: `1af604dad66148618cc0e578c28adb4a9829e607`; tree: `5a3ebb9fbdf61185f05b18e58b8799ced40eb60b`.
- SPEC-039 v1.14 SHA-256: `0cda2743e797cfbed952a1bad7c30e8e0f15d37c57e51d70e3388b9453103115`.
- Full manifest v1.14/r12 SHA-256: `63ccf50e6ecd19940f0c0b5e98d438ae6e34f5b2dbf0304faef97b5eca7c0917`.
- Prior immutable r11 packet SHA-256: `feb69f164ec879e1b567c44f83e7e56811d2a347fbe904a16337a7a8e3bad829`; prior SPEC/manifest: `bbb26a1331d832ab2dcbf37f1d243d7b9239e6ff1d1932319ad1141923baa609 / `c7d9598d1827b6ee91ab0e6ad9d15ebeaa04e73ddf181b363eb5f2f04f1e589b`.
- Prior r11 response SHA-256: `9ef48bdfafbfd9818613d9356f9b011972f6efc0af1d2433f7a039c3ba260ed7`; raw SSE and submission receipt hashes: `5c9ffcebbb2bd90535465bc04a54fa13155531c267bf9ab69b69f8e830174acb / `4fa8e4828ea27ffd94185ff00d3a5d3364bdf152090b4d093569863991c6aafd`.
- Functional baseline source hashes: `{"Common/src/GW.cpp": "7893766ab808473f53b66d8fb9e3804ae5c579bc37d700d90e78155a2ab2cc5e", "Common/src/Process.cpp": "bd45b77db19adf7a3c0f78bf5970996afce9720b77ea15519f5a4bcf703845d1"}`.
- Working-tree status (context only):
```
M Specs/SPEC-039-common-performance-measurement.md
 M Specs/SPEC-062-ng-elc-aiopt5-cross-vm-lifecycle-oracle.md
?? Specs/RESULTS-SPEC-039/
?? Specs/RESULTS-SPEC-059/
```

## Exact r11 verdict and bounded closure request
The owner authorized exactly one documentary review of r11; that consent is consumed. Astra returned AMEND. B1: mode selection allowed abnormal finalization after normal eligibility failed, but both-mode preconditions still required continuous lease. B2: abnormal schema forced `cleanup_outcome=not_started` even after a cleanup intent/removal may have occurred. Optional advice: normal strict schema/completeness parsing must not block bounded abnormal evidence preservation. The full r11 response is embedded verbatim below.

Review whether r12 closes these two blockers and the bounded evidence-preservation advice while preserving r08 B2/B3. Specifically assess: (1) lease/supervisor/result-directory authority is a common prerequisite; lease or identity loss blocks finalizer entry/re-entry and leaves pending evidence; (2) normal strict full-bundle checking is separated from abnormal raw preservation; malformed but safely readable non-control artifacts can be preserved and do not block INCONCLUSIVE; (3) `missing_artifacts` and `unavailable_artifacts` are exact, disjoint, exclude optional diagnostics and the run record; (4) cleanup history is `not_started` only with complete journal proof of no intent, `partial` for proven failed attempt, `unknown` for missing/ambiguous history, with no retries or rewrite; (5) a normal cleanup failure remains INCONCLUSIVE with truthful outcome while authority permits, otherwise pending; and (6) schema, SPEC and manifest contain no bypass or false-pass contradiction.

Do not infer implementation or runtime readiness. The launcher remains NOT_PROVISIONED and D2 BLOCKED. Documentary review grants no operational authority. Return concise UK English: verdict APPROVE/AMEND/HOLD, must-fix blockers with exact packet locations or none, optional advice separately, and explicit no-operational-authority statement.

## Full r11 Astra response — immutable prior evidence
## Verdict: AMEND — documentary only

Performed the single review authorised by owner message **1559215860005081219**, for packet SHA-256 `feb69f164ec879e1b567c44f83e7e56811d2a347fbe904a16337a7a8e3bad829`.

Reviewed only the supplied text; no tools, hash recomputation, code execution or runtime validation. **The original join/destruction prerequisite conflict is resolved, but two related contradictions remain.**

## Must-fix blockers

### B1 — Continuous lease remains an unconditional prerequisite for abnormal finalisation

**Locations**
- Manifest `sandbox.finalizer_ipc_boundary.preconditions.both_modes`, repeated under `semantic_finalizer_delta.finalizer_ipc_boundary`: requires a “valid dedicated non-login UID launcher and continuous lease”.
- `sandbox.finalizer_mode_selection`: otherwise permits abnormal evidence-only finalisation after confirmed reaping/log closure when normal eligibility—including continuous lease—fails.
- §7.1 normal/abnormal sequence: an uncertain lease selects no-cleanup handling, followed by the evidence-only finalisation contract.

**Conflict:** after lease loss, confirmed termination and closed logs are insufficient to satisfy the explicit *both-modes* prerequisite. The contract therefore both offers abnormal finalisation and prohibits invoking it.

**Required amendment:** distinguish the trusted-supervisor/evidence prerequisites from continuous exclusion required for IPC inspection and cleanup. Explicitly define whether lease loss permits evidence-only finalisation, and under what file-safety conditions. If it instead prevents finalisation, consistently require pending/partial evidence and `BLOCKED/INCONCLUSIVE`, rather than promising a final record. Retain the prohibition on IPC operations and automatic recovery cleanup.

### B2 — Recovery after a cleanup attempt cannot truthfully report `not_started`

**Locations**
- §7.1 normal/abnormal sequence includes “a finalizer failure requiring recovery” among abnormal-mode cases.
- The same clause, `semantic_finalizer_delta.abnormal_evidence_only.cleanup_outcome`, and run-record v5 `abnormal_evidence_only_constraints` unconditionally require `cleanup_outcome=not_started`.

**Conflict:** a normal finalizer can fail after `CLEANUP_INTENT` or a destructive call. A subsequent evidence-only recovery record would have to misreport historical cleanup as never started, or breach its schema. Performing no cleanup **during abnormal finalisation** does not establish that no earlier cleanup occurred.

**Required amendment:** either restrict this abnormal-mode contract to runs with no prior cleanup attempt and specify a separate pending/recovery disposition, or permit an evidence-derived historical cleanup outcome for recovery. Preserve ambiguity, prohibit retries, and keep recovery `INCONCLUSIVE`.

## Bounded findings otherwise satisfied

- **Normal gating:** exactly one durable `RUN_SHUTDOWN_COMPLETE`, after joins/destruction and before cleanup intents, plus verified supervisor wait/log evidence is expressly required.
- **Supervisor binding:** the semantic contract requires exact recorded PID-set equality, unique per-PID raw wait receipts, decoded termination states and hashes for present closed logs. JSON Schema alone does not enforce these cross-artifact checks.
- **Mode selection:** expressly evidence-derived, not caller-controlled, subject to B1 above.
- **Abnormal restrictions:** only `INCONCLUSIVE`; no IPC inspection, cleanup or journal mutation. Present but unverified post/final files remain artifacts, not verified inventories.
- **Schema representation:** the shown nullable reservation fields and journal v2 event support the amendment without dummy resource values. Full observation-field compatibility cannot be independently established from omitted, digest-only schema portions. PASS/FAIL remain contractually dependent on complete normal evidence.
- **Bundle accounting:** eight required evidence artifacts, one optional diagnostic, and the separate run record correctly give a ten-file maximum. Missing-artifact semantics exclude optional diagnostics and present-but-unverified files.
- **B2/B3 safeguards from prior reviews:** retained at documentary level; no new false-PASS route is established by the supplied changes.

## Optional advice

Scope the generic strict-parsing and “eight artifacts exactly once” checks explicitly to normal eligibility: malformed or absent evidence must remain preservable in abnormal mode, not prevent its permitted `INCONCLUSIVE` record.

Journal bytes alone do not independently prove historical fsync completion. Reported local validation results are supplied claims, not validation performed in this review.

## Authority boundary

The launcher remains **NOT_PROVISIONED** and D2 **BLOCKED**. This verdict grants **no operational authority**: no implementation, configure/build, harness/finalizer execution, runtime/IPC, namespace, VM/SSH, D3, cleanup, commit, push or release.

## Current SPEC clauses — exact text
Path `Specs/SPEC-039-common-performance-measurement.md`, full SHA pinned above.

### Proposed D2/finalizer boundary
```markdown
| D2 proposed allowlist (not implemented) | `CMakeLists.txt`, `Common/src/GW.h`, `Common/src/GW.cpp`, new `tests/spec039/gw_q0_q1_d2.cpp`, and `tests/spec039/q0_q1_d2.manifest.json`; separate opt-in option defaults OFF. No modifications to NGAL, `Process`, `PGCS` or `PG`, no algorithm change, separate source helper file, or D3 timing files are proposed. The D2 harness may construct the existing `Process/GW` under test; the same allowlisted harness binary's `--finalize` mode runs after the test exits without constructing `Process/GW`, creating IPC resources, or sending/receiving production messages. In `normal_cleanup` mode, it requires a durable `RUN_SHUTDOWN_COMPLETE` journal event proving worker joins and `Process/GW` destruction, plus a validated required `supervisor-termination.json` artifact containing one raw `waitpid` receipt per recorded process and stdout/stderr closure state/hashes; only then may it read inventories, inspect exact identities, and perform journal-authorized `IPC_RMID`/`sem_unlink` under the continuously verified lease. In `abnormal_evidence_only` mode, only while trusted launcher, execution/result-directory identities and lease remain valid and a valid supervisor receipt proves every process reaped and logs closed, the finalizer may hash raw, safely readable allowlisted evidence without requiring the complete bundle or parsing/replaying non-control artifacts. It writes `INCONCLUSIVE`; `failure.missing_artifacts` and `failure.unavailable_artifacts` are disjoint, accurate sets; cleanup history is exactly one of `not_started`, `partial`, `complete` or `unknown`, derived from valid evidence and never forced to `not_started`. It performs no IPC inspection, cleanup, journal mutation or retry. If lease, receipt, process/log proof or result-directory authority is unconfirmed, leave the pending record and report `BLOCKED/INCONCLUSIVE`. The trusted dedicated-UID launcher/lease is a provisioned external runtime prerequisite, not part of the five-file code allowlist; it does not exist here and direct host-UID execution is prohibited. |
```

### §7.1 normal and abnormal lifecycle sequence
```markdown
After Gateway/output workers stop and join and `Process/GW` are destroyed, the harness must append/fsync exactly one `RUN_SHUTDOWN_COMPLETE` event to the still-open reservation journal. This event is the positive proof that normal shutdown completed; a caller boolean or process exit is not a substitute. The trusted supervisor then waits/reaps every recorded harness process PID and closes stdout/stderr before invoking the same allowlisted `--finalize` binary. It selects `normal_cleanup` only when the journal contains exactly one valid shutdown-complete event, every recorded process has a verified wait receipt, both logs are closed, no timeout/signal/ambiguous termination occurred, and the lease remains continuously valid. In this mode the finalizer captures post inventory, validates/replays the journal, reads required inventories, inspects exact current identities (`shmctl(..., IPC_STAT, ...)` for SysV SHM and `stat`/equivalent metadata for the exact POSIX semaphore object), and removes only journal-proven run-created resources. For each resource it appends/fsyncs `CLEANUP_INTENT`, rechecks lease and identity immediately before the destructive call, invokes only exact `shmctl(exact_shmid, IPC_RMID, ...)` or `sem_unlink(exact_recorded_name)`, then appends/fsyncs the matching outcome. The `normal_cleanup` gate above is satisfied only while the trusted launcher, supervisor receipt, execution identity, result-directory identity and continuous lease remain valid through final-record closure. If normal-shutdown proof is absent while these common authority/file-safety conditions remain valid, the finalizer may choose `abnormal_evidence_only`. Loss or ambiguity of the lease, supervisor proof, execution identity or result-directory authority is not an abnormal-mode trigger: do not invoke or re-enter the finalizer; preserve the pending/partial record and report `BLOCKED/INCONCLUSIVE`. A failure during normal cleanup after any `CLEANUP_INTENT` stops further destructive calls and retries; while authority remains valid, record `INCONCLUSIVE` with the evidence-derived cleanup outcome in the current finalization, otherwise retain pending recovery state. No ambiguous deletion is retried.

In `abnormal_evidence_only` mode, only while the trusted launcher, execution identity, result-directory identity and continuous lease remain verified, and after a valid `supervisor-termination.json` proves every recorded process was reaped and both logs were closed, the finalizer strictly validates the pending run record and supervisor receipt. It may then open only allowlisted evidence names using no-follow regular-file checks, preserving and hashing safely readable closed raw bytes within per-file caps; it does not require all eight evidence files, parse/replay their JSON or journal schemas, or enforce the normal complete-bundle check. `failure.missing_artifacts` lists required evidence names actually absent; the new `failure.unavailable_artifacts` lists required names present but unreadable/unsafe/over-cap. These sets are disjoint, exclude the run record and optional `harness-diagnostics.txt`, and neither may falsely classify a present file as missing. Malformed but safely readable evidence is preserved and hashed, not parsed or used to block an otherwise permitted `INCONCLUSIVE` record. A bounded cleanup-history classifier may recognize only well-formed journal intent/outcome records to distinguish `not_started`, `partial` or `complete`; it does not perform full schema replay, and any missing, malformed, truncated or conflicting history yields `unknown` without preventing evidence finalization. The final record sets `record_finalized=true`, `finalization_mode=abnormal_evidence_only`, and `status=INCONCLUSIVE`. `failure.cleanup_outcome` is `not_started` only if a complete validated journal proves neither `CLEANUP_INTENT` nor a successful `CREATED` resource; it is `partial` when valid durable evidence proves at least one cleanup action failed; it is `complete` only when a complete validated journal records exactly one `REMOVED` outcome for every successful `CREATED` resource and no cleanup failure, while the run remains `INCONCLUSIVE` because abnormal mode has no final-inventory proof; and it is `unknown` when cleanup history/outcomes are absent, malformed, truncated or ambiguous. The abnormal finalizer performs no new cleanup, does not retry, and preserves journal bytes. If the lease, receipt, process/log proof or result-directory authority fails at any point, do not write/re-enter finalization; keep the pending/partial record and report `BLOCKED/INCONCLUSIVE`. A present but partial, stale, or shutdown-unverified `ipc-post.json` or `ipc-final.json` may be preserved and hashed as a file, but must not be treated as a verified inventory: the run-record post/final inventory summaries remain unavailable, with `is_empty=null` and no claimed inventory digest. The finalizer never reads `/proc/sysvipc` or `/dev/shm`, calls `IPC_STAT`, `IPC_RMID` or `sem_unlink`, appends/repairs/closes the journal, or performs cleanup during this mode.

In `normal_cleanup` mode, capture a complete final inventory and require it to match pre exactly for all `/proc/sysvipc` raw bytes/parsed summaries and `/dev/shm` entries. The final run record lists `reservation-journal.jsonl` exactly once with final size/hash and every other required runtime artifact exactly once. The finalizer strictly parses journal/inventories, recomputes event order/transitions, raw table hashes and normalized inventories, proves each cleanup event maps to one successful create receipt, and derives status from those bytes rather than caller booleans. It must not accept `journal_fsynced=true`, `mapping_verified=true` or a final inventory match as substitutes for underlying evidence. PASS/FAIL require the complete bundle and safe normal cleanup; missing/contradictory evidence, unexplained change, residual candidate, failed join, timeout or ambiguous exit is `INCONCLUSIVE`; no broad host cleanup is permitted. If a failure occurs before journal closure, retain partial evidence and do not claim PASS/FAIL. If normal cleanup has written any `CLEANUP_INTENT` and then fails, stop subsequent destructive calls and retries; while lease and result-directory authority still hold, write an `INCONCLUSIVE` final record with `cleanup_outcome=partial` or `unknown` as supported by the journal. If authority or safe record writing is lost, leave the pending/partial evidence and report `BLOCKED/INCONCLUSIVE`; never re-enter finalization to claim `not_started`. The abnormal evidence-only path described above preserves the historical cleanup outcome and performs no further cleanup. D2 remains one controlled correctness run only; no network/VM, D3 timing, performance claim, algorithm change, PGCS/PG, commit or push unless separately authorised.
```

### §7.1 v5 run-record and finalizer contract
```markdown
5. Bounded run-record and dual-mode finalizer contract: `tests/spec039/q0_q1_d2.manifest.json` defines strict Draft 2020-12 schemas for the pending/final run record v5, `supervisor-termination.json` receipt v1, IPC inventories, observer events and `reservation-journal.jsonl` events v2. The finalized run record's `finalization_mode` is exactly `normal_cleanup` or `abnormal_evidence_only`; pending records use null, and the finalizer derives the mode from journal and supervisor evidence rather than a caller label. The same allowlisted `--finalize` binary is the sole semantic authority. Both finalization modes require a strictly valid pending run record and supervisor receipt; every recorded PID reaped; closed stdout/stderr; unchanged execution/result-directory identities; and a dedicated-UID lease continuously held through final-record closure. If any common prerequisite is absent, ambiguous or lost, do not invoke/re-enter the finalizer or write a final record; retain pending/partial evidence and report `BLOCKED/INCONCLUSIVE`. In `normal_cleanup`, strictly parse/replay all required JSON/JSONL, verify raw `/proc/sysvipc` hashes/rows, exact artifact set, wait/log receipt, lease and semaphore mapping, then inspect exact identities and perform only journal-authorised cleanup after `CLEANUP_INTENT` fsync and immediate identity/lease recheck. `RUN_SHUTDOWN_COMPLETE` must be durable after joins/destruction and before any cleanup intent. PASS/FAIL require the full normal evidence and inventory gates; if a normal cleanup attempt fails after an intent, stop further destructive calls/retries and write `INCONCLUSIVE` with `cleanup_outcome=partial` or `unknown` as supported by durable evidence. If the final record cannot safely be written, leave pending recovery evidence.

In `abnormal_evidence_only`, normal shutdown proof may be absent, but all common prerequisites still hold. Strictly validate the pending record and supervisor receipt; for other allowlisted evidence files, use no-follow regular-file checks, preserve/hash safely readable closed raw bytes within bounds, but do not require the full bundle or parse/replay inventory, observer or journal schemas. Malformed but safely readable evidence must not block a final `INCONCLUSIVE` record. `failure.missing_artifacts` is exactly the required evidence names actually absent; `failure.unavailable_artifacts` records required names present but unsafe, unreadable or over-cap. The lists are disjoint; neither includes the record itself or optional `harness-diagnostics.txt`. Bound raw-byte reads by the per-file and 32 MiB total caps; classify over-budget files unavailable instead of aborting final-record creation. Present post/final files may be preserved but their inventory summaries remain unavailable, never empty. Derive `failure.cleanup_outcome`: `not_started` only when a complete validated journal proves neither `CLEANUP_INTENT` nor a successful `CREATED` resource; `partial` when valid durable evidence proves a cleanup attempt failed; `complete` only when a complete validated journal records exactly one `REMOVED` outcome for every successful `CREATED` resource and no cleanup failure, while the abnormal record remains `INCONCLUSIVE` without final-inventory proof; and `unknown` when cleanup history or outcomes are incomplete, malformed, truncated or ambiguous. This mode always yields `INCONCLUSIVE`, performs no IPC inspection, cleanup or journal mutation, and never retries. A cleanup intent or prior removal is reported truthfully, not rewritten as `not_started`.
```

### §7.1 bounded regression and artifact rules
```markdown
The bounded semantic regression set must reject claimed PASS/FAIL for missing/unverified launcher or lease, a second/foreign process, diagnostic `sem_open` failure calling `sem_unlink`, pre-existing candidate keys/names, unreadable/over-cap/raw-parsed-mismatched normal inventories, noncanonical base64, missing/reordered/duplicated/unfsynced normal journal events, candidate identity/mode mismatch, missing `sem_t` mapping, residual candidates, unexpected host changes, or final-inventory mismatch. Reject normal cleanup without a durable `RUN_SHUTDOWN_COMPLETE`, complete supervisor wait receipts, closed logs and continuous lease. Reject invocation of either mode before all recorded PIDs are reaped and logs closed; when common launcher/receipt/lease/result-directory authority is absent, retain pending evidence and report `BLOCKED/INCONCLUSIVE`. Abnormal fixtures must accept malformed but safely readable non-control evidence as raw preserved/hashable bytes without requiring a complete bundle or parsing it; reject claims of PASS/FAIL, empty inventories, joins/destruction, optional diagnostics in `missing_artifacts`, overlap between `missing_artifacts` and `unavailable_artifacts`, or an inaccurate cleanup-history outcome. `not_started` requires a complete journal proving no cleanup intent and no successful `CREATED` resource; a proven failed attempt is `partial`; `complete` requires exactly one `REMOVED` outcome for every successful `CREATED` resource and no cleanup failure; missing/truncated/malformed, intent-without-outcome, or created-without-cleanup history is `unknown`. Abnormal mode must never retry cleanup or mutate journal/IPC. The finalizer must perform no speculative deletion for disputed resources. Normal PASS/FAIL requires a complete, replayable journal and every required evidence artifact; the normal bundle has eight evidence artifacts plus `run-record.json`, optional `harness-diagnostics.txt`, and a ten-file maximum. Normal artifacts are each ≤4 MiB, the journal ≤256 KiB/128 events, the final run record ≤256 KiB, and the complete normal bundle ≤32 MiB. Abnormal raw preservation remains no-follow and bounded: per-file/total read caps apply, but excess/unsafe present files are recorded as unavailable rather than making strict schema/completeness validation a prerequisite to the `INCONCLUSIVE` record.
```

### §7.1 runtime/recovery boundary
```markdown
The host-controlled runtime shape remains one controlled D2 process tree under the dedicated UID, one Q0/Q1 correctness run, no network input and no performance timing. The launcher serializes the run and finalizer under the verified exclusive lease; the harness itself spawns no child process. The host IPC namespace may contain unrelated objects, but all candidate resources must be absent before reservation and all exact cleanup remains bound to the durable journal. No broad host cleanup is permitted. If the launcher/lease or result-directory identity is absent/lost, a second process enters the identity domain, or supervisor evidence is incomplete, do not perform name-based unlink/RMID or invoke/re-enter finalization; preserve pending/partial evidence and report `BLOCKED/INCONCLUSIVE`. An abnormal record may be written only while common lease and file-safety authority remains valid.
```

## Machine-readable r12 manifest projection
The complete manifest SHA is pinned above. This projection contains all changed mode policies, the full changed failure object schema and abnormal conditional, relevant collection rules and the current validation claims. Unchanged schema portions are digest-bound, not claimed as re-reviewed.
```json
{
  "collection_responsibility": {
    "artifact_name_uniqueness": "Normal cleanup requires each of eight evidence artifacts exactly once, optional diagnostics at most once, and run-record separately. Abnormal evidence-only does not require a complete bundle; missing_artifacts is exact absent required names, unavailable_artifacts is present but unsafe/unreadable/over-cap required names, and the sets are disjoint.",
    "finalizer_semantic_authority": "The same allowlisted finalizer uses common lease/supervisor/result-directory gates. Normal mode performs strict completeness/schema/replay checks and exact cleanup. Abnormal mode strictly validates pending record and supervisor receipt only, raw-preserves bounded evidence, records actual cleanup history, and never performs IPC, cleanup, journal mutation or retry.",
    "harness_run": "Only after dedicated UID launcher/lease verified: create pending record/journal, create/reserve candidates and collect evidence. After all worker joins and Process/GW destruction, append/fsync one RUN_SHUTDOWN_COMPLETE before any cleanup intent. The supervisor later emits supervisor-termination.json after process wait/log closure.",
    "outer_finalizer": "The finalizer requires valid lease, result-directory identity, supervisor receipt, complete process reaping and closed logs for either mode. If common authority fails, leave pending and report BLOCKED/INCONCLUSIVE. Normal mode may clean only after durable shutdown proof; abnormal mode records INCONCLUSIVE without live IPC or new cleanup.",
    "supervisor_termination_receipt": "Trusted launcher writes/fsyncs supervisor-termination.json after waiting/reaping every recorded process PID and closing stdout/stderr. The finalizer binds the receipt PID set to execution_identity.process_tree_pids, decodes raw waitpid statuses, checks closed-log hashes and validates launcher/lease identity."
  },
  "journal_event_schema_v2": {
    "$id": "urn:ng039:reservation-journal-event:v2",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "additionalProperties": false,
    "properties": {
      "candidate_key": {
        "minimum": 1,
        "type": [
          "integer",
          "null"
        ]
      },
      "error_number": {
        "minimum": 0,
        "type": [
          "integer",
          "null"
        ]
      },
      "event": {
        "enum": [
          "JOURNAL_OPENED",
          "CREATE_INTENT",
          "CREATED",
          "CREATE_FAILED",
          "RUN_SHUTDOWN_COMPLETE",
          "CLEANUP_INTENT",
          "REMOVED",
          "CLEANUP_FAILED",
          "JOURNAL_CLOSED"
        ]
      },
      "identity": {
        "additionalProperties": false,
        "properties": {
          "candidate_key": {
            "minimum": 1,
            "type": [
              "integer",
              "null"
            ]
          },
          "creator_pid": {
            "minimum": 2,
            "type": [
              "integer",
              "null"
            ]
          },
          "creator_uid": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "device": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "gid": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "inode": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "libc_path": {
            "maxLength": 1024,
            "type": [
              "string",
              "null"
            ]
          },
          "libc_sha256": {
            "pattern": "^[0-9a-f]{64}$",
            "type": [
              "string",
              "null"
            ]
          },
          "libc_version": {
            "maxLength": 64,
            "type": [
              "string",
              "null"
            ]
          },
          "map_device_major": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "map_device_minor": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "map_end": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "map_inode": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "map_start": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "mapping_line": {
            "maxLength": 4096,
            "type": [
              "string",
              "null"
            ]
          },
          "mode": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "nlink": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "owner_uid": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "path": {
            "maxLength": 512,
            "type": [
              "string",
              "null"
            ]
          },
          "sem_t_address": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "shmid": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "size_bytes": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "source_name": {
            "maxLength": 255,
            "type": [
              "string",
              "null"
            ]
          },
          "uid": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          }
        },
        "type": [
          "object",
          "null"
        ]
      },
      "monotonic_ns": {
        "minimum": 0,
        "type": "integer"
      },
      "pid": {
        "minimum": 2,
        "type": "integer"
      },
      "resource_type": {
        "enum": [
          "sysv_shm",
          "posix_semaphore",
          "journal",
          null
        ],
        "type": [
          "string",
          "null"
        ]
      },
      "run_id": {
        "maxLength": 64,
        "minLength": 16,
        "type": "string"
      },
      "schema_version": {
        "const": "ng039-reservation-journal-event/v2"
      },
      "sequence": {
        "maximum": 128,
        "minimum": 1,
        "type": "integer"
      },
      "source_name": {
        "maxLength": 255,
        "type": [
          "string",
          "null"
        ]
      },
      "uid": {
        "minimum": 0,
        "type": "integer"
      }
    },
    "required": [
      "schema_version",
      "run_id",
      "sequence",
      "event",
      "monotonic_ns",
      "pid",
      "uid",
      "resource_type",
      "candidate_key",
      "source_name",
      "error_number",
      "identity"
    ],
    "type": "object"
  },
  "manifest_sha256": "63ccf50e6ecd19940f0c0b5e98d438ae6e34f5b2dbf0304faef97b5eca7c0917",
  "r11_review": {
    "consent_message_id": "1559215860005081219",
    "manifest_sha256": "c7d9598d1827b6ee91ab0e6ad9d15ebeaa04e73ddf181b363eb5f2f04f1e589b",
    "must_fix": "Keep abnormal finalization behind common lease/supervisor/result-directory authority; derive historical cleanup as not_started/partial/complete/unknown from evidence and never retry; scope strict bundle parsing to normal mode.",
    "one_call_consent_consumed": true,
    "packet_sha256": "feb69f164ec879e1b567c44f83e7e56811d2a347fbe904a16337a7a8e3bad829",
    "raw_sse_path": "Specs/RESULTS-SPEC-039/astra-review-v1.14-r11-20261012.raw.sse",
    "raw_sse_sha256": "5c9ffcebbb2bd90535465bc04a54fa13155531c267bf9ab69b69f8e830174acb",
    "response_path": "Specs/RESULTS-SPEC-039/astra-review-v1.14-r11-20261012.md",
    "response_sha256": "9ef48bdfafbfd9818613d9356f9b011972f6efc0af1d2433f7a039c3ba260ed7",
    "spec_sha256": "bbb26a1331d832ab2dcbf37f1d243d7b9239e6ff1d1932319ad1141923baa609",
    "submission_receipt_path": "Specs/RESULTS-SPEC-039/astra-review-v1.14-r11-20261012-submission.json",
    "submission_receipt_sha256": "4fa8e4828ea27ffd94185ff00d3a5d3364bdf152090b4d093569863991c6aafd",
    "verdict": "AMEND — documentary scope only; r10 B1 was resolved, but r11 identified two remaining blockers: lease loss could select abnormal finalization despite a both-modes lease prerequisite; cleanup history after a cleanup intent could be misreported as not_started. Optional advice: scope strict full-bundle parsing/completeness to normal mode."
  },
  "review_scope": "Same-SPEC v1.14 documentary response to Astra r11 B1/B2 and evidence-preservation advice. No code, configure/build, harness/finalizer execution, IPC, D3, commit or push. Fresh exact-SHA owner consent is required for one r12 Astra review. D2 remains blocked until dedicated launcher/lease is provisioned and a separate owner execution decision is recorded. Abnormal evidence-only writes a final record only while the trusted launcher, execution/result-directory identities, supervisor proof and continuous lease remain valid; otherwise preserve pending evidence. It records historical cleanup outcome and never retries.",
  "run_record_v5_changed_contract": {
    "$id": "urn:ng039:run-record:v5",
    "abnormal_finalization_conditional": {
      "if": {
        "properties": {
          "finalization_mode": {
            "const": "abnormal_evidence_only"
          }
        },
        "required": [
          "finalization_mode"
        ]
      },
      "then": {
        "properties": {
          "cleanup": {
            "properties": {
              "candidate_resources_removed": {
                "enum": [
                  false,
                  null
                ]
              },
              "complete": {
                "const": false
              },
              "final_inventory_complete": {
                "enum": [
                  false,
                  null
                ]
              },
              "final_inventory_matches_pre": {
                "enum": [
                  false,
                  null
                ]
              },
              "finalizer_complete": {
                "const": true
              },
              "gateway_joined": {
                "enum": [
                  false,
                  null
                ]
              },
              "output_worker_joined": {
                "enum": [
                  false,
                  null
                ]
              },
              "process_destroyed": {
                "enum": [
                  false,
                  null
                ]
              }
            },
            "required": [
              "complete",
              "candidate_resources_removed",
              "finalizer_complete",
              "gateway_joined",
              "output_worker_joined",
              "process_destroyed",
              "final_inventory_complete",
              "final_inventory_matches_pre"
            ],
            "type": "object"
          },
          "failure": {
            "properties": {
              "cleanup_outcome": {
                "enum": [
                  "not_started",
                  "partial",
                  "complete",
                  "unknown"
                ]
              }
            },
            "required": [
              "cleanup_outcome",
              "missing_artifacts",
              "unavailable_artifacts"
            ],
            "type": "object"
          },
          "inventories": {
            "properties": {
              "final": {
                "properties": {
                  "available": {
                    "const": false
                  },
                  "complete": {
                    "enum": [
                      false,
                      null
                    ]
                  },
                  "is_empty": {
                    "type": "null"
                  },
                  "sha256": {
                    "type": "null"
                  }
                },
                "required": [
                  "available",
                  "complete",
                  "is_empty",
                  "sha256"
                ],
                "type": "object"
              },
              "post": {
                "properties": {
                  "available": {
                    "const": false
                  },
                  "complete": {
                    "enum": [
                      false,
                      null
                    ]
                  },
                  "is_empty": {
                    "type": "null"
                  },
                  "sha256": {
                    "type": "null"
                  }
                },
                "required": [
                  "available",
                  "complete",
                  "is_empty",
                  "sha256"
                ],
                "type": "object"
              }
            },
            "required": [
              "post",
              "final"
            ],
            "type": "object"
          },
          "record_finalized": {
            "const": true
          },
          "status": {
            "const": "INCONCLUSIVE"
          }
        },
        "required": [
          "record_finalized",
          "status",
          "failure",
          "cleanup",
          "inventories",
          "artifacts"
        ]
      }
    },
    "failure_object_schema": {
      "additionalProperties": false,
      "properties": {
        "cleanup_outcome": {
          "enum": [
            "complete",
            "partial",
            "not_started",
            "unknown"
          ]
        },
        "code": {
          "enum": [
            "ASSERTION_FAILED",
            "HOST_INVENTORY_FAILED",
            "PRE_INVENTORY_FAILED",
            "PARTIAL_CONSTRUCTION",
            "IDENTITY_MISMATCH",
            "COUNTER_OVERFLOW",
            "TIMEOUT",
            "SIGNAL",
            "JOIN_BLOCKED",
            "OWNERSHIP_UNCERTAIN",
            "INVENTORY_INCOMPLETE",
            "EVIDENCE_MISSING",
            "EVIDENCE_INVALID",
            "RESOURCE_LIMIT",
            "OTHER",
            "CANDIDATE_IDENTITY_UNCERTAIN",
            "CANDIDATE_CLEANUP_FAILED",
            "FINALIZER_FAILED"
          ]
        },
        "inventory_availability": {
          "additionalProperties": false,
          "properties": {
            "final": {
              "type": "boolean"
            },
            "post": {
              "type": "boolean"
            },
            "pre": {
              "type": "boolean"
            }
          },
          "required": [
            "pre",
            "post",
            "final"
          ],
          "type": "object"
        },
        "last_event_sequence": {
          "minimum": 0,
          "type": [
            "integer",
            "null"
          ]
        },
        "message": {
          "maxLength": 1024,
          "type": "string"
        },
        "missing_artifacts": {
          "items": {
            "enum": [
              "stdout.txt",
              "stderr.txt",
              "ipc-pre.json",
              "ipc-post.json",
              "ipc-final.json",
              "observer-events.json",
              "reservation-journal.jsonl",
              "supervisor-termination.json"
            ]
          },
          "maxItems": 8,
          "type": "array",
          "uniqueItems": true
        },
        "phase": {
          "enum": [
            "preflight",
            "reservation",
            "pre_inventory",
            "construction",
            "q0",
            "q1",
            "shutdown",
            "post_inventory",
            "cleanup",
            "final_inventory",
            "capture",
            "validation",
            "unknown",
            "finalize"
          ]
        },
        "unavailable_artifacts": {
          "items": {
            "enum": [
              "stdout.txt",
              "stderr.txt",
              "ipc-pre.json",
              "ipc-post.json",
              "ipc-final.json",
              "observer-events.json",
              "reservation-journal.jsonl",
              "supervisor-termination.json"
            ]
          },
          "maxItems": 8,
          "type": "array",
          "uniqueItems": true
        }
      },
      "required": [
        "code",
        "phase",
        "message",
        "last_event_sequence",
        "inventory_availability",
        "cleanup_outcome",
        "missing_artifacts",
        "unavailable_artifacts"
      ],
      "type": "object"
    },
    "prior_r11_run_record_schema_sha256": "59e9aade316f74dd9fe7447ffe323cdcec536046e88c6e6750af1bea7a1fd879",
    "run_record_schema_sha256": "ba05cd0bf45cac024771361e713d3d7df0a69f967ad618ce5c98047f87d72b24",
    "schema_version": {
      "const": "ng039-d2-run-record/v5"
    },
    "spec_sha256": {
      "const": "0cda2743e797cfbed952a1bad7c30e8e0f15d37c57e51d70e3388b9453103115"
    }
  },
  "runtime_bundle": {
    "abnormal_evidence_policy": "No complete-bundle requirement or non-control JSON/JSONL schema/replay in abnormal_evidence_only. Preserve/hash safe raw files under per-file/32 MiB aggregate read caps; classify absent required names as missing and present unsafe/unreadable/over-cap required names as unavailable; optional diagnostics is never required or listed missing.",
    "max_artifact_bytes": 4194304,
    "max_artifacts_excluding_run_record": 9,
    "max_files_total": 10,
    "max_total_bytes": 33554432,
    "optional_files": [
      "harness-diagnostics.txt"
    ],
    "required_files": [
      "run-record.json",
      "stdout.txt",
      "stderr.txt",
      "ipc-pre.json",
      "ipc-post.json",
      "ipc-final.json",
      "observer-events.json",
      "reservation-journal.jsonl",
      "supervisor-termination.json"
    ]
  },
  "sandbox": {
    "abnormal_evidence_preservation": "Open only allowlisted no-follow regular files after supervisor proof; hash raw bytes within per-file/aggregate limits. Missing required names and present-but-unavailable required names are disjoint arrays. Unsafe common authority/lease means pending BLOCKED/INCONCLUSIVE, not finalization.",
    "abnormal_mode_live_inventory_capture": false,
    "cleanup_authority": "Destructive cleanup only in normal_cleanup after durable shutdown proof and full common gates. Abnormal evidence-only performs no IPC/cleanup/journal mutation; it requires the same continuous lease and result-directory authority through final-record closure.",
    "finalizer_ipc_boundary": {
      "abnormal_evidence_only": {
        "allowed_file_operations": [
          "read pending record and supervisor receipt for strict validation",
          "enumerate allowlisted evidence names without following links",
          "hash safely readable closed raw file bytes within per-file and aggregate read caps",
          "write/fsync one final INCONCLUSIVE run record while continuous lease/result-directory authority holds"
        ],
        "cleanup_outcome_classifier": {
          "complete": "only a complete validated journal records exactly one REMOVED outcome for every successful CREATED resource and no cleanup failure; overall run remains INCONCLUSIVE without final inventory proof",
          "not_started": "only a complete validated journal proves neither CLEANUP_INTENT nor a successful CREATED resource",
          "partial": "validated durable journal evidence proves at least one cleanup operation failed or ended incomplete",
          "unknown": "journal absent/malformed/truncated/conflicting or any intent/outcome is ambiguous"
        },
        "forbidden": [
          "read /proc/sysvipc or /dev/shm",
          "IPC_STAT, IPC_RMID, sem_unlink or any IPC operation",
          "append/repair/close reservation journal",
          "strict-parse/replay non-control evidence schemas or require complete normal bundle",
          "cleanup, retry, PASS/FAIL, infer joins/Process/GW destruction, or call unavailable inventories empty"
        ],
        "missing_artifacts": "Exact required evidence names absent from result directory; excludes run-record.json and optional harness-diagnostics.txt.",
        "required": [
          "trusted dedicated non-login UID launcher and execution identity verified",
          "valid supervisor-termination.json schema/hash; unique wait_receipts PID set equals execution_identity.process_tree_pids",
          "every recorded PID reaped and stdout/stderr closed",
          "result-directory device/inode/owner/mode remains bound; only no-follow allowlisted paths are opened",
          "continuous dedicated-UID lease held through final-record closure",
          "pending run record strictly valid",
          "all opened non-control evidence paths are allowlisted and opened no-follow as regular closed files"
        ],
        "result": "Final status INCONCLUSIVE; cleanup history is evidence-derived, no cleanup is performed/retried; present partial artifacts are preserved as raw bytes and never treated as verified inventories.",
        "unavailable_artifacts": "Required names present but unsafe/unreadable/over-cap; disjoint from missing_artifacts and excludes optional diagnostics."
      },
      "allowed_operations": [
        "read complete /proc/sysvipc/{shm,sem,msg} table bytes and /dev/shm inventory for the required pre/post/final phases",
        "shmctl(exact_recorded_shmid, IPC_STAT, ...) to inspect the exact SysV shared-memory object identity",
        "stat/equivalent metadata inspection of the exact journal-recorded /dev/shm semaphore object, including device/inode/UID/GID/mode/link count",
        "shmctl(exact_recorded_shmid, IPC_RMID, ...) only for a complete successful CREATE receipt whose exact current identity and continuous lease recheck pass",
        "sem_unlink(exact_recorded_name) only for a complete successful CREATE receipt whose exact current identity and continuous lease recheck pass"
      ],
      "failure_rule": "If lease, supervisor/process/log proof, execution identity, pending record, result-directory identity or safe output authority is missing/ambiguous, do not invoke/re-enter the finalizer or write a final record; preserve pending/partial evidence and report BLOCKED/INCONCLUSIVE. A cleanup failure after CLEANUP_INTENT stops all later destructive calls and retries; write INCONCLUSIVE with evidence-derived partial/unknown outcome only while common authority remains valid.",
      "forbidden_operations": [
        "construct Process/GW",
        "create/open candidate IPC resources or attach/use them for message traffic",
        "invoke Gateway production IPC/send/receive paths",
        "delete an object not exactly bound to its successful journal CREATE receipt",
        "retry a removal whose durable outcome is ambiguous",
        "broad/speculative host cleanup"
      ],
      "mode_selection": "Evidence-derived. Select normal_cleanup only with durable RUN_SHUTDOWN_COMPLETE, normal wait statuses and full common prerequisites. Select abnormal_evidence_only only when normal shutdown proof is absent but all common lease/supervisor/result-directory prerequisites remain valid. Lease loss, invalid receipt, unreaped PID, open log or changed/unsafe result directory is not an abnormal-mode trigger: do not invoke/re-enter finalizer; preserve pending evidence and report BLOCKED/INCONCLUSIVE.",
      "normal_cleanup": {
        "allowed": "Only the exact normal-mode operations in allowed_operations, using journal-proven resources and immediate lease/identity recheck.",
        "required": [
          "exactly one durable RUN_SHUTDOWN_COMPLETE after all worker joins and Process/GW destruction",
          "supervisor-termination.json hash/schema valid; wait_receipts PID set exactly equals recorded process_tree_pids",
          "all raw wait statuses decode as reaped normal exits; timeout_fired=false",
          "stdout/stderr closed and present-file hashes match",
          "continuous dedicated-UID lease"
        ]
      },
      "owner_and_sequence": "The launcher binds the result directory and process_tree_pids before execution and writes the supervisor receipt only after reaping every PID and closing logs. The finalizer checks common authority/lease/file-safety gates first. Normal mode strictly validates complete evidence and performs exact journal-authorized cleanup. Abnormal mode strictly validates only pending run-record and supervisor receipt, raw-preserves bounded allowlisted non-control evidence, writes INCONCLUSIVE and records historical cleanup outcome; it performs no IPC, cleanup, journal mutation or retry.",
      "preconditions": {
        "abnormal_evidence_only": [
          "normal shutdown proof may be absent, but every both_modes prerequisite remains true",
          "pending run record and supervisor receipt strictly validate",
          "no requirement for normal joins/destruction or complete evidence bundle"
        ],
        "both_modes": [
          "trusted dedicated non-login UID launcher and execution identity verified",
          "valid supervisor-termination.json schema/hash; unique wait_receipts PID set equals execution_identity.process_tree_pids",
          "every recorded PID reaped and stdout/stderr closed",
          "result-directory device/inode/owner/mode remains bound; only no-follow allowlisted paths are opened",
          "continuous dedicated-UID lease held through final-record closure"
        ],
        "normal_cleanup": [
          "exactly one durable RUN_SHUTDOWN_COMPLETE after Gateway/output joins and Process/GW destruction, before cleanup intents",
          "no timeout/signal/ambiguous normal wait",
          "complete strict normal evidence bundle and inventory/journal validation"
        ]
      }
    },
    "finalizer_mode_selection": "Evidence-derived. Select normal_cleanup only with durable RUN_SHUTDOWN_COMPLETE, normal wait statuses and full common prerequisites. Select abnormal_evidence_only only when normal shutdown proof is absent but all common lease/supervisor/result-directory prerequisites remain valid. Lease loss, invalid receipt, unreaped PID, open log or changed/unsafe result directory is not an abnormal-mode trigger: do not invoke/re-enter finalizer; preserve pending evidence and report BLOCKED/INCONCLUSIVE.",
    "inventory_requirements_by_mode": {
      "abnormal_evidence_only": "No live post/final inventory capture; do not require complete artifact bundle or parse/replay non-control evidence. Raw-preserve safely readable closed files; report unavailable separately from absent.",
      "normal_cleanup": "Complete pre/post/final inventories, strict journal/inventory parsing and full required artifact set are necessary for PASS/FAIL."
    },
    "launcher_requirement": {
      "dedicated_non_login_uid": true,
      "exclusive_run_lease": true,
      "launcher_in_five_file_code_allowlist": false,
      "lease_held_from_pre_inventory_through_finalizer": true,
      "owner_execution_authorization_required": true,
      "process_census_alone_proves_exclusivity": false,
      "trusted_root_assumption": "host root/administrator is trusted; otherwise shared-host execution remains prohibited"
    },
    "launcher_status": "NOT_PROVISIONED; no D2 configure/build/harness/finalizer or IPC action until separate owner authorization",
    "normal_cleanup_full_pre_post_final_inventory_required": true,
    "runtime_finalizer_cleanup": "normal_cleanup: strict complete bundle, journal replay, bounded exact identity inspection and journal-authorized cleanup under continuous lease. abnormal_evidence_only: bounded raw evidence preservation and one INCONCLUSIVE record only; no full bundle requirement, parsing/replay of non-control artifacts, IPC inspection, cleanup, journal mutation or retry.",
    "supervisor_termination_receipt": "Required closed artifact supervisor-termination.json v1; contains run_id, pinned launcher/lease identity, exact process_tree_pids, one raw waitpid status per PID, stdout/stderr closure flags/hashes, timeout flag and capture monotonic time. Finalizer requires exact PID-set equality, decodes each wait status, and checks each present log digest."
  },
  "semantic_finalizer": {
    "abnormal_evidence_only": {
      "cleanup_attempt": "never retry; preserve historical outcome as partial/complete/unknown and remain INCONCLUSIVE",
      "cleanup_outcome": "not_started/partial/complete/unknown derived from complete journal evidence; no full final-inventory verification in abnormal mode",
      "inventories": "post/final summaries unavailable and not empty; present raw files may be preserved; no live inventory capture",
      "journal_mutation": false,
      "lease_loss": "do not invoke/re-enter finalizer or write a final record; preserve pending and report BLOCKED/INCONCLUSIVE",
      "live_ipc_operations": false,
      "missing_artifacts": "exact actually absent required evidence-name set",
      "non_control_validation": "raw bounded preservation only; no strict schema parsing/replay or full-bundle gate",
      "status": "INCONCLUSIVE only",
      "trigger": "partial construction, _Exit, blocked join, timeout/signal, absent/invalid RUN_SHUTDOWN_COMPLETE or other non-normal process termination, only while all common lease/supervisor/result-directory gates remain valid",
      "unavailable_artifacts": "present but unsafe/unreadable/over-cap required names; disjoint from missing"
    },
    "artifact_checks": [
      "Both modes: accept only fixed artifact names; no-follow regular-file checks; preserve/hash only safely readable closed files within declared read bounds.",
      "Both modes: validate supervisor-termination.json and pending run-record before final-record writing.",
      "normal_cleanup only: require all eight evidence artifacts; require unique artifact names; apply complete bundle/file caps; strict-parse/replay journal and schemas; recompute raw inventories and all cross-artifact relationships.",
      "abnormal_evidence_only: do not require the complete evidence bundle; do not strict-parse/replay non-control JSON/JSONL; hash raw bytes. Missing and unavailable evidence-name sets are disjoint; optional diagnostics are excluded from missing_artifacts."
    ],
    "artifact_checks_by_mode": {
      "abnormal_evidence_only": [
        "strict pending-record and supervisor-receipt validation only",
        "no completeness or schema/replay requirement for non-control evidence",
        "raw hash safely readable closed allowlisted files within per-file and aggregate read caps",
        "classify absent required names as missing and present unsafe/unreadable/over-cap required names as unavailable",
        "optional diagnostics may be preserved but are never required or listed missing"
      ],
      "normal_cleanup": [
        "exact required artifact set",
        "optional diagnostics at most once",
        "strict JSON/JSONL/schema/order/run-ID/replay validation",
        "full inventory parsing/recomputation",
        "full bundle byte and count caps"
      ]
    },
    "failure_summary_checks": [
      "failure.code is recomputed from the highest-precedence observed condition; ASSERTION_FAILED is used only for a demonstrated correctness predicate failure after all evidence/safety gates pass.",
      "failure.phase must be the phase mapped to the selected failed predicate or uncertainty condition.",
      "failure.missing_artifacts equals the actually absent required evidence-name set; optional diagnostics and run-record.json are excluded.",
      "failure.inventory_availability is derived from actual inventory-file presence and successful parsing, not caller-provided booleans.",
      "failure.cleanup_outcome derives from stop/join, exclusive reservation receipts, exact unlink/RMID results and final inventory equality; FAIL requires complete.",
      "failure.last_event_sequence equals the greatest validated event sequence, or null only when no valid event exists.",
      "Any contradiction in code/phase/availability/cleanup/missing-artifact fields yields INCONCLUSIVE/EVIDENCE_INVALID.",
      "failure.unavailable_artifacts equals required evidence names that are present but unsafe/unreadable/over-cap; it is disjoint from missing_artifacts, excludes optional diagnostics and run-record.json, and cannot block abnormal INCONCLUSIVE when safe raw preservation is possible."
    ],
    "finalizer_ipc_boundary": {
      "abnormal_evidence_only": {
        "allowed_file_operations": [
          "read pending record and supervisor receipt for strict validation",
          "enumerate allowlisted evidence names without following links",
          "hash safely readable closed raw file bytes within per-file and aggregate read caps",
          "write/fsync one final INCONCLUSIVE run record while continuous lease/result-directory authority holds"
        ],
        "cleanup_outcome_classifier": {
          "complete": "only a complete validated journal records exactly one REMOVED outcome for every successful CREATED resource and no cleanup failure; overall run remains INCONCLUSIVE without final inventory proof",
          "not_started": "only a complete validated journal proves neither CLEANUP_INTENT nor a successful CREATED resource",
          "partial": "validated durable journal evidence proves at least one cleanup operation failed or ended incomplete",
          "unknown": "journal absent/malformed/truncated/conflicting or any intent/outcome is ambiguous"
        },
        "forbidden": [
          "read /proc/sysvipc or /dev/shm",
          "IPC_STAT, IPC_RMID, sem_unlink or any IPC operation",
          "append/repair/close reservation journal",
          "strict-parse/replay non-control evidence schemas or require complete normal bundle",
          "cleanup, retry, PASS/FAIL, infer joins/Process/GW destruction, or call unavailable inventories empty"
        ],
        "missing_artifacts": "Exact required evidence names absent from result directory; excludes run-record.json and optional harness-diagnostics.txt.",
        "required": [
          "trusted dedicated non-login UID launcher and execution identity verified",
          "valid supervisor-termination.json schema/hash; unique wait_receipts PID set equals execution_identity.process_tree_pids",
          "every recorded PID reaped and stdout/stderr closed",
          "result-directory device/inode/owner/mode remains bound; only no-follow allowlisted paths are opened",
          "continuous dedicated-UID lease held through final-record closure",
          "pending run record strictly valid",
          "all opened non-control evidence paths are allowlisted and opened no-follow as regular closed files"
        ],
        "result": "Final status INCONCLUSIVE; cleanup history is evidence-derived, no cleanup is performed/retried; present partial artifacts are preserved as raw bytes and never treated as verified inventories.",
        "unavailable_artifacts": "Required names present but unsafe/unreadable/over-cap; disjoint from missing_artifacts and excludes optional diagnostics."
      },
      "allowed_operations": [
        "read complete /proc/sysvipc/{shm,sem,msg} table bytes and /dev/shm inventory for the required pre/post/final phases",
        "shmctl(exact_recorded_shmid, IPC_STAT, ...) to inspect the exact SysV shared-memory object identity",
        "stat/equivalent metadata inspection of the exact journal-recorded /dev/shm semaphore object, including device/inode/UID/GID/mode/link count",
        "shmctl(exact_recorded_shmid, IPC_RMID, ...) only for a complete successful CREATE receipt whose exact current identity and continuous lease recheck pass",
        "sem_unlink(exact_recorded_name) only for a complete successful CREATE receipt whose exact current identity and continuous lease recheck pass"
      ],
      "failure_rule": "If lease, supervisor/process/log proof, execution identity, pending record, result-directory identity or safe output authority is missing/ambiguous, do not invoke/re-enter the finalizer or write a final record; preserve pending/partial evidence and report BLOCKED/INCONCLUSIVE. A cleanup failure after CLEANUP_INTENT stops all later destructive calls and retries; write INCONCLUSIVE with evidence-derived partial/unknown outcome only while common authority remains valid.",
      "forbidden_operations": [
        "construct Process/GW",
        "create/open candidate IPC resources or attach/use them for message traffic",
        "invoke Gateway production IPC/send/receive paths",
        "delete an object not exactly bound to its successful journal CREATE receipt",
        "retry a removal whose durable outcome is ambiguous",
        "broad/speculative host cleanup"
      ],
      "mode_selection": "Evidence-derived. Select normal_cleanup only with durable RUN_SHUTDOWN_COMPLETE, normal wait statuses and full common prerequisites. Select abnormal_evidence_only only when normal shutdown proof is absent but all common lease/supervisor/result-directory prerequisites remain valid. Lease loss, invalid receipt, unreaped PID, open log or changed/unsafe result directory is not an abnormal-mode trigger: do not invoke/re-enter finalizer; preserve pending evidence and report BLOCKED/INCONCLUSIVE.",
      "normal_cleanup": {
        "allowed": "Only the exact normal-mode operations in allowed_operations, using journal-proven resources and immediate lease/identity recheck.",
        "required": [
          "exactly one durable RUN_SHUTDOWN_COMPLETE after all worker joins and Process/GW destruction",
          "supervisor-termination.json hash/schema valid; wait_receipts PID set exactly equals recorded process_tree_pids",
          "all raw wait statuses decode as reaped normal exits; timeout_fired=false",
          "stdout/stderr closed and present-file hashes match",
          "continuous dedicated-UID lease"
        ]
      },
      "owner_and_sequence": "The launcher binds the result directory and process_tree_pids before execution and writes the supervisor receipt only after reaping every PID and closing logs. The finalizer checks common authority/lease/file-safety gates first. Normal mode strictly validates complete evidence and performs exact journal-authorized cleanup. Abnormal mode strictly validates only pending run-record and supervisor receipt, raw-preserves bounded allowlisted non-control evidence, writes INCONCLUSIVE and records historical cleanup outcome; it performs no IPC, cleanup, journal mutation or retry.",
      "preconditions": {
        "abnormal_evidence_only": [
          "normal shutdown proof may be absent, but every both_modes prerequisite remains true",
          "pending run record and supervisor receipt strictly validate",
          "no requirement for normal joins/destruction or complete evidence bundle"
        ],
        "both_modes": [
          "trusted dedicated non-login UID launcher and execution identity verified",
          "valid supervisor-termination.json schema/hash; unique wait_receipts PID set equals execution_identity.process_tree_pids",
          "every recorded PID reaped and stdout/stderr closed",
          "result-directory device/inode/owner/mode remains bound; only no-follow allowlisted paths are opened",
          "continuous dedicated-UID lease held through final-record closure"
        ],
        "normal_cleanup": [
          "exactly one durable RUN_SHUTDOWN_COMPLETE after Gateway/output joins and Process/GW destruction, before cleanup intents",
          "no timeout/signal/ambiguous normal wait",
          "complete strict normal evidence bundle and inventory/journal validation"
        ]
      }
    },
    "normal_cleanup_failure": "After any durable CLEANUP_INTENT, stop subsequent destructive calls and retries on failure. While common authority remains valid, write a final INCONCLUSIVE record with cleanup_outcome=partial/complete/unknown as supported by durable journal evidence and final-inventory state. If lease/file authority is lost or safe final-record writing fails, leave pending recovery evidence; do not re-enter abnormal finalization to claim not_started."
  },
  "spec": {
    "change_from_v1_11": "Same-SPEC documentary response to r08 B2/B3: block direct ordinary-UID execution; require dedicated single-run UID/launcher/lease; specify diagnostic no-unlink guards for all three GW.cpp sem_open-failure paths; define durable journal, raw inventory retention/recomputation, and returned-sem_t mapping evidence. No implementation/runtime authority.",
    "change_from_v1_12": "Resolve the sole r09 B1 by consistently defining finalizer IPC authority: bounded inventory/current-identity inspection and exact journal-authorized removal under continuous dedicated-UID lease; prohibit Process/GW construction, resource creation, production message traffic, ambiguous retries and broad cleanup. Preserve r09 B2/B3 controls. Documentary only.",
    "change_from_v1_13": "Same-SPEC r12 response to r11 AMEND: require common continuous lease/supervisor/file-authority for both modes and block finalization on loss; derive abnormal cleanup history as not_started/partial/complete/unknown without retry; add unavailable_artifacts; scope full strict evidence parsing/completeness to normal_cleanup and raw bounded preservation to abnormal evidence-only.",
    "path": "Specs/SPEC-039-common-performance-measurement.md",
    "sha256": "0cda2743e797cfbed952a1bad7c30e8e0f15d37c57e51d70e3388b9453103115",
    "status": "In Progress",
    "version": "v1.14"
  },
  "validation_claims": {
    "abnormal_partial_evidence_fixture": "PASS: INCONCLUSIVE record accepted with present raw post/final artifacts and a separate unavailable-artifact name; no non-control parsing required",
    "astra_submitted": false,
    "cleanup_history_outcome_fixtures": "PASS: a bounded offline classifier accepted only schema-valid, ordered, complete journal streams; not_started requires no-intent/no-CREATED proof; failed intent=partial; complete requires exactly one REMOVED outcome for every successful CREATED resource; empty/unknown/malformed/reordered/truncated or created-without-any-cleanup streams=unknown",
    "d2_d3_measurement": false,
    "harness_execution": false,
    "interpretation": "Documentary contract only. Continuous lease and stable result-directory authority remain required for either finalization mode; if they fail, preserve pending evidence. Launcher is NOT_PROVISIONED; D2 blocked until separate gates.",
    "ipc_or_vm_activity": false,
    "json_schema_meta_validation": "PASS: five Draft 2020-12 JSON Schemas meta-validated; collection_responsibility is manifest policy data, not a schema",
    "lease_loss_policy_static_check": "PASS: the manifest contract says common prerequisite failure blocks finalizer/re-entry and leaves pending evidence; no lease-loss runtime/finalizer fixture was executed",
    "manifest_generation": "PASS: r12 generated from immutable submitted r11 manifest; r11 packet/response/raw/receipt preserved",
    "missing_unavailable_set_fixtures": "PASS: exact absent set, exact unavailable set and disjointness checked; optional diagnostics rejected from missing_artifacts",
    "owner_exact_sha_consent_for_r12": false,
    "runtime_finalizer_execution": false,
    "spec_sha256": "0cda2743e797cfbed952a1bad7c30e8e0f15d37c57e51d70e3388b9453103115",
    "supervisor_pid_set_cross_artifact_validation": "NOT EXECUTED: semantic finalizer absent; not claimed by schema tests"
  }
}
```

## Source-grounded lifecycle excerpts from immutable functional baseline
These source snippets are unchanged context only and do not implement the proposed journal, supervisor or finalizer contract.

### GW.cpp:658-667
```cpp
658:   // Stop the output thread
659:   if (StopGateway == false)
660:   {
661:     StopGateway = true;
662:     OutputQueueCV.notify_all();
663:   }
664: 
665:   T->join();
666:   delete T;
667: }
```

### Process.cpp:228-257
```cpp
228:   // Create the process blocks
229:   NewBlock("HT", PB3);
230:   NewBlock("GW", PB2);
231:   // NewBlock("CLI",PB1);
232: }
233: 
234: Process::~Process()
235: {
236:   cout << "Deleting " << LN << endl;
237: 
238:   cout << "Deleting Messages container" << endl;
239: 
240:   DeleteMessages();
241: 
242:   cout << "Done" << endl;
243: 
244:   vector<Block*>::iterator it;
245:   unsigned int i = 0;
246: 
247:   for (it = Blocks.begin(); it != Blocks.end(); it++)
248:   {
249:     delete Blocks[i];
250: 
251:     i++;
252:   }
253: 
254:   Blocks.clear();
255: 
256:   delete PMB;
257: }
```

The prior r11 response is supplied as historical immutable evidence; r12 supersedes neither its artifact identity nor verdict. The complete r11 packet and response remain archived.
Packet SHA-256 is computed out-of-band and intentionally is not embedded in itself.

===== PRIMARY REVIEW PACKET END =====
