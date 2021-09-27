#!/bin/sh

docker run -itd --privileged -e CA_PATH="/home/ng/workspace/novagenesis/IO/IoTTestApp/" --name IoTTestApp ng-iottestapp:latest

echo "Creating IoTTestApp Input/Output folder"

docker exec -w /home/ng/workspace/novagenesis/IO/ IoTTestApp mkdir IoTTestApp

echo "Changing IoTTestApp Input/Output folder permissions"

docker exec -w /home/ng/workspace/novagenesis/IO/ IoTTestApp chmod -R 777 IoTTestApp

echo "Copying IoTTestApp.ini from local Includes folder to IoTTestApp Input/Output folder"

#Copy local IoTTestApp.ini to the container to set up Source app parameters
docker cp Includes-IoTTestApp/IoTTestApp.ini IoTTestApp:/home/ng/workspace/novagenesis/IO/IoTTestApp/
