#!/bin/bash
# NRNCS - Start this 10 seconds after PGCS

BASE=/home/gandalf/workspace/novagenesis

cd $BASE/cmake-build-debug
gdb -batch -ex "run" -ex "bt" -ex "quit" --args ./NRNCS $BASE/IO/NRNCS/
