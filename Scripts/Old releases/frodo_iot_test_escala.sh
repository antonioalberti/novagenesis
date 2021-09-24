BASE=`cd ..; pwd`;




gnome-terminal --tab --title="PGCS" -e "/bin/bash -c 'cd $BASE/PGCS/Debug;sh clean.sh;./PGCS $BASE/IO/PGCS/ 0 Intra_Domain -pc Ethernet Intra_Domain eno1 a4:ba:db:04:31:8a 1200;exec bash'" \
--tab --title="HTS" -e "/bin/bash -c 'cd $BASE/HTS/Debug;sleep 10;./HTS $BASE/IO/HTS/;exec bash'" \
--tab --title="GIRS" -e "/bin/bash -c 'cd $BASE/GIRS/Debug;sleep 10;./GIRS $BASE/IO/GIRS/;exec bash'" \
--tab --title="PSS" -e "/bin/bash -c 'cd $BASE/PSS/Debug;sleep 10;./PSS $BASE/IO/PSS/;exec bash'" \
--tab --title="IoTTestApp1" -e "/bin/bash -c 'cd $BASE/IoTTestApp/Debug;sleep 60;./IoTTestApp IoTTestApp1 $BASE/IO/IoTTestApp/;exec bash'" 
