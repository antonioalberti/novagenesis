#!/bin/bash
#
# clean.sh — Kill NovaGenesis processes, clean IO files, purge IPC resources.
#
# Active components: PGCS, NRNCS, ContentApp, IoTTestApp, NBTestApp

set -e
BASE=$(cd "$(dirname "$0")/../.." && pwd)
ME=$(whoami)

echo "=== Killing NovaGenesis processes ==="

for proc in PGCS NRNCS ContentApp IoTTestApp NBTestApp; do
    if pgrep -x "$proc" > /dev/null 2>&1; then
        echo "  Killing $proc..."
        killall -9 "$proc" 2>/dev/null || true
    fi
done

echo
echo "=== Cleaning IO directories ==="

for dir in NRNCS PGCS Source1 Repository1 IoTTestApp NBTestApp; do
    io_dir="$BASE/IO/$dir"
    if [ -d "$io_dir" ]; then
        count=$(find "$io_dir" -type f -not -name '*.ini' 2>/dev/null | wc -l)
        if [ "$count" -gt 0 ]; then
            echo "  $dir ($count files)"
            find "$io_dir" -type f -not -name '*.ini' -delete
        fi
    fi
done

echo
echo "=== Cleaning IPC resources ==="

# System V shared memory
IPCS_M=$(ipcs -m 2>/dev/null | awk -v user="$ME" '$3 == user {print $2}')
for id in $IPCS_M; do
    ipcrm -m "$id" 2>/dev/null && echo "  Removed shmid $id"
done

# System V semaphores
IPCS_S=$(ipcs -s 2>/dev/null | awk -v user="$ME" '$3 == user {print $2}')
for id in $IPCS_S; do
    ipcrm -s "$id" 2>/dev/null && echo "  Removed semid $id"
done

# System V message queues
IPCS_Q=$(ipcs -q 2>/dev/null | awk -v user="$ME" '$3 == user {print $2}')
for id in $IPCS_Q; do
    ipcrm -q "$id" 2>/dev/null && echo "  Removed msqid $id"
done

# Named POSIX semaphores (all /dev/shm/sem.*)
count=$(ls /dev/shm/sem.* 2>/dev/null | wc -l)
if [ "$count" -gt 0 ]; then
    echo "  Removing $count named semaphore(s) from /dev/shm..."
    rm -fv /dev/shm/sem.* 2>/dev/null
fi

echo
echo "=== Done ==="
