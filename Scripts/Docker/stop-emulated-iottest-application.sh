#!/bin/sh

docker stop Client

docker container prune -f
