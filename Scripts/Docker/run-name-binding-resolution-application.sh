#!/bin/sh

docker container prune -f

echo "Creating NRNCS container, which stands for Name Resolution and Network Cache Service" 

sh run-NRNCS.sh 0

echo "Creating name binding testing application" 

sh run-NBTestApp.sh
