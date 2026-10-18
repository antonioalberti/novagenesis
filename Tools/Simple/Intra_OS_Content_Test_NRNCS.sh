BASE=`cd ../..; pwd`;


gnome-terminal --tab -e "/bin/bash -c 'cd $BASE/cmake-build-debug;./PGCS $BASE/IO/PGCS/ 0 Intra_Domain -lc;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/cmake-build-debug;sleep 2;./NRNCS $BASE/IO/NRNCS/;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/cmake-build-debug;sleep 2;./ContentApp $BASE/IO/Repository1/ Repository;exec bash'" \
--tab -e "/bin/bash -c 'cd $BASE/cmake-build-debug;sleep 2;./ContentApp $BASE/IO/Source1/ Source;exec bash'"


