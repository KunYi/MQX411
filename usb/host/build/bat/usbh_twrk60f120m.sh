#!/usr/bin/env bash

# expect forward slash paths
ROOTDIR="${1}"
OUTPUTDIR="${2}"
TOOL="${3}"


# copy common files
mkdir -p "${OUTPUTDIR}"
mkdir -p "${OUTPUTDIR}/"
cp -f "${ROOTDIR}/usb/common/include/usb_misc.h" "${OUTPUTDIR}/usb_misc.h"
cp -f "${ROOTDIR}/usb/host/source/host/ehci/ehci_shut.h" "${OUTPUTDIR}/ehci_shut.h"
cp -f "${ROOTDIR}/usb/common/include/usb_debug.h" "${OUTPUTDIR}/usb_debug.h"
cp -f "${ROOTDIR}/usb/host/source/rtos/mqx/usb_mqx.h" "${OUTPUTDIR}/usb_mqx.h"
cp -f "${ROOTDIR}/usb/common/include/usb_misc_prv.h" "${OUTPUTDIR}/usb_misc_prv.h"
cp -f "${ROOTDIR}/usb/host/source/classes/printer/usb_host_printer.h" "${OUTPUTDIR}/usb_host_printer.h"
cp -f "${ROOTDIR}/usb/host/source/rtos/mqx/mqx_host.h" "${OUTPUTDIR}/mqx_host.h"
cp -f "${ROOTDIR}/usb/common/include/usb_error.h" "${OUTPUTDIR}/usb_error.h"
cp -f "${ROOTDIR}/usb/host/source/include/host_close.h" "${OUTPUTDIR}/host_close.h"
cp -f "${ROOTDIR}/usb/host/source/include/hostapi.h" "${OUTPUTDIR}/hostapi.h"
cp -f "${ROOTDIR}/usb/host/source/host/ehci/ehci_intr.h" "${OUTPUTDIR}/ehci_intr.h"
cp -f "${ROOTDIR}/usb/common/include/usb_types.h" "${OUTPUTDIR}/usb_types.h"
cp -f "${ROOTDIR}/usb/host/source/classes/phdc/usb_host_phdc.h" "${OUTPUTDIR}/usb_host_phdc.h"
cp -f "${ROOTDIR}/usb/host/source/include/host_dev_list.h" "${OUTPUTDIR}/host_dev_list.h"
cp -f "${ROOTDIR}/usb/host/source/include/host_cnfg.h" "${OUTPUTDIR}/host_cnfg.h"
cp -f "${ROOTDIR}/usb/host/source/classes/hub/usb_host_hub.h" "${OUTPUTDIR}/usb_host_hub.h"
cp -f "${ROOTDIR}/usb/host/source/classes/audio/usb_host_audio.h" "${OUTPUTDIR}/usb_host_audio.h"
cp -f "${ROOTDIR}/usb/host/source/host/ehci/ehci_iso.h" "${OUTPUTDIR}/ehci_iso.h"
cp -f "${ROOTDIR}/usb/host/source/host/khci/khci.h" "${OUTPUTDIR}/khci.h"
cp -f "${ROOTDIR}/usb/common/include/usb.h" "${OUTPUTDIR}/usb.h"
cp -f "${ROOTDIR}/usb/host/source/classes/msd/usb_host_msd_bo.h" "${OUTPUTDIR}/usb_host_msd_bo.h"
cp -f "${ROOTDIR}/usb/host/source/host/ehci/ehci.h" "${OUTPUTDIR}/ehci.h"
cp -f "${ROOTDIR}/usb/host/source/classes/hid/usb_host_hid.h" "${OUTPUTDIR}/usb_host_hid.h"
cp -f "${ROOTDIR}/usb/host/source/include/host_snd.h" "${OUTPUTDIR}/host_snd.h"
cp -f "${ROOTDIR}/usb/host/source/classes/msd/usb_host_msd_ufi.h" "${OUTPUTDIR}/usb_host_msd_ufi.h"
cp -f "${ROOTDIR}/usb/host/source/include/host_cnl.h" "${OUTPUTDIR}/host_cnl.h"
cp -f "${ROOTDIR}/usb/host/source/host/ehci/ehci_cache.h" "${OUTPUTDIR}/ehci_cache.h"
cp -f "${ROOTDIR}/usb/host/source/rtos/mqx/mqx_dll.h" "${OUTPUTDIR}/mqx_dll.h"
cp -f "${ROOTDIR}/usb/host/source/host/ehci/ehci_bw.h" "${OUTPUTDIR}/ehci_bw.h"
cp -f "${ROOTDIR}/usb/host/source/host/ehci/ehci_utl.h" "${OUTPUTDIR}/ehci_utl.h"
cp -f "${ROOTDIR}/usb/host/source/include/host_rcv.h" "${OUTPUTDIR}/host_rcv.h"
cp -f "${ROOTDIR}/usb/host/source/include/host_main.h" "${OUTPUTDIR}/host_main.h"
cp -f "${ROOTDIR}/usb/common/include/usb_prv.h" "${OUTPUTDIR}/usb_prv.h"
cp -f "${ROOTDIR}/usb/host/source/include/host_shut.h" "${OUTPUTDIR}/host_shut.h"
cp -f "${ROOTDIR}/usb/host/source/include/host_ch9.h" "${OUTPUTDIR}/host_ch9.h"
cp -f "${ROOTDIR}/usb/host/source/classes/msd/mfs/usbmfspr.h" "${OUTPUTDIR}/usbmfspr.h"
cp -f "${ROOTDIR}/usb/host/source/classes/hub/usb_host_hub_sm.h" "${OUTPUTDIR}/usb_host_hub_sm.h"
cp -f "${ROOTDIR}/usb/common/include/usb_desc.h" "${OUTPUTDIR}/usb_desc.h"
cp -f "${ROOTDIR}/usb/host/source/include/host_common.h" "${OUTPUTDIR}/host_common.h"
cp -f "${ROOTDIR}/usb/host/source/classes/msd/mfs/usbmfs.h" "${OUTPUTDIR}/usbmfs.h"
cp -f "${ROOTDIR}/usb/host/source/classes/cdc/usb_host_cdc.h" "${OUTPUTDIR}/usb_host_cdc.h"
cp -f "${ROOTDIR}/usb/host/source/host/ehci/ehci_main.h" "${OUTPUTDIR}/ehci_main.h"
cp -f "${ROOTDIR}/usb/host/source/host/ehci/ehci_cncl.h" "${OUTPUTDIR}/ehci_cncl.h"


# kds files
if [ "${TOOL}" = "kds" ]; then
:
fi

# gcc_arm files
if [ "${TOOL}" = "gcc_arm" ]; then
:
fi


