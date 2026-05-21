#!/bin/sh

sleep 170

docker run -itd --privileged --name EPGS$1 ng-epgs:latest
