#!/bin/sh

# Copy results from IoT Testing Application

sudo docker exec Client cat /home/ng/workspace/novagenesis/supervisorPGCS.log > 0_Client_PGCS.txt

sudo docker exec Client cat /home/ng/workspace/novagenesis/supervisorIoTTestApp.log > 0_Client_IoTTestApp.txt

# Copy results from EPGS

sudo docker exec EPGS cat /home/ng/workspace/novagenesis/supervisorEPGS.log > 0_EPGS.txt

Path=$(pwd)

echo $Path

sudo docker cp Client:/home/ng/workspace/novagenesis/IO/IoTTestApp/. $Path

# Finally, print data from core NRNCS
sudo docker exec Core0 cat /home/ng/workspace/novagenesis/supervisorPGCS.log > 0_Core0_PGCS.txt

sudo docker exec Core0 cat /home/ng/workspace/novagenesis/supervisorNRNCS.log > 0_Core0_NRNCS.txt
