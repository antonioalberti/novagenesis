#!/bin/sh

# Copy results from Name Binding Testing Application

sudo docker exec NBTestApp cat /home/ng/workspace/novagenesis/supervisorPGCS.log > Results/0_NBTestApp_PGCS.txt

sudo docker exec NBTestApp cat /home/ng/workspace/novagenesis/supervisorNBTestApp.log > Results/0_NBTestApp.txt

Path=$(pwd)

echo $Path

sudo docker cp NBTestApp:/home/ng/workspace/novagenesis/IO/NBTestApp/. $Path/Results/

# Finally, print data from core NRNCS
sudo docker exec Core0 cat /home/ng/workspace/novagenesis/supervisorPGCS.log > Results/0_Core0_PGCS.txt

sudo docker exec Core0 cat /home/ng/workspace/novagenesis/supervisorNRNCS.log > Results/0_Core0_NRNCS.txt
