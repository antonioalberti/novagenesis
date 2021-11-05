#!/bin/sh

gnome-terminal --tab -e "/bin/bash -c 'sudo docker exec Core0 supervisorctl tail -f PGCS;exec bash'" \
gnome-terminal --tab -e "/bin/bash -c 'sudo docker exec Core0 supervisorctl tail -f NRNCS;exec bash'" \
gnome-terminal --tab -e "/bin/bash -c 'sudo docker exec Source1 supervisorctl tail -f PGCS;exec bash'" \
gnome-terminal --tab -e "/bin/bash -c 'sudo docker exec Source1 supervisorctl tail -f ContentApp;exec bash'" \
gnome-terminal --tab -e "/bin/bash -c 'sudo docker exec Repository1 supervisorctl tail -f PGCS;exec bash'" \
gnome-terminal --tab -e "/bin/bash -c 'sudo docker exec Repository1 supervisorctl tail -f ContentApp;exec bash'" \
gnome-terminal --tab -e "/bin/bash -c 'sudo watch -n 1 docker exec Core0 ls -a /home/ng/workspace/novagenesis/IO/NRNCS;exec bash'" \
gnome-terminal --tab -e "/bin/bash -c 'sudo watch -n 1 docker exec Source1 ls -a /home/ng/workspace/novagenesis/IO/Source1;exec bash'" \
gnome-terminal --tab -e "/bin/bash -c 'sudo watch -n 1 docker exec Repository1 ls -a /home/ng/workspace/novagenesis/IO/Repository1;exec bash'" 
