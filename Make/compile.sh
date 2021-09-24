#!/bin/sh

for i in PGCS HTS GIRS PSS NRNCS NBTestApp IoTTestApp ContentApp; do g++ -std=c++20 -O0 -g3 -p -pg -Wall -fmessage-length=0 -pthread -Wno-deprecated -o $i ../$i/src/*.cpp ../Common/src/*.cpp -I ../Common/src/ -lpthread -lrt; cp $i ../cmake-build-debug; done

