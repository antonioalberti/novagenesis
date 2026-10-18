BASE=`cd ../..; pwd`;


gnome-terminal --tab -e "/bin/bash -c 'cd $BASE/cmake-build-debug;./PGCS $BASE/IO/PGCS/ 0 Intra_Domain -l;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/cmake-build-debug;sleep 2;./NRNCS $BASE/IO/NRNCS/;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/cmake-build-debug;sleep 10;./ContentApp $BASE/IO/Source1/ Source;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/cmake-build-debug;sleep 10;./ContentApp $BASE/IO/Source2/ Source;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/cmake-build-debug;sleep 10;./ContentApp $BASE/IO/Repository1/ Repository;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/cmake-build-debug;sleep 10;./ContentApp $BASE/IO/Repository2/ Repository;exec bash'" \
