#!/bin/sh

sudo docker container prune -f

sudo docker rmi ng-generic

sudo docker rmi ng-nrncs

sudo docker rmi ng-contentapp

sudo docker rmi ng-iottestapp

sudo docker rmi ng-nbtestapp

sudo docker rmi ng-epgs

