BASE=`cd ..; pwd`;


gnome-terminal --tab -e "/bin/bash -c 'cd $BASE/PGCS/Debug;./PGCS $BASE/IO/PGCS/ 0 Intra_Domain -pc Ethernet Intra_Domain wlp7s0 3c:52:82:99:18:17 1200;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/APS/Debug;sleep 60;./APS APS01 $BASE/IO/APS01/ tcp://192.168.1.1:5555;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/SSS/Debug;sleep 60;./SSS SSS01 $BASE/IO/SSS01/ tcp://192.168.1.1:5555;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/RMS/Debug;sleep 80;./RMS RMS01 $BASE/IO/RMS/ tcp://192.168.1.1:5555;exec bash'"
