BASE=`cd ..; pwd`;


gnome-terminal --tab -e "/bin/bash -c 'cd $BASE/PGCS/Debug;./PGCS $BASE/IO/PGCS/ 0 Intra_Domain -l;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/HTS/Debug;sleep 2;./HTS $BASE/IO/HTS/;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/GIRS/Debug;sleep 2;./GIRS $BASE/IO/GIRS/;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/PSS/Debug;sleep 2;./PSS $BASE/IO/PSS/;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/App/Debug;sleep 10;./App $BASE/IO/Server/ Server;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/App/Debug;sleep 10;./App $BASE/IO/Client1/ Client;exec bash'" 
