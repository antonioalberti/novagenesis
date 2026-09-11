#!/bin/sh
# NovaGenesis Stop Script
: "${NG_REPO_PATH:?Set NG_REPO_PATH before running}"
echo "Stopping NovaGenesis processes..."

# Kill processes by PID files
for pidfile in "$NG_REPO_PATH"/IO/logs/*.pid; do
    if [ -f "$pidfile" ]; then
        pid=$(cat "$pidfile")
        echo "Killing process $pid..."
        kill $pid 2>/dev/null || true
        rm -f "$pidfile"
    fi
done

# Also kill by name as fallback
killall PGCS NRNCS ContentApp 2>/dev/null || true

echo "NovaGenesis processes stopped!"