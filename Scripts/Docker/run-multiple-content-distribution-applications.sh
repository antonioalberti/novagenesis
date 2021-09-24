#!/bin/sh

docker container prune -f

echo "Creating NRNCS container, which stands for Name Resolution and Network Cache Service" 

sh run-PGCS-NRNCS.sh 0

# Loop to create the repository applications
# $1 is the number of content repository apps
for i in $(seq 1 $1)
do 
echo "Creating content repository number: $i" 

sh run-PGCS-ContentApp-Repository.sh $i

done

# Loop to create the repository applications
# $2 is the number of content source apps
for j in $(seq 1 $2)
do 
echo "Creating content source number: $j" 

# $3 paramenter is the amount of photos to be created in Source $j
# $4 paramenter is the width of the photos to be created in Source $j
# $5 paramenter is the height of the photos to be created in Source $j

sh run-PGCS-ContentApp-Source.sh $j $3 $4 $5

echo "Finished creating containers" 

done

