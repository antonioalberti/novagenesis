BASE=`cd ..; pwd`;


gnome-terminal --tab --title="PGS" -e "/bin/bash -c 'cd $BASE/PGCS/Debug;sh clean.sh;./PGCS $BASE/IO/PGCS/ -d Ethernet;exec bash'" \
--tab --title="Client1" -e "/bin/bash -c 'cd $BASE/App/Debug;sleep 120;./App $BASE/IO/Client1/ Client;exec bash'" 

