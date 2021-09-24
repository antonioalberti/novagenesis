BASE=`cd ..; pwd`;


gnome-terminal --tab --title="PGCS" -e "/bin/bash -c 'cd $BASE/PGCS/Debug;sh clean.sh;./PGCS /home/ng/workspace/novagenesis/IO/PGCS/ 0 Intra_Domain -p Ethernet Intra_Domain em1 e0:db:55:a1:cb:d2 1200 Ethernet Intra_Domain docker0 02:42:05:a0:25:7e 1200;exec bash'" \
--tab --title="GIRS" -e "/bin/bash -c 'cd $BASE/GIRS/Debug;sleep 10;./GIRS $BASE/IO/GIRS/;exec bash'" 

