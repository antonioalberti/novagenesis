#!/bin/sh

docker stop Core0

docker stop Client

docker container prune -f
