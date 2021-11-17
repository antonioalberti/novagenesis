#!/bin/sh

docker stop Client

docker stop EPGS

docker container prune -f
