#!/bin/sh

gnome-terminal --tab -e "/bin/bash -c 'sudo docker exec Core0 supervisorctl tail -f PGCS;exec bash'" \
gnome-terminal --tab -e "/bin/bash -c 'sudo docker exec Core0 supervisorctl tail -f NRNCS;exec bash'" \
gnome-terminal --tab -e "/bin/bash -c 'sudo docker exec Client supervisorctl tail -f PGCS;exec bash'" \
gnome-terminal --tab -e "/bin/bash -c 'sudo docker exec Client supervisorctl tail -f IoTTestApp;exec bash'" \
gnome-terminal --tab -e "/bin/bash -c 'sudo docker exec EPGS supervisorctl tail -f EPGS;exec bash'" 
