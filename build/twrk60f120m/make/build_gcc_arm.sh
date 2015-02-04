#!/usr/bin/env bash

status=0

$(cd ../../../mqx/build/make/bsp_twrk60f120m && ./build_gcc_arm.sh nopause)
if [ "$?" != "0" ]; then
    status=-1
fi
$(cd ../../../ffs/build/make/ffs_twrk60f120m && ./build_gcc_arm.sh nopause)
if [ "$?" != "0" ]; then
    status=-1
fi
$(cd ../../../mfs/build/make/mfs_twrk60f120m && ./build_gcc_arm.sh nopause)
if [ "$?" != "0" ]; then
    status=-1
fi
$(cd ../../../mqx/build/make/psp_twrk60f120m && ./build_gcc_arm.sh nopause)
if [ "$?" != "0" ]; then
    status=-1
fi
$(cd ../../../rtcs/build/make/rtcs_twrk60f120m && ./build_gcc_arm.sh nopause)
if [ "$?" != "0" ]; then
    status=-1
fi
$(cd ../../../shell/build/make/shell_twrk60f120m && ./build_gcc_arm.sh nopause)
if [ "$?" != "0" ]; then
    status=-1
fi
$(cd ../../../usb/device/build/make/usbd_twrk60f120m && ./build_gcc_arm.sh nopause)
if [ "$?" != "0" ]; then
    status=-1
fi
$(cd ../../../usb/host/build/make/usbh_twrk60f120m && ./build_gcc_arm.sh nopause)
if [ "$?" != "0" ]; then
    status=-1
fi

if [ "${1}" != "nopause" ]; then
read -p "Press any key to continue... " -n1 -s
fi

exit $status

