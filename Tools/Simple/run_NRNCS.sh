#!/bin/bash
# NRNCS - Start this 10 seconds after PGCS

BASE=${NG_REPO_PATH:-$(cd ../..; pwd)}

cd $BASE/cmake-build-debug
gdb -batch -ex "run" -ex "bt" -ex "quit" --args ./NRNCS $BASE/IO/NRNCS/
