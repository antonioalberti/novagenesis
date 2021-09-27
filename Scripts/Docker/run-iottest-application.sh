#!/bin/sh

docker container prune -f

echo "Creating NRNCS container, which stands for Name Resolution and Network Cache Service" 

sh run-PGCS-NRNCS.sh 0

echo "Creating IoT testing application" 

sh run-PGCS-IoTTestApp.sh

echo "Creating EPGS" 

sh run-EPGS.sh
