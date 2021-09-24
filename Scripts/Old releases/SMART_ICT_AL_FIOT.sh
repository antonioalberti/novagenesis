BASE=`cd ..; pwd`;


gnome-terminal --tab -e "/bin/bash -c 'cd $BASE/PGCS/Debug;./PGCS $BASE/IO/PGCS/ 8888 Intra_Domain -pc ZMQ_TCP Intra_Domain enp9s0 192.168.235.17:9999 1200;exec bash'"
