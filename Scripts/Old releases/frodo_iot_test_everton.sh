BASE=`cd ..; pwd`;


gnome-terminal --tab --title="IoTTestApp1" -e "/bin/bash -c 'cd $BASE/IoTTestApp/Debug;./IoTTestApp IoTTestApp1 $BASE/IO/IoTTestApp/;exec bash'"; 
