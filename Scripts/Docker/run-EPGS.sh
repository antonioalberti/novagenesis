#!/bin/sh

sleep 170

sudo docker run -itd --privileged --name EPGS$1 ng-epgs:latest
