#!/usr/bin/env bash

status=0


make TOOL=gcc_arm CONFIG=debug LOAD=intflash_ddrdata clean 
if [ "$?" != "0" ]; then
    status=-1
fi

make TOOL=gcc_arm CONFIG=debug LOAD=intflash_sramdata clean 
if [ "$?" != "0" ]; then
    status=-1
fi

make TOOL=gcc_arm CONFIG=release LOAD=intflash_ddrdata clean 
if [ "$?" != "0" ]; then
    status=-1
fi

make TOOL=gcc_arm CONFIG=release LOAD=intflash_sramdata clean 
if [ "$?" != "0" ]; then
    status=-1
fi

if [ "${1}" != "nopause" ]; then
read -p "Press any key to continue... " -n1 -s
fi

exit $status

