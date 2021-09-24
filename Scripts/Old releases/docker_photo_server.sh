BASE=`cd ..; pwd`;


gnome-terminal --tab --title="PGS" -e "/bin/bash -c 'cd $BASE/PGCS/Debug;sh clean.sh;gdb -ex run --args ./PGCS $BASE/IO/PGCS/ -d Ethernet;exec bash'" \
--tab --title="HTS" -e "/bin/bash -c 'cd $BASE/HTS/Debug;sleep 10;./HTS $BASE/IO/HTS/;exec bash'" \
--tab --title="GIRS" -e "/bin/bash -c 'cd $BASE/GIRS/Debug;sleep 10;./GIRS $BASE/IO/GIRS/;exec bash'" \
--tab --title="PSS" -e "/bin/bash -c 'cd $BASE/PSS/Debug;sleep 10;./PSS $BASE/IO/PSS/;exec bash'" \
--tab --title="Server" -e "/bin/bash -c 'cd $BASE/App/Debug;sleep 120;./App $BASE/IO/Server/ Server;exec bash'" 


