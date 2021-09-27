#!/bin/sh

sudo docker run -itd --privileged --name EPGS$1 ng-epgs:latest
