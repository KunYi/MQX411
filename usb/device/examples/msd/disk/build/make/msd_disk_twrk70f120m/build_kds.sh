#!/usr/bin/env bash

status=0


make TOOL=kds CONFIG=debug LOAD=intflash_ddrdata build -j3 
if [ "$?" != "0" ]; then
    status=-1
fi

make TOOL=kds CONFIG=debug LOAD=intflash_sramdata build -j3 
if [ "$?" != "0" ]; then
    status=-1
fi

make TOOL=kds CONFIG=release LOAD=intflash_ddrdata build -j3 
if [ "$?" != "0" ]; then
    status=-1
fi

make TOOL=kds CONFIG=release LOAD=intflash_sramdata build -j3 
if [ "$?" != "0" ]; then
    status=-1
fi

if [ "${1}" != "nopause" ]; then
read -p "Press any key to continue... " -n1 -s
fi

exit $status

