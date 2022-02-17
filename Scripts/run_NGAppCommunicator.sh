#!/bin/bash

BASE=`cd ..; pwd`;

find $BASE/IO/NGAppCommunicator/ ! -name "*.ini" | xargs rm

#SLEEP='10'

gnome-terminal --tab -e "/bin/bash -c 'cd $BASE/cmake-build-debug;./NGAppCommunicator $NGBASE/IO/NGAppCommunicator/ Client;exec bash'"
