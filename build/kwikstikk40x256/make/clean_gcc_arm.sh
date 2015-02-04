#!/usr/bin/env bash

status=0

$(cd ../../../mqx/build/make/bsp_kwikstikk40x256 && ./clean_gcc_arm.sh nopause)
if [ "$?" != "0" ]; then
    status=-1
fi
$(cd ../../../mfs/build/make/mfs_kwikstikk40x256 && ./clean_gcc_arm.sh nopause)
if [ "$?" != "0" ]; then
    status=-1
fi
$(cd ../../../mqx/build/make/psp_kwikstikk40x256 && ./clean_gcc_arm.sh nopause)
if [ "$?" != "0" ]; then
    status=-1
fi
$(cd ../../../rtcs/build/make/rtcs_kwikstikk40x256 && ./clean_gcc_arm.sh nopause)
if [ "$?" != "0" ]; then
    status=-1
fi
$(cd ../../../shell/build/make/shell_kwikstikk40x256 && ./clean_gcc_arm.sh nopause)
if [ "$?" != "0" ]; then
    status=-1
fi
$(cd ../../../usb/device/build/make/usbd_kwikstikk40x256 && ./clean_gcc_arm.sh nopause)
if [ "$?" != "0" ]; then
    status=-1
fi
$(cd ../../../usb/host/build/make/usbh_kwikstikk40x256 && ./clean_gcc_arm.sh nopause)
if [ "$?" != "0" ]; then
    status=-1
fi

if [ "${1}" != "nopause" ]; then
read -p "Press any key to continue... " -n1 -s
fi

exit $status

