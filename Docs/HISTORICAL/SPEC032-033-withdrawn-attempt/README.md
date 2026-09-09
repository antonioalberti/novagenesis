# Withdrawn SPEC-032/033 attempt — historical evidence only

Snapshot source: `c6ffcf7ce71f09d8124f346f94ede915093f4769`.
Preservation tag: `recovery/pre-spec032-rollback-20260909T061553`.
Withdrawal decision: user-approved on 2026-09-09.
Rollback commit: `77efe27fafbd7483507ed1448cbeedf04782bff4`.

These ten documents are verbatim historical snapshots. Their implementation
statuses, line references, relative links, diagnoses, approval statements and
proposed sequencing describe the withdrawn attempt, not the restored AIOPT3.
Do not apply their patches or treat earlier conditional reviews as renewed approval.
All implementation commits remain in Git history; no force-push was used.

Current authority:
- `../../../Specs/SPEC-032-gw-input-queue-batching.md`: deferred.
- `../../../Specs/SPEC-033-message-commandline-refactor.md`: replacement planning draft.
- `../../DECISIONS/BASELINE-RESTORATION-2026-09-09.md`: recovery evidence and limitations.

The rollback intentionally also withdraws later safety fixes. A historically
successful workload is not a proof of memory safety. Each relevant finding must
be checked against the restored source and reproduced before a separately
approved minimal correction is introduced.
