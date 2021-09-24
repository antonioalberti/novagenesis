BASE=`cd ..; pwd`;


gnome-terminal --tab --title="PGCS" -e "/bin/bash -c 'cd $BASE/PGCS/Debug;./PGCS $BASE/IO/PGCS/ 0 Intra_Domain -p Ethernet Intra_Domain enp9s0 ac:22:0b:c9:df:3b 1200 Ethernet Intra_Domain enp9s0 ac:22:0b:c9:df:fd 1200;exec bash'" \
--tab --title="GIRS" -e "/bin/bash -c 'cd $BASE/GIRS/Debug;sleep 10;./GIRS $BASE/IO/GIRS/;exec bash'" \
--tab --title="PSS" -e "/bin/bash -c 'cd $BASE/PSS/Debug;sleep 10;./PSS $BASE/IO/PSS/;exec bash'" \
--tab --title="NBSimpleTestApp" -e "/bin/bash -c 'cd $BASE/NBSimpleTestApp/Debug;sleep 120;./NBSimpleTestApp $BASE/IO/NBSimpleTestApp/;exec bash'" 

