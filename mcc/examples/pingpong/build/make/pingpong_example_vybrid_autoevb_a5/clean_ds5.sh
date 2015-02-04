#!/usr/bin/env bash

status=0


make TOOL=ds5 CONFIG=debug LOAD=ddr clean 
if [ "$?" != "0" ]; then
    status=-1
fi

make TOOL=ds5 CONFIG=release LOAD=ddr clean 
if [ "$?" != "0" ]; then
    status=-1
fi

if [ "${1}" != "nopause" ]; then
read -p "Press any key to continue... " -n1 -s
fi

exit $status

