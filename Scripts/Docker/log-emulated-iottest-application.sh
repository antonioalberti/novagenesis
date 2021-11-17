#!/bin/sh

# Copy results from IoT Testing Application

sudo docker exec Client cat /home/ng/workspace/novagenesis/supervisorPGCS.log > Results/0_Client_PGCS.txt

sudo docker exec Client cat /home/ng/workspace/novagenesis/supervisorNRNCS.log > Results/0_Client_NRNCS.txt

sudo docker exec Client ls /home/ng/workspace/novagenesis/IO/NRNCS > Results/0_Client_NRNCS_CACHE_CONTENT_LIST.txt

sudo docker exec Client ls /home/ng/workspace/novagenesis/IO/IoTTestApp/ > Results/0_Client_IoTTestApp_CACHE_CONTENT_LIST.txt

sudo docker exec Client cat /home/ng/workspace/novagenesis/supervisorIoTTestApp.log > Results/0_Client_IoTTestApp.txt

sudo docker exec Client ps -a > Results/0_Client_PROCESSES_RUNNING_LIST.txt

# Copy results from EPGS

sudo docker exec EPGS cat /home/ng/workspace/novagenesis/supervisorEPGS.log > Results/0_EPGS.txt

Path=$(pwd)

echo $Path

sudo docker cp Client:/home/ng/workspace/novagenesis/IO/IoTTestApp/. $Path/Results/

sudo docker cp Client:/home/ng/workspace/novagenesis/IO/PGCS/. $Path/Results/
