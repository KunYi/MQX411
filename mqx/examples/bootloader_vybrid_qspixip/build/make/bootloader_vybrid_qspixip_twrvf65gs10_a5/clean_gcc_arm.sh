#!/usr/bin/env bash

status=0


make TOOL=gcc_arm CONFIG=debug LINKER_FILE=$(MQX_ROOTDIR)/mqx/examples/bootloader_vybrid_qspixip/sram_gcc_arm_a5.ld clean 
if [ "$?" != "0" ]; then
    status=-1
fi

make TOOL=gcc_arm CONFIG=release LINKER_FILE=$(MQX_ROOTDIR)/mqx/examples/bootloader_vybrid_qspixip/sram_gcc_arm_a5.ld clean 
if [ "$?" != "0" ]; then
    status=-1
fi

if [ "${1}" != "nopause" ]; then
read -p "Press any key to continue... " -n1 -s
fi

exit $status

