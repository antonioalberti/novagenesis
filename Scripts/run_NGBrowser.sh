#!/bin/bash

NGBASE=`cd ..;pwd`

gnome-terminal --title="NGBrowser" --tab -e "/bin/bash -c 'cd $NGBASE/NG_Web/build-NGBrowser-Desktop-Debug/;./NGBrowser;exec bash'" &
