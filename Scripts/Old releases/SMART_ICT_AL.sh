BASE=`cd ..; pwd`;


gnome-terminal --tab -e "/bin/bash -c 'cd $BASE/PGCS/Debug;./PGCS $BASE/IO/PGCS/ 9999 Intra_Domain -p ZMQ_TCP Intra_Domain enp9s0 192.168.235.17:8888 1200;exec bash'"
