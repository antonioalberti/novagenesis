#!/bin/sh

docker run -itd --privileged -e CA_PATH="/home/ng/workspace/novagenesis/IO/NBTestApp/" --name NBTestApp ng-nbtestapp:latest

echo "Creating NBTestApp Input/Output folder"

docker exec -w /home/ng/workspace/novagenesis/IO/ NBTestApp mkdir NBTestApp

echo "Changing NBTestApp Input/Output folder permissions"

docker exec -w /home/ng/workspace/novagenesis/IO/ NBTestApp chmod -R 777 NBTestApp

echo "Copying App.ini from local Includes folder to NBTestApp Input/Output folder"

#Copy local App.ini to the container to set up Source app parameters
docker cp Includes-NBTestApp/App.ini NBTestApp:/home/ng/workspace/novagenesis/IO/NBTestApp/
