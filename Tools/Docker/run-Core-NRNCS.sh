#!/bin/sh

# Creates the Core container running NRNCS (Name Resolution and Network Cache Service).
# Usage: sh run-Core-NRNCS.sh <id>
#   sh run-Core-NRNCS.sh 0   # creates Core0

docker run -itd --privileged --ipc=shareable --name Core$1 ng-nrncs:latest
