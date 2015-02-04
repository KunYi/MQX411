#!/usr/bin/env bash

status=0


make TOOL=ds5 CONFIG=debug clean 
if [ "$?" != "0" ]; then
    status=-1
fi

make TOOL=ds5 CONFIG=release clean 
if [ "$?" != "0" ]; then
    status=-1
fi

if [ "${1}" != "nopause" ]; then
read -p "Press any key to continue... " -n1 -s
fi

exit $status

