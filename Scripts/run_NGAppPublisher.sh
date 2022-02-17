#!/bin/bash

BASE=`cd ..; pwd`;
#SLEEP='10'

gnome-terminal --tab -e "/bin/bash -c 'cd $BASE/cmake-build-debug;./NGAppPublisher $BASE/IO/NGAppPublisher/ Client;exec bash'"