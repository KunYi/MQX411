#!/usr/bin/env bash

status=0

$(cd ../../../mqx/build/make/bsp_vybrid_autoevb_m4 && ./build_ds5.sh nopause)
if [ "$?" != "0" ]; then
    status=-1
fi
$(cd ../../../mcc/build/make/mcc_vybrid_autoevb_m4 && ./build_ds5.sh nopause)
if [ "$?" != "0" ]; then
    status=-1
fi
$(cd ../../../mfs/build/make/mfs_vybrid_autoevb_m4 && ./build_ds5.sh nopause)
if [ "$?" != "0" ]; then
    status=-1
fi
$(cd ../../../mqx/build/make/psp_vybrid_autoevb_m4 && ./build_ds5.sh nopause)
if [ "$?" != "0" ]; then
    status=-1
fi
$(cd ../../../rtcs/build/make/rtcs_vybrid_autoevb_m4 && ./build_ds5.sh nopause)
if [ "$?" != "0" ]; then
    status=-1
fi
$(cd ../../../shell/build/make/shell_vybrid_autoevb_m4 && ./build_ds5.sh nopause)
if [ "$?" != "0" ]; then
    status=-1
fi
$(cd ../../../usb/device/build/make/usbd_vybrid_autoevb_m4 && ./build_ds5.sh nopause)
if [ "$?" != "0" ]; then
    status=-1
fi
$(cd ../../../usb/host/build/make/usbh_vybrid_autoevb_m4 && ./build_ds5.sh nopause)
if [ "$?" != "0" ]; then
    status=-1
fi

if [ "${1}" != "nopause" ]; then
read -p "Press any key to continue... " -n1 -s
fi

exit $status

