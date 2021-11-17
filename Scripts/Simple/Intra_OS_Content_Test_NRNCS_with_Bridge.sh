BASE=`cd ../..; pwd`;

INTERFACE1="eth8"
MAC1="C8:D7:4A:4E:47:50"

INTERFACE2="eth9"
MAC2="C8:D7:4A:4E:47:51"

sudo ip link add $INTERFACE1 type veth peer name $INTERFACE2


sudo ip netns add ns1

sudo ip link add $INTERFACE1 netns ns1 type veth

sudo ifconfig $INTERFACE1 hw ether $MAC1

sudo ip link set $INTERFACE1 up



sudo ip netns add ns2

sudo ip link add $INTERFACE2 netns ns2 type veth

sudo ifconfig $INTERFACE2 hw ether $MAC2

sudo ip link set $INTERFACE2 up



#sudo ip netns exec ns1 ip link set $INTERFACE1 up

gnome-terminal --tab -e "/bin/bash -c 'cd $BASE/cmake-build-debug;./PGCS $BASE/IO/PGCS/ 0 Intra_Domain -p Ethernet Intra_Domain $INTERFACE1 $MAC2 1200;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/cmake-build-debug;sleep 2;./NRNCS $BASE/IO/NRNCS/;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/cmake-build-debug;sleep 2;./ContentApp $BASE/IO/Repository1/ Repository;exec bash'"

gnome-terminal --tab -e "/bin/bash -c 'cd $BASE/cmake-build-debug;./PGCS $BASE/IO/PGCS/ 0 Intra_Domain -p Ethernet Intra_Domain $INTERFACE2 $MAC1 1200;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/cmake-build-debug;sleep 2;./ContentApp $BASE/IO/Source1/ Source;exec bash'"



