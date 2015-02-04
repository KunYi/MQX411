#----------------------------------------------------------
# setup variables
#----------------------------------------------------------
MQX_ROOTDIR             = $(abspath ../../../../..)
TYPE                    = library
NAME                    = usbh
BOARD                   = twrvf65gs10_a5
LIBRARY_ROOTDIR         = $(MQX_ROOTDIR)/lib/$(BOARD).$(TOOL)
LIBRARY_DIR             = $(LIBRARY_ROOTDIR)/$(CONFIG)/usb
LIBRARY_FILE            = $(LIBRARY_DIR)/usbh.$(LIBRARY_EXT)
POSTBUILD_CMD           = $(if $(findstring $(HOSTENV),WINDOWS),$(MQX_ROOTDIR)/usb/host/build/bat/usbh_twrvf65gs10_a5.bat $(MQX_ROOTDIR) $(LIBRARY_DIR) $(TOOL),$(MQX_ROOTDIR)/usb/host/build/bat/usbh_twrvf65gs10_a5.sh $(MQX_ROOTDIR) $(LIBRARY_DIR) $(TOOL))


#-----------------------------------------------------------
# load tool settings
#-----------------------------------------------------------
-include tools/$(TOOL).mak


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/audio
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/cdc
INCLUDE += $(MQX_ROOTDIR)/usb/common/include
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/host/ehci
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/hid
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/hub
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/include
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/msd
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/msd/mfs
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/rtos/mqx
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/phdc
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/printer
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/cm
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/cm/vybrid
INCLUDE += $(MQX_ROOTDIR)/config/twrvf65gs10_a5
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/core_mutex
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/dcu4
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/dma
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/macnet
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/phy
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/esdhc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/fbdev
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/flashx/freescale
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/flashx
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/can/flexcan
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/hwtimer
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c/polled
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5
INCLUDE += $(MQX_ROOTDIR)/config/common
INCLUDE += $(MQX_ROOTDIR)/mqx/source/include
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/io_mem
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/io_null
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pipe
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lpm/vybrid
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lpm
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lwadc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lwgpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/nandflash/nfc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/nandflash
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb/shmem
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb/mqxa
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/qspi
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/rtc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sai
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sai/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sdcard/sdcard_esdhc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sdcard
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/serial
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/spi
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/tfs
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/usb
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_a/cpu
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_a
INCLUDE += $(MQX_ROOTDIR)/mqx/source/tad
INCLUDE += $(MQX_ROOTDIR)/mfs/source/include
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/audio
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/cdc
INCLUDE += $(MQX_ROOTDIR)/usb/common/include
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/host/ehci
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/hid
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/hub
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/include
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/msd
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/msd/mfs
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/rtos/mqx
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/phdc
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/printer
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/cm
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/cm/vybrid
INCLUDE += $(MQX_ROOTDIR)/config/twrvf65gs10_a5
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/core_mutex
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/dcu4
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/dma
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/macnet
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/phy
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/esdhc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/fbdev
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/flashx/freescale
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/flashx
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/can/flexcan
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/hwtimer
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c/polled
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5
INCLUDE += $(MQX_ROOTDIR)/config/common
INCLUDE += $(MQX_ROOTDIR)/mqx/source/include
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/io_mem
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/io_null
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pipe
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lpm/vybrid
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lpm
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lwadc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lwgpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/nandflash/nfc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/nandflash
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb/shmem
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb/mqxa
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/qspi
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/rtc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sai
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sai/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sdcard/sdcard_esdhc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sdcard
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/serial
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/spi
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/tfs
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/usb
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_a/cpu
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_a
INCLUDE += $(MQX_ROOTDIR)/mqx/source/tad
INCLUDE += $(MQX_ROOTDIR)/mfs/source/include
endif



#-----------------------------------------------------------
# 'audio' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/usb/host/source/classes/audio/usb_host_audio.c
SOURCES += $(MQX_ROOTDIR)/usb/host/source/classes/audio/usb_host_audio_intf.c

#-----------------------------------------------------------
# 'cdc' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/usb/host/source/classes/cdc/usb_host_cdc.c
SOURCES += $(MQX_ROOTDIR)/usb/host/source/classes/cdc/usb_host_cdc_intf.c

#-----------------------------------------------------------
# 'ehci' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/usb/host/source/host/ehci/ehci_bw.c
SOURCES += $(MQX_ROOTDIR)/usb/host/source/host/ehci/ehci_cncl.c
SOURCES += $(MQX_ROOTDIR)/usb/host/source/host/ehci/ehci_intr.c
SOURCES += $(MQX_ROOTDIR)/usb/host/source/host/ehci/ehci_iso.c
SOURCES += $(MQX_ROOTDIR)/usb/host/source/host/ehci/ehci_main.c
SOURCES += $(MQX_ROOTDIR)/usb/host/source/host/ehci/ehci_shut.c
SOURCES += $(MQX_ROOTDIR)/usb/host/source/host/ehci/ehci_utl.c

#-----------------------------------------------------------
# 'hid' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/usb/host/source/classes/hid/usb_host_hid.c

#-----------------------------------------------------------
# 'hub' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/usb/host/source/classes/hub/usb_host_hub.c
SOURCES += $(MQX_ROOTDIR)/usb/host/source/classes/hub/usb_host_hub_sm.c

#-----------------------------------------------------------
# 'msd' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/usb/host/source/classes/msd/usb_host_msd_bo.c
SOURCES += $(MQX_ROOTDIR)/usb/host/source/classes/msd/usb_host_msd_queue.c
SOURCES += $(MQX_ROOTDIR)/usb/host/source/classes/msd/usb_host_msd_ufi.c
SOURCES += $(MQX_ROOTDIR)/usb/host/source/classes/msd/mfs/usbmfs.c

#-----------------------------------------------------------
# 'os' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/usb/host/source/rtos/mqx/mqx_dll.c
SOURCES += $(MQX_ROOTDIR)/usb/host/source/rtos/mqx/usb_mqx.c

#-----------------------------------------------------------
# 'phdc' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/usb/host/source/classes/phdc/usb_host_phdc.c

#-----------------------------------------------------------
# 'printer' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/usb/host/source/classes/printer/usb_host_printer.c

#-----------------------------------------------------------
# 'source' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/usb/host/source/host/host_ch9.c
SOURCES += $(MQX_ROOTDIR)/usb/host/source/host/host_close.c
SOURCES += $(MQX_ROOTDIR)/usb/host/source/host/host_cnl.c
SOURCES += $(MQX_ROOTDIR)/usb/host/source/host/host_common.c
SOURCES += $(MQX_ROOTDIR)/usb/host/source/host/host_dev_list.c
SOURCES += $(MQX_ROOTDIR)/usb/host/source/host/host_main.c
SOURCES += $(MQX_ROOTDIR)/usb/host/source/host/host_rcv.c
SOURCES += $(MQX_ROOTDIR)/usb/host/source/host/host_shut.c
SOURCES += $(MQX_ROOTDIR)/usb/host/source/host/host_snd.c


#----------------------------------------------------------
# load global settings
#----------------------------------------------------------
include $(MQX_ROOTDIR)/build/common/make/global.mak
-include $(MQX_ROOTDIR)/build/$(BOARD)/make/tools/$(TOOL).mak
include $(MQX_ROOTDIR)/build/common/make/verify.mak
include $(MQX_ROOTDIR)/build/common/make/lib-process.mak

