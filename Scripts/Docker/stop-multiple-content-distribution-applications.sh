#!/bin/sh

docker stop Core0

for i in $(seq 1 $1)
do 
echo "Stopping content repository number: $i" 

docker stop Repository$i

done

for j in $(seq 1 $2)
do 
echo "Stopping content source number: $j" 

docker stop Source$j

done

docker container prune -f
