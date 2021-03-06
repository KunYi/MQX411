#!/usr/bin/env bash

status=0


make TOOL=ds5 CONFIG=debug LINKER_FILE=$(MQX_ROOTDIR)/mqx/examples/bootloader_vybrid_qspixip/sram_ds5_m4.scf build -j3 
if [ "$?" != "0" ]; then
    status=-1
fi

make TOOL=ds5 CONFIG=release LINKER_FILE=$(MQX_ROOTDIR)/mqx/examples/bootloader_vybrid_qspixip/sram_ds5_m4.scf build -j3 
if [ "$?" != "0" ]; then
    status=-1
fi

if [ "${1}" != "nopause" ]; then
read -p "Press any key to continue... " -n1 -s
fi

exit $status

