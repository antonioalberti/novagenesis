#!/bin/bash
# IoTTestApp - Start this 60 seconds after HTS

BASE=${NG_REPO_PATH:-$(cd ../..; pwd)}

cd $BASE/cmake-build-debug
gdb -batch -ex "run" -ex "bt" -ex "quit" --args ./IoTTestApp IoTTestApp $BASE/IO/IoTTestApp/
