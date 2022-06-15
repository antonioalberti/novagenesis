#!/bin/sh

sudo docker tag ng-nrncs antonioalberti/ng-nrncs:june2022

sudo docker tag ng-contentapp antonioalberti/ng-contentapp:june2022

sudo docker tag ng-nbtestapp antonioalberti/ng-nbtestapp:june2022

sudo docker tag ng-iottestapp antonioalberti/ng-iottestapp:june2022

sudo docker tag ng-epgs antonioalberti/ng-epgs:june2022

sudo docker push antonioalberti/ng-nrncs:june2022

sudo docker push antonioalberti/ng-contentapp:june2022

sudo docker push antonioalberti/ng-nbtestapp:june2022

sudo docker push antonioalberti/ng-iottestapp:june2022

sudo docker push antonioalberti/ng-epgs:june2022



