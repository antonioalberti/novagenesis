# SPEC-059 build/preflight packet — candidate 4268fda

Candidate source HEAD: `4268fda748d09a09cd50dcc8b73b1b436b0d3f9d`
Branch: `AIOPT3`

This packet supersedes the r16 setup mismatch for future review. The build and the controller source are now captured from the same clean HEAD.

Verified:

- canonical `ng_observability.py build`, profile `obs-normal`, Debug, two jobs: return code 0;
- manifest source snapshot HEAD: `4268fda748d09a09cd50dcc8b73b1b436b0d3f9d`;
- manifest receipt SHA-256: `8883d45639da33d292af22a9df6de09aed589c531fe32c47d48a38360058c4af`;
- offline `capture_local_provenance` validation: `build_linkage=true`, `capture_complete=true`, `git_clean=true`;
- preserved manifest identity is checked in `provenance/summary.json`;
- QGA preflight in VM 100: transport exit 0, guest exit 0, UID 0, `native-privileged`, contract `LOCAL`;
- no role launch, cleanup or runtime trial performed for this packet.

The r16 trial remains preserved separately as a setup diagnostic: it used a build from `3eb46c7` while the controller source was at `81acedf`, so the controller correctly rejected `source head divergence` before launch. Its final inventory was zero.

This packet is not runtime or release acceptance. A fresh trial requires post-r16 Astra review and explicit bounded execution approval.
