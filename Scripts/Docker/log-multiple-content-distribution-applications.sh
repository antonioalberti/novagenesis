#!/bin/sh

# Loop to log data from the repository applications
# $1 is the number of content repository apps
for i in $(seq 1 $1)
do 
echo "Logging data from the content repository number: $i" 

sudo docker exec Repository$i cat /home/ng/workspace/novagenesis/supervisorPGCS.log > 0_Repository${i}_PGCS.txt

sudo docker exec Repository$i cat /home/ng/workspace/novagenesis/supervisorContentApp.log > 0_Repository${i}_ContentApp.txt

sudo docker exec Repository$i ls /home/ng/workspace/novagenesis/IO/Repository$i | wc -l > 0_Repository${i}_Amount_Received.txt

sudo docker exec Repository$i ls /home/ng/workspace/novagenesis/IO/Repository$i > 0_Repository${i}_Name_of_Received_Files.txt

sudo docker exec Repository$i cat /home/ng/workspace/novagenesis/IO/Repository$i/ContentApp_Core_subrtt.dat > 0_Repository${i}_ContentApp_Core_subrtt.dat

sudo docker exec Repository$i cat /home/ng/workspace/novagenesis/IO/Repository$i/ContentApp_Core_pubrtt.dat > 0_Repository${i}_ContentApp_Core_pubrtt.dat

done

# Loop to log data from the source applications
# $2 is the number of content source apps
for j in $(seq 1 $2)
do 
echo "Logging data from the content source number: $j" 

sudo docker exec Source$j cat /home/ng/workspace/novagenesis/supervisorPGCS.log > 0_Source${j}_PGCS.txt

sudo docker exec Source$j cat /home/ng/workspace/novagenesis/supervisorContentApp.log > 0_Source${j}_ContentApp.txt

sudo docker exec Source$j ls /home/ng/workspace/novagenesis/IO/Source$j | wc -l > 0_Source${j}_Amount_Received.txt

sudo docker exec Source$j ls /home/ng/workspace/novagenesis/IO/Source$j > 0_Source${j}_Name_of_Received_Files.txt

sudo docker exec Source$j cat /home/ng/workspace/novagenesis/IO/Source$j/ContentApp_Core_subrtt.dat > 0_Source${j}_ContentApp_Core_subrtt.dat

sudo docker exec Source$j cat /home/ng/workspace/novagenesis/IO/Source$j/ContentApp_Core_pubrtt.dat > 0_Source${j}_ContentApp_Core_pubrtt.dat

done

# Finally, print data from core NRNCS


sudo docker exec Core0 cat /home/ng/workspace/novagenesis/supervisorPGCS.log > 0_Core0_PGCS.txt

sudo docker exec Core0 cat /home/ng/workspace/novagenesis/supervisorNRNCS.log > 0_Core0_NRNCS.txt

sudo docker exec Core0 ls /home/ng/workspace/novagenesis/IO/NRNCS | wc -l > 0_NRNCS_Amount_Received.txt



