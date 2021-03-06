#----------------------------------------------------------
# setup variables
#----------------------------------------------------------
MQX_ROOTDIR             = $(abspath ../../../../..)
TYPE                    = library
NAME                    = usbh
BOARD                   = twrk64f120m
LIBRARY_ROOTDIR         = $(MQX_ROOTDIR)/lib/$(BOARD).$(TOOL)
LIBRARY_DIR             = $(LIBRARY_ROOTDIR)/$(CONFIG)/usb
LIBRARY_FILE            = $(LIBRARY_DIR)/usbh.$(LIBRARY_EXT)
POSTBUILD_CMD           = $(if $(findstring $(HOSTENV),WINDOWS),$(MQX_ROOTDIR)/usb/host/build/bat/usbh_twrk64f120m.bat $(MQX_ROOTDIR) $(LIBRARY_DIR) $(TOOL),$(MQX_ROOTDIR)/usb/host/build/bat/usbh_twrk64f120m.sh $(MQX_ROOTDIR) $(LIBRARY_DIR) $(TOOL))


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
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/hid
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/hub
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/include
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/host/khci
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/msd
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/msd/mfs
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/rtos/mqx
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/phdc
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/printer
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/adc/kadc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/adc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/cm
INCLUDE += $(MQX_ROOTDIR)/config/twrk64f120m
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/dma
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/macnet
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/phy
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/esdhc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/flashx/freescale
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/flashx
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/can/flexcan
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/gpio/kgpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/gpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/hwtimer
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c/polled
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/bsp/twrk64f120m
INCLUDE += $(MQX_ROOTDIR)/config/common
INCLUDE += $(MQX_ROOTDIR)/mqx/source/include
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/debug
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/io_mem
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/io_null
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pipe
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lpm
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lwadc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lwgpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb/mqxa
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pccard
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcflash
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/rtc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sai
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sai/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sdcard/sdcard_esdhc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sdcard/sdcard_spi
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sdcard
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/serial
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/spi
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/tchres
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/tfs
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/timer
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/usb
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/usb_dcd
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/usb_dcd/polled
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/usb_dcd/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m/cpu
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m/core/M4
INCLUDE += $(MQX_ROOTDIR)/mqx/source/tad
INCLUDE += $(MQX_ROOTDIR)/mfs/source/include
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/audio
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/cdc
INCLUDE += $(MQX_ROOTDIR)/usb/common/include
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/hid
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/hub
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/include
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/host/khci
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/msd
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/msd/mfs
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/rtos/mqx
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/phdc
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/printer
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/adc/kadc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/adc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/cm
INCLUDE += $(MQX_ROOTDIR)/config/twrk64f120m
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/dma
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/macnet
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/phy
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/esdhc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/flashx/freescale
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/flashx
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/can/flexcan
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/gpio/kgpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/gpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/hwtimer
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c/polled
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/bsp/twrk64f120m
INCLUDE += $(MQX_ROOTDIR)/config/common
INCLUDE += $(MQX_ROOTDIR)/mqx/source/include
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/debug
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/io_mem
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/io_null
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pipe
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lpm
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lwadc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lwgpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb/mqxa
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pccard
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcflash
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/rtc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sai
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sai/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sdcard/sdcard_esdhc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sdcard/sdcard_spi
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sdcard
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/serial
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/spi
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/tchres
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/tfs
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/timer
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/usb
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/usb_dcd
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/usb_dcd/polled
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/usb_dcd/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m/cpu
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m/core/M4
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
# 'hid' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/usb/host/source/classes/hid/usb_host_hid.c

#-----------------------------------------------------------
# 'hub' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/usb/host/source/classes/hub/usb_host_hub.c
SOURCES += $(MQX_ROOTDIR)/usb/host/source/classes/hub/usb_host_hub_sm.c

#-----------------------------------------------------------
# 'khci' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/usb/host/source/host/khci/khci.c

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

