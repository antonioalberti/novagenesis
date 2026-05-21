#!/bin/bash
# NRNCS - Start this 10 seconds after PGCS

BASE=/home/gandalf/workspace/novagenesis

cd $BASE/cmake-build-debug
./NRNCS $BASE/IO/NRNCS/
