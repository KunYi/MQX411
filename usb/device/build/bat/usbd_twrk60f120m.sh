#!/usr/bin/env bash

# expect forward slash paths
ROOTDIR="${1}"
OUTPUTDIR="${2}"
TOOL="${3}"


# copy common files
mkdir -p "${OUTPUTDIR}"
mkdir -p "${OUTPUTDIR}/"
cp -f "${ROOTDIR}/usb/device/source/classes/include/usb_cdc_pstn.h" "${OUTPUTDIR}/usb_cdc_pstn.h"
cp -f "${ROOTDIR}/usb/common/include/usb_misc.h" "${OUTPUTDIR}/usb_misc.h"
cp -f "${ROOTDIR}/usb/host/source/host/khci/khci_prv.h" "${OUTPUTDIR}/khci_prv.h"
cp -f "${ROOTDIR}/usb/device/source/include/dev_cnfg.h" "${OUTPUTDIR}/dev_cnfg.h"
cp -f "${ROOTDIR}/usb/common/include/usb_debug.h" "${OUTPUTDIR}/usb_debug.h"
cp -f "${ROOTDIR}/usb/device/source/classes/include/usb_class.h" "${OUTPUTDIR}/usb_class.h"
cp -f "${ROOTDIR}/usb/device/source/classes/include/usb_phdc.h" "${OUTPUTDIR}/usb_phdc.h"
cp -f "${ROOTDIR}/usb/common/include/usb_misc_prv.h" "${OUTPUTDIR}/usb_misc_prv.h"
cp -f "${ROOTDIR}/usb/common/include/usb_error.h" "${OUTPUTDIR}/usb_error.h"
cp -f "${ROOTDIR}/usb/device/source/classes/include/usb_msc.h" "${OUTPUTDIR}/usb_msc.h"
cp -f "${ROOTDIR}/usb/device/source/include/rtos/mqx_dev.h" "${OUTPUTDIR}/mqx_dev.h"
cp -f "${ROOTDIR}/usb/common/include/usb_prv.h" "${OUTPUTDIR}/usb_prv.h"
cp -f "${ROOTDIR}/usb/device/source/include/khci/khci_dev_main.h" "${OUTPUTDIR}/khci_dev_main.h"
cp -f "${ROOTDIR}/usb/device/source/include/ehci/ehci_dev_main.h" "${OUTPUTDIR}/ehci_dev_main.h"
cp -f "${ROOTDIR}/usb/device/source/classes/include/usb_framework.h" "${OUTPUTDIR}/usb_framework.h"
cp -f "${ROOTDIR}/usb/common/include/usb_types.h" "${OUTPUTDIR}/usb_types.h"
cp -f "${ROOTDIR}/usb/common/include/usb_desc.h" "${OUTPUTDIR}/usb_desc.h"
cp -f "${ROOTDIR}/usb/device/source/include/devapi.h" "${OUTPUTDIR}/devapi.h"
cp -f "${ROOTDIR}/usb/common/include/usb.h" "${OUTPUTDIR}/usb.h"
cp -f "${ROOTDIR}/usb/device/source/classes/include/usb_cdc.h" "${OUTPUTDIR}/usb_cdc.h"
cp -f "${ROOTDIR}/usb/device/source/classes/include/usb_stack_config.h" "${OUTPUTDIR}/usb_stack_config.h"
cp -f "${ROOTDIR}/usb/device/source/classes/include/usb_msc_scsi.h" "${OUTPUTDIR}/usb_msc_scsi.h"
cp -f "${ROOTDIR}/usb/device/source/include/dev_main.h" "${OUTPUTDIR}/dev_main.h"
cp -f "${ROOTDIR}/usb/device/source/classes/include/usb_audio.h" "${OUTPUTDIR}/usb_audio.h"
cp -f "${ROOTDIR}/usb/device/source/classes/include/usb_hid.h" "${OUTPUTDIR}/usb_hid.h"


# kds files
if [ "${TOOL}" = "kds" ]; then
:
fi

# gcc_arm files
if [ "${TOOL}" = "gcc_arm" ]; then
:
fi


