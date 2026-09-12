# ISSUE-001: NRInfoPayload01 Fails to Cache Some Payloads to Disk

**Date:** 2026-07-15  
**Status:** Open  
**Priority:** High  
**Related:** SPEC-022, NRInfoPayload01, HTGetBind01

---

## 1. Description

Test with 500 JPG files (~289 KB each) published via ContentApp Source. NRInfoPayload01 (NRNCS) caches payload to disk. 31/500 files (6.2%) were **not cached** — bindings were registered (NRPubNotify01), but content never reached disk.

## 2. Evidence

### ContentApp Log (source guest)

**Fast phase — 470 files delivered in ~25s:**
```
[38752.145s] RTT 0.005s  → file=00000-alpine-ng-source.jpg
[38753.240s] RTT 0.023s  → file=00003-alpine-ng-source.jpg
...
[38777.627s] RTT 0.027s  → file=00499-alpine-ng-source.jpg
```
Average RTT: ~20-40ms per file. Rate: ~14-18 files/second.

**31 subscriptions stuck without content:**
```
(Subscription 0: Status = Waiting delivery, Key = FA8CFC1C, HasContent = 0, Time from subscription = 29.422s)
(Subscription 1: Status = Waiting delivery, Key = 173B5334, HasContent = 0, Time from subscription = 29.394s)
...
(Subscription 30: Status = Waiting delivery, Key = ECCB36AB, HasContent = 0, Time from subscription = 5.423s)
```
31 bindings with `HasContent = 0` — content was never written to disk.

**Second attempt — 60-90s later, recovery:**
```
[38842.691s] RTT 89.467s  → file=00001-alpine-ng-source.jpg
[38842.708s] RTT 89.455s  → file=00002-alpine-ng-source.jpg
...
[38843.074s] RTT 65.849s  → file=00490-alpine-ng-source.jpg
```
ContentApp re-submitted the 31 requests. NRNCS responded with all in <0.4s.

### Affected Files

| File | Hash | RTT (1st attempt) |
|------|------|-------------------|
| 00001-alpine-ng-source.jpg | FA8CFC1C | 89.467s |
| 00002-alpine-ng-source.jpg | 173B5334 | 89.455s |
| 00004-alpine-ng-source.jpg | B7867AAA | 89.387s |
| 00005-alpine-ng-source.jpg | 688AE50F | 89.375s |
| 00007-alpine-ng-source.jpg | EF3B4F38 | 89.303s |
| 00011-alpine-ng-source.jpg | 2A7AD894 | 89.141s |
| 00012-alpine-ng-source.jpg | 9C1099A0 | 89.107s |
| 00038-alpine-ng-source.jpg | 49B51632 | 88.000s |
| 00039-alpine-ng-source.jpg | 4F8F3D58 | 87.964s |
| 00063-alpine-ng-source.jpg | A7545319 | 86.912s |
| 00141-alpine-ng-source.jpg | 49782822 | 83.436s |
| 00232-alpine-ng-source.jpg | C8E8EF23 | 79.122s |
| 00271-alpine-ng-source.jpg | D4887C85 | 77.301s |
| 00272-alpine-ng-source.jpg | D36A8194 | 77.267s |
| 00286-alpine-ng-source.jpg | A8C9F48C | 76.490s |
| 00319-alpine-ng-source.jpg | B307EBB9 | 74.927s |
| 00325-alpine-ng-source.jpg | 4497BB50 | 74.423s |
| 00351-alpine-ng-source.jpg | EF7CEE98 | 73.142s |
| 00356-alpine-ng-source.jpg | 55E9915C | 72.944s |
| 00371-alpine-ng-source.jpg | F3A68226 | 72.338s |
| 00407-alpine-ng-source.jpg | 8005FE0E | 70.264s |
| 00414-alpine-ng-source.jpg | AECA28BC | 69.985s |
| 00423-alpine-ng-source.jpg | 78E97943 | 69.621s |
| 00430-alpine-ng-source.jpg | 7E997696 | 69.077s |
| 00431-alpine-ng-source.jpg | 9CCED065 | 69.046s |
| 00432-alpine-ng-source.jpg | 91AB2465 | 69.010s |
| 00433-alpine-ng-source.jpg | 75560612 | 68.980s |
| 00483-alpine-ng-source.jpg | 34724DF3 | 66.109s |
| 00486-alpine-ng-source.jpg | 5481C8EB | 65.994s |
| 00487-alpine-ng-source.jpg | 9174C1E6 | 65.966s |
| 00490-alpine-ng-source.jpg | ECCB36AB | 65.849s |

## 3. Probable Cause

`NRInfoPayload01::Run()` processes the `-info --payload` that arrives in the `-p --notify` message from the Publisher. When the Publisher (ContentApp Source) publishes many photos in rapid succession (`--publish 0.1` — 100ms between each), `NRInfoPayload01` may:

1. **Race condition**: Same payload processed twice, or not at all
2. **Silent failure**: `ConvertPayloadFromCharArrayToFile()` returns error unhandled
3. **Timeout**: Payload arrives after `NRInfoPayload01` has already processed the message
4. **Lost messages**: `-info --payload` processed but `ConvertPayloadFromCharArrayToFile` not called (e.g., conditional skipping cache)

## 4. Impact

- 6.2% of publications end up without content in cache
- ContentApp must wait up to 90s for re-submission (recovery mechanism exists but is slow)
- For real-time applications (VoIP, video), 90s delay is unacceptable

## 5. Proposed Solution (TBD)

Possible approaches:
1. **Add diagnostics**: Log in `NRInfoPayload01::Run()` when `ConvertPayloadFromCharArrayToFile` fails or isn't called
2. **Verify post-condition**: After processing each `-info --payload`, verify file exists on disk
3. **Rate limiting**: Reduce `--publish` to >0.5s to avoid overload
4. **Internal retry**: If `ConvertPayloadFromCharArrayToFile` fails, retry after delay

## 6. Next Steps

- [ ] Investigate `NRInfoPayload01::Run()` to identify why 31 payloads were not cached
- [ ] Add diagnostic logging to `ConvertPayloadFromCharArrayToFile`
- [ ] Check for race condition between messages published in rapid succession
- [ ] Implement fix
- [ ] Re-test with 500 files `--publish 0.1`