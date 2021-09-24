BASE=`cd ..; pwd`;


gnome-terminal --tab -e "/bin/bash -c 'cd $BASE/PGCS/Debug;./PGCS $BASE/IO/PGCS/ 8888 Intra_Domain -pc IPv4_UDP Intra_Domain enp9s0 192.168.235.17:9999 1200 Wi-Fi Intra_Domain wlp7s0 00:23:a7:23:06:b2 1200 Wi-Fi Intra_Domain wlp7s0 00:23:a7:23:06:66 1200;exec bash'"
