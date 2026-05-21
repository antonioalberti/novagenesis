#!/bin/bash
# IoTTestApp - Start this 60 seconds after HTS

BASE=/home/gandalf/workspace/novagenesis

cd $BASE/cmake-build-debug
./IoTTestApp IoTTestApp $BASE/IO/IoTTestApp/
