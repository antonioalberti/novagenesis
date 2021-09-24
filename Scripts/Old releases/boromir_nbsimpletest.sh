BASE=`cd ..; pwd`;


gnome-terminal --tab --title="PGCS" -e "/bin/bash -c 'cd $BASE/PGCS/Debug;sh clean.sh;nice -n 19 ./PGCS $BASE/IO/PGCS/ 0 Intra_Domain -p Ethernet Intra_Domain em1 ac:22:0b:c9:df:3b 1200;exec bash'" \
--tab --title="NBSimpleTestApp" -e "/bin/bash -c 'cd $BASE/NBSimpleTestApp/Debug;sleep 120;nice -n 19 ./NBSimpleTestApp $BASE/IO/NBSimpleTestApp/;exec bash'" 

