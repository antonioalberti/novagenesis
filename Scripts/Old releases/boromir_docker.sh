BASE=`cd ..; pwd`;


gnome-terminal --tab --title="PGCS" -e "/bin/bash -c 'cd $BASE/PGCS/Debug;sh clean.sh;gdb -ex run --args ./PGCS /home/ng/workspace/novagenesis/IO/PGCS/ 0 Intra_Domain -p Ethernet Intra_Domain em1 e0:db:55:a1:cb:d2 1200 Ethernet Intra_Domain veth69f21f6 02:42:ac:11:00:01 1200 Ethernet Intra_Domain vethc5ed7bc 02:42:ac:11:00:02 1200;exec bash'"

