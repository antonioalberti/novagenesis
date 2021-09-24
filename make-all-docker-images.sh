#!/bin/sh

sudo docker build -t ng-generic:latest .

sudo docker build -f Docker/NRNCS/Dockerfile -t ng-nrncs:latest .

sudo docker build -f Docker/ContentApp/Dockerfile -t ng-contentapp:latest .

sudo docker build -f Docker/NBTestApp/Dockerfile -t ng-nbtestapp:latest .

sudo docker image prune -f
