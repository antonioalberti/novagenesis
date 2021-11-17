BASE=`cd ../..; pwd`;

INTERFACE1="eth8"
MAC1="00:00:00:00:00:01"

INTERFACE2="eth9"
MAC1="00:00:00:00:00:02"

sudo ip link add $INTERFACE1 type veth peer name $INTERFACE2


sudo ip netns add ns1

sudo ip link add $INTERFACE1 netns ns1 type veth

sudo ifconfig $INTERFACE1 hw ether $MAC1

sudo ip link set $INTERFACE1 up


sudo ip netns add ns2

sudo ip link add $INTERFACE2 netns ns2 type veth

sudo ifconfig $INTERFACE2 hw ether $MAC2

sudo ip link set $INTERFACE2 up


gnome-terminal --tab -e "/bin/bash -c 'cd $BASE/cmake-build-debug;./PGCS $BASE/IO/PGCS/ 0 Intra_Domain -pc Ethernet Intra_Domain $INTERFACE1 $MAC2 1200;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/cmake-build-debug;sleep 2;./NRNCS $BASE/IO/NRNCS/;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/cmake-build-debug;sleep 20;./IoTTestApp Client $BASE/IO/IoTTestApp/;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/cmake-build-debug;sleep 180;./EPGS -i $INTERFACE2;exec bash'"
