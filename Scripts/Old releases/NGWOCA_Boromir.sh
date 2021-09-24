BASE=`cd ..; pwd`;


gnome-terminal --tab --title="PGCS" -e "/bin/bash -c 'cd $BASE/PGCS/Debug;sh clean.sh;./PGCS $BASE/IO/PGCS/ -d Ethernet;exec bash'" \
--tab --title="RMS" -e "/bin/bash -c 'cd $BASE/RMS/Debug;sleep 120;gdb -ex run --args ./RMS RMS01 $BASE/IO/RMS/ tcp://10.0.185.163:5555;exec bash'"
