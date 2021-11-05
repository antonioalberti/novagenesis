#!/bin/sh

sudo docker container prune -f

sudo docker build -t ng-generic:latest .

sudo docker build -f Docker/NRNCS/Dockerfile -t ng-nrncs:latest .

sudo docker build -f Docker/ContentApp/Dockerfile -t ng-contentapp:latest .

sudo docker build -f Docker/NBTestApp/Dockerfile -t ng-nbtestapp:latest .

sudo docker build -f Docker/IoTTestApp/Dockerfile -t ng-iottestapp:latest .

sudo docker build -f Docker/EPGS/Dockerfile -t ng-epgs:latest .

sudo docker image prune -f
