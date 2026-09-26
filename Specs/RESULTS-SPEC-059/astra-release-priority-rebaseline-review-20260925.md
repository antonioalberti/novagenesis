# Astra Review — remote-first priority rebaseline (2026-09-25)

## Review identity

- **Verdict:** `AMEND — documentary rebaseline only; operational and release HOLD remain`.
- **Repository baseline supplied to Astra:** `51ea4c8a127a9c00bf340d2da02c702a19613434`.
- **Branch:** `AIOPT3`; `origin/AIOPT3`: `24a3851e1d5a90f039b9e148a1af38dd03c4d925`.
- **Working tree supplied:** only `Specs/RELEASE-MASTER-PLAN-v1.0.0.md` modified; its exact diff was included.
- **Packet SHA-256:** `dc1723d83dd6c4336dcbe9202fe753268deae040703f7169100a26a44ca8dfc4`.
- **Response SHA-256:** `80f30bf45a55d3c06636d572f22fca0ed7e136785938a8edf83e33b669aa21a8`.
- **Astra evidence boundary:** review of supplied excerpts/diff/identities only; Astra did not independently access the checkout, verify hashes, ancestry, tests, or implementation.

The complete response, stateless transcript, and raw SSE are archived under `/home/gandalf/CodeRepository/tool-codex-usage-tracker/data/sessions/`:

- `ng050-scope-priority-remote-v1-20260925-80f30bf45a55.md` — response plus metadata.
- `ng050-scope-priority-remote-v1-20260925.jsonl` — request/response transcript.
- `ng050-scope-priority-remote-v1-20260925.last.sse` — raw response body.

## Findings

1. **Priority:** Astra considers the user's requested remote-first sequence legitimate, but requires an explicit, approved, synchronized documentary rebaseline before a candidate trial. Preserve the RDEMO-first verdict as history, superseded only for sequencing if the owner approves the new baseline.
2. **Normative conflict:** `SPEC-054` §6 still makes G9's 100-photo workload a release gate and G10 depends on G1–G9. The current Master Plan v6/M2-R proposal uses five photos and defers 100/1000-photo targets. `SPEC-054` must explicitly map retained criteria and approved deferrals; five-photo scope does not silently override G9. No replacement or `N/A` is approved by this review.
3. **Additional applicability check:** `SPEC-052` §11.2 lists a later `photos-100` trial and `SPEC-053` L5 defines 100-photo normal-path evidence. The review packet did not include those full SPECs; their applicability to the exact v1.0 profile remains to be checked in the next complete packet.
4. **Other required synchronized edits if the owner approves the rebaseline:** Master Plan, NG-050, NG-060, NG-058, Dashboard, `SPEC-054` §§4/6/7, and relevant applicability references in `SPEC-060`; preserve ownership, local R0–R7 criteria, and historical evidence. Astra also identified that NG-058 must prepare the exact candidate and clear prerequisites before a separately reviewed/authorized trial can produce a real `run_trial` bundle.
5. **Next technical gate after documentary approval:** bounded exact-scope review of the `SPEC-060`/NG-058 collector-helper integration design and acceptance/test plan. The collector/helper integration and real `run_trial` bundle v2 remain open.

## Decision and authorization boundary

The owner must decide whether v1.0.0 retains G9's 100-photo requirement or formally replaces it for the declared remote profile with the five-photo oracle and defers G9/G10 accordingly. Until that decision and the synchronized rebaseline are approved, the existing normative criteria are unchanged. Current remote gates remain M0 `BLOCKED`, M1-R `HOLD`, M2-R `OPEN`, M3 `BLOCKED`.

This review authorizes no implementation, build, staging, VM, SSH, QGA, cleanup, reboot, trial, push, tag, or release. Astra explicitly kept operational and release status on `HOLD`.

## Quota record

- Model: `gpt-6-astra` (Plus).
- Usage: 20,128 input tokens, 1,467 output tokens; estimated 7.2325 credits.
- Live quota headers: primary window 2%; weekly window 93% used.
- No follow-up Astra call has been made.