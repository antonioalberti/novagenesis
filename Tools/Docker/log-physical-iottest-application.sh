#!/bin/sh

# Copy results from IoT Testing Application

docker exec Client cat /home/ng/workspace/novagenesis/supervisorPGCS.log > Results/0_Client_PGCS.txt

docker exec Client cat /home/ng/workspace/novagenesis/supervisorNRNCS.log > Results/0_Client_NRNCS.txt

docker exec Client cat /home/ng/workspace/novagenesis/supervisorIoTTestApp.log > Results/0_Client_IoTTestApp.txt

Path=$(pwd)

echo $Path

docker cp Client:/home/ng/workspace/novagenesis/IO/IoTTestApp/. $Path/Results/
