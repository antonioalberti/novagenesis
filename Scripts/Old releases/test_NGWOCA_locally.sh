BASE=`cd ..; pwd`;


gnome-terminal --tab --title="PGCS" -e "/bin/bash -c 'cd $BASE/PGCS/Debug;sh clean.sh;./PGCS $BASE/IO/PGCS/ 0 Intra_Domain -l;exec bash'" \
--tab --title="GIRS" -e "/bin/bash -c 'cd $BASE/GIRS/Debug;sleep 10;./GIRS $BASE/IO/GIRS/;exec bash'" \
--tab --title="HTS" -e "/bin/bash -c 'cd $BASE/HTS/Debug;sleep 10;./HTS $BASE/IO/HTS/;exec bash'" \
--tab --title="PSS" -e "/bin/bash -c 'cd $BASE/PSS/Debug;sleep 10;./PSS $BASE/IO/PSS/;exec bash'" \
--tab --title="AAS01" -e "/bin/bash -c 'cd $BASE/AAS/Debug;sleep 120;./AAS AAS01 $BASE/IO/AAS01/ tcp://10.0.185.162:5555;exec bash'" \
--tab --title="APS01" -e "/bin/bash -c 'cd $BASE/APS/Debug;sleep 120;./APS APS01 $BASE/IO/APS01/ tcp://10.0.185.162:5555;exec bash'" \
--tab --title="APS02" -e "/bin/bash -c 'cd $BASE/APS/Debug;sleep 120;./APS APS02 $BASE/IO/APS02/ tcp://10.0.185.162:5555;exec bash'" \
--tab --title="SSS01" -e "/bin/bash -c 'cd $BASE/SSS/Debug;sleep 120;./SSS SSS01 $BASE/IO/SSS01/ tcp://0.0.0.0:5555 tcp://192.168.60.170:6001 tcp://192.168.60.170:5055;exec bash'" \
--tab --title="POXS01" -e "/bin/bash -c 'cd $BASE/POXS/Debug;sleep 120;./POXS POXS01 $BASE/IO/POXS01/ tcp://192.168.60.218:5555;exec bash'" \
--tab --title="RMS" -e "/bin/bash -c 'cd $BASE/RMS/Debug;sleep 160;gdb -ex run --args ./RMS RMS01 $BASE/IO/RMS/ tcp://10.0.185.163:5555;exec bash'"
