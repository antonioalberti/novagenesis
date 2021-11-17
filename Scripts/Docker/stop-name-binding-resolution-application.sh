#!/bin/sh

docker stop Core0

docker stop NBTestApp

docker container prune -f
