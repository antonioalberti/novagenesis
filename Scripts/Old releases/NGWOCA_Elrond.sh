BASE=`cd ..; pwd`;


gnome-terminal --tab --title="PGCS" -e "/bin/bash -c 'cd $BASE/PGCS/Debug;sh clean.sh;./PGCS $BASE/IO/PGCS/ -d Ethernet;exec bash'" \
--tab --title="GIRS" -e "/bin/bash -c 'cd $BASE/GIRS/Debug;sleep 10;./GIRS $BASE/IO/GIRS/;exec bash'" \
--tab --title="HTS" -e "/bin/bash -c 'cd $BASE/HTS/Debug;sleep 10;./HTS $BASE/IO/HTS/;exec bash'" \
--tab --title="PSS" -e "/bin/bash -c 'cd $BASE/PSS/Debug;sleep 10;./PSS $BASE/IO/PSS/;exec bash'" \
--tab --title="SSS01" -e "/bin/bash -c 'cd $BASE/SSS/Debug;sleep 120;gdb -ex run --args ./SSS SSS01 $BASE/IO/SSS01/ tcp://0.0.0.0:5555 tcp://192.168.60.113:6001 tcp://192.168.60.113:5055;exec bash'"
