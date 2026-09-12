#!/bin/sh

# Copy results from IoT Testing Application

docker exec Client cat /home/ng/workspace/novagenesis/supervisorPGCS.log > Results/0_Client_PGCS.txt

docker exec Client cat /home/ng/workspace/novagenesis/supervisorNRNCS.log > Results/0_Client_NRNCS.txt

docker exec Client ls /home/ng/workspace/novagenesis/IO/NRNCS > Results/0_Client_NRNCS_CACHE_CONTENT_LIST.txt

docker exec Client ls /home/ng/workspace/novagenesis/IO/IoTTestApp/ > Results/0_Client_IoTTestApp_CACHE_CONTENT_LIST.txt

docker exec Client cat /home/ng/workspace/novagenesis/supervisorIoTTestApp.log > Results/0_Client_IoTTestApp.txt

docker exec Client ps -a > Results/0_Client_PROCESSES_RUNNING_LIST.txt

Path=$(pwd)

echo $Path

docker cp Client:/home/ng/workspace/novagenesis/IO/IoTTestApp/. $Path/Results/

docker cp Client:/home/ng/workspace/novagenesis/IO/PGCS/. $Path/Results/
