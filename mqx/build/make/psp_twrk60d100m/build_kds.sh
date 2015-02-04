#!/usr/bin/env bash

status=0


make TOOL=kds CONFIG=debug build -j3 
if [ "$?" != "0" ]; then
    status=-1
fi

make TOOL=kds CONFIG=release build -j3 
if [ "$?" != "0" ]; then
    status=-1
fi

if [ "${1}" != "nopause" ]; then
read -p "Press any key to continue... " -n1 -s
fi

exit $status

