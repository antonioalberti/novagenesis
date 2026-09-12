#!/bin/sh

docker container prune -f

docker build -f Docker/NRNCS/Dockerfile -t ng-nrncs:latest .

docker build -f Docker/ContentApp/Dockerfile -t ng-contentapp:latest .

docker build -f Docker/NBTestApp/Dockerfile -t ng-nbtestapp:latest .

docker build -f Docker/IoTTestApp/Dockerfile -t ng-iottestapp:latest .

docker image prune -f
