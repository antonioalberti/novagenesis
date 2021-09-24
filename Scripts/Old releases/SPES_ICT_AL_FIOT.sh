BASE=`cd ..; pwd`;


gnome-terminal --tab -e "/bin/bash -c 'cd $BASE/PGCS/Debug;./PGCS $BASE/IO/PGCS/ 9999 Intra_Domain -pc ZMQ_TCP Intra_Domain enp9s0 192.168.235.18:8888 1200;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/GIRS/Debug;sleep 10;./GIRS $BASE/IO/GIRS/;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/HTS/Debug;sleep 10;./HTS $BASE/IO/HTS/;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/PSS/Debug;sleep 10;./PSS $BASE/IO/PSS/;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/IoTTestApp/Debug;./IoTTestApp IoTTestApp1 $BASE/IO/IoTTestApp/;exec bash'"; 

