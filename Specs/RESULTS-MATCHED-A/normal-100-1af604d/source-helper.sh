#!/bin/sh
set -eu
BASE=/root/workspace/novagenesis
RUN_ID=$(date -u +%Y%m%dT%H%M%SZ)-matched-a
IO_DIR="$BASE/IO/SourceStaging/$RUN_ID"
printf '%s\n' "$RUN_ID" > /tmp/ng050-source-run-id
cd "$BASE"
python3 Scripts/Python/BuildPhotos.py --staging "$IO_DIR" different 100 800 600
(cd "$IO_DIR" && sha256sum -c manifest.sha256 >/dev/null)
printf 'RUN_ID=%s\n' "$RUN_ID"
printf 'MANIFEST='; sha256sum "$IO_DIR/manifest.sha256"
cd "$BASE/cmake-build-debug"
exec ./ContentApp "$IO_DIR/" Source
