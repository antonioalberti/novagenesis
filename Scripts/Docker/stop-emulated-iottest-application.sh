#!/bin/sh

docker stop Core0

docker stop Client

docker stop EPGS

docker container prune -f
