BASE=`cd ..; pwd`;


gnome-terminal --tab --title="PGCS" -e "/bin/bash -c 'cd $BASE/PGCS/Debug;sh clean.sh;nice -n -19 ./PGCS $BASE/IO/PGCS/ 0 Intra_Domain -p Ethernet Intra_Domain em1 ac:22:0b:c9:df:fd 1200;exec bash'" \
--tab --title="HTS" -e "/bin/bash -c 'cd $BASE/HTS/Debug;sleep 10;nice -n 19 ./HTS $BASE/IO/HTS/;exec bash'" \
--tab --title="GIRS" -e "/bin/bash -c 'cd $BASE/GIRS/Debug;sleep 10;nice -n -19 ./GIRS $BASE/IO/GIRS/;exec bash'" \
--tab --title="PSS" -e "/bin/bash -c 'cd $BASE/PSS/Debug;sleep 10;nice -n -19 ./PSS $BASE/IO/PSS/;exec bash'" 


