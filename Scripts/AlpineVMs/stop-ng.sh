#!/bin/sh
# NovaGenesis Stop Script
echo "Stopping NovaGenesis processes..."

# Kill processes by PID files
for pidfile in /root/workspace/novagenesis/IO/logs/*.pid; do
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