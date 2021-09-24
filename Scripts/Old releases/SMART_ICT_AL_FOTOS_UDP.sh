BASE=`cd ..; pwd`;


gnome-terminal --tab -e "/bin/bash -c 'cd $BASE/PGCS/Debug;./PGCS $BASE/IO/PGCS/ 8888 Intra_Domain -p IPv4_UDP Intra_Domain enp9s0 192.168.235.17:9999 1200;exec bash'" \
--tab --title="Client1" -e "/bin/bash -c 'cd $BASE/App/Debug;sleep 120;./App $BASE/IO/Client1/ Client;exec bash'" 
