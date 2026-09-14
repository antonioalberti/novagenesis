#!/bin/sh

# Compile script for NovaGenesis
# Run from: <workspace-root>/novagenesis/Make/
# Usage: bash compile.sh [service1] [service2] ...
#   If no arguments: compiles all services
#   With arguments: compiles only specified services

cd "$(dirname "$0")/.." || exit 1

NG_BUILD_PROFILE="${NG_BUILD_PROFILE:-debug}"
case "$NG_BUILD_PROFILE" in
  debug|performance|sanitizer) ;;
  *)
    echo "Unknown NG_BUILD_PROFILE: $NG_BUILD_PROFILE (use debug, performance or sanitizer)" >&2
    exit 2
    ;;
esac

BUILD_DIR="cmake-build-$NG_BUILD_PROFILE"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

case "$NG_BUILD_PROFILE" in
  debug)
    CXX_FLAGS="-O0 -g3 -Wall -fmessage-length=0 -pthread -Wno-deprecated -fno-omit-frame-pointer"
    ;;
  performance)
    CXX_FLAGS="-O2 -g -Wall -fmessage-length=0 -pthread -Wno-deprecated -fno-omit-frame-pointer"
    ;;
  sanitizer)
    CXX_FLAGS="-O1 -g -Wall -fmessage-length=0 -pthread -Wno-deprecated -fsanitize=address,undefined -fno-omit-frame-pointer"
    ;;
  *)
    echo "Unknown NG_BUILD_PROFILE: $NG_BUILD_PROFILE (use debug, performance or sanitizer)" >&2
    exit 2
    ;;
esac
echo "Build profile: $NG_BUILD_PROFILE"

# If arguments provided, compile only those services
if [ $# -gt 0 ]; then
    SERVICES="$@"
else
    SERVICES="PGCS NRNCS NBTestApp IoTTestApp ContentApp"
fi

for i in $SERVICES; do
  echo "Compiling $i..."
  if g++ -std=c++20 $CXX_FLAGS \
    -o "$BUILD_DIR/$i" "$i/src/"*.cpp Common/src/*.cpp \
    -I Common/src/ -lpthread -lrt; then
    echo "✓ $i compiled successfully"
  else
    echo "✗ $i compilation failed"
    exit 1
  fi
done

