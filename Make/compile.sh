#!/bin/sh

# Compile script for NovaGenesis
# Run from: /home/gandalf/workspace/novagenesis/Make/
# Usage: bash compile.sh [service1] [service2] ...
#   If no arguments: compiles all services
#   With arguments: compiles only specified services

cd "$(dirname "$0")/.." || exit 1

BUILD_DIR="cmake-build-debug"
mkdir -p "$BUILD_DIR"

# If arguments provided, compile only those services
if [ $# -gt 0 ]; then
    SERVICES="$@"
else
    SERVICES="PGCS NRNCS NBTestApp IoTTestApp ContentApp"
fi

for i in $SERVICES; do
  echo "Compiling $i..."
  if g++ -std=c++20 -O0 -g3 -Wall -fmessage-length=0 -pthread -Wno-deprecated \
    -o "$BUILD_DIR/$i" "$i/src/"*.cpp Common/src/*.cpp \
    -I Common/src/ -lpthread -lrt; then
    echo "✓ $i compiled successfully"
  else
    echo "✗ $i compilation failed"
    exit 1
  fi
done

