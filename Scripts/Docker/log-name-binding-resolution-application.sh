#!/bin/sh

# Copy results from Name Binding Testing Application

docker exec NBTestApp cat /home/ng/workspace/novagenesis/supervisorPGCS.log > Results/0_NBTestApp_PGCS.txt

docker exec NBTestApp cat /home/ng/workspace/novagenesis/supervisorNBTestApp.log > Results/0_NBTestApp.txt

Path=$(pwd)

echo $Path

docker cp NBTestApp:/home/ng/workspace/novagenesis/IO/NBTestApp/. $Path/Results/

# Finally, print data from core NRNCS
docker exec Core0 cat /home/ng/workspace/novagenesis/supervisorPGCS.log > Results/0_Core0_PGCS.txt

docker exec Core0 cat /home/ng/workspace/novagenesis/supervisorNRNCS.log > Results/0_Core0_NRNCS.txt
