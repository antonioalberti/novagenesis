#!/bin/bash

cd /home/ng/workspace/novagenesis/Build

# Required to avoid permission issues
chmod -R 777 /home/ng/workspace/novagenesis

# Create a shared memory segment
ipcmk -M 1024

# Diplay a shm report before running. NovaGenesis employs shm for inter container communication
ipcs -m

echo Starting PGCS 

echo Starting NBTestApp with parameters:

sh -c "echo $PATH"

# Run the PGCS and NBTestApp
/usr/bin/supervisord -c /home/ng/workspace/novagenesis/supervisord.conf
