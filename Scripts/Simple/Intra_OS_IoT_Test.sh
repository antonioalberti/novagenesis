BASE=`cd ../..; pwd`;


gnome-terminal --tab -e "/bin/bash -c 'cd $BASE/cmake-build-debug;./PGCS $BASE/IO/PGCS/ 0 Intra_Domain -pc Ethernet Intra_Domain eth0 FF:FF:FF:FF:FF:FF 1200;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/cmake-build-debug;sleep 2;./NRNCS $BASE/IO/NRNCS/;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/cmake-build-debug;sleep 20;./IoTTestApp Client $BASE/IO/IoTTestApp/;exec bash'" 

