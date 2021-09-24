BASE=`cd ..; pwd`;


gnome-terminal --tab -e "/bin/bash -c 'cd $BASE/PGCS/Debug;./PGCS $BASE/IO/PGCS/ 0 Intra_Domain -lc;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/GIRS/Debug;sleep 10;./GIRS $BASE/IO/GIRS/;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/HTS/Debug;sleep 10;./HTS $BASE/IO/HTS/;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/PSS/Debug;sleep 10;./PSS $BASE/IO/PSS/;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/APS/Debug;sleep 60;./APS APS01 $BASE/IO/APS01/ tcp://10.0.185.162:5555;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/SSS/Debug;sleep 60;./SSS SSS01 $BASE/IO/SSS01/ tcp://192.168.235.16:5555;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/RMS/Debug;sleep 80;./RMS RMS01 $BASE/IO/RMS/ tcp://10.0.185.163:5555;exec bash'"
