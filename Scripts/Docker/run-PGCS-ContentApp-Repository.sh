#!/bin/sh

sudo docker run -itd --privileged -e CA_PATH="/home/ng/workspace/novagenesis/IO/Repository"$1"/" -e CA_TYPE="Repository" --name Repository$1 ng-contentapp:latest

echo "Creating Repository$1 Input/Output folder"

docker exec -w /home/ng/workspace/novagenesis/IO/ Repository$1 mkdir Repository$1

echo "Changing Repository$1 Input/Output folder permissions"

docker exec -w /home/ng/workspace/novagenesis/IO/ Repository$1 chmod -R 777 Repository$1

echo "Copying App.ini from local Includes folder to Repository$1 Input/Output folder"

#Copy local App.ini to the container to set up Repository app parameters
docker cp Includes-ContentApp/App.ini Repository$1:/home/ng/workspace/novagenesis/IO/Repository$1/
