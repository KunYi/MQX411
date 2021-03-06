#----------------------------------------------------------
# setup variables
#----------------------------------------------------------
MQX_ROOTDIR             = $(abspath ../../../../..)
TYPE                    = library
NAME                    = usbd
BOARD                   = twrk20d50m
LIBRARY_ROOTDIR         = $(MQX_ROOTDIR)/lib/$(BOARD).$(TOOL)
LIBRARY_DIR             = $(LIBRARY_ROOTDIR)/$(CONFIG)/usb
LIBRARY_FILE            = $(LIBRARY_DIR)/usbd.$(LIBRARY_EXT)
POSTBUILD_CMD           = $(if $(findstring $(HOSTENV),WINDOWS),$(MQX_ROOTDIR)/usb/device/build/bat/usbd_twrk20d50m.bat $(MQX_ROOTDIR) $(LIBRARY_DIR) $(TOOL),$(MQX_ROOTDIR)/usb/device/build/bat/usbd_twrk20d50m.sh $(MQX_ROOTDIR) $(LIBRARY_DIR) $(TOOL))


#-----------------------------------------------------------
# load tool settings
#-----------------------------------------------------------
-include tools/$(TOOL).mak


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(MQX_ROOTDIR)/usb/device/source/classes/include
INCLUDE += $(MQX_ROOTDIR)/usb/common/include
INCLUDE += $(MQX_ROOTDIR)/usb/device/source/include
INCLUDE += $(MQX_ROOTDIR)/usb/device/source/include/khci
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/host/khci
INCLUDE += $(MQX_ROOTDIR)/usb/device/source/include/rtos
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/adc/kadc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/adc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/cm
INCLUDE += $(MQX_ROOTDIR)/config/twrk20d50m
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/dma
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/flashx/freescale
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/flashx
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/gpio/kgpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/gpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/hwtimer
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c/polled
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/bsp/twrk20d50m
INCLUDE += $(MQX_ROOTDIR)/config/common
INCLUDE += $(MQX_ROOTDIR)/mqx/source/include
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/debug
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/io_mem
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/io_null
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pipe
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lpm
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lwadc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lwgpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb/mqxa
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/rtc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sai
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sai/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/serial
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/spi
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/tfs
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/timer
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/usb
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m/cpu
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m/core/M4
INCLUDE += $(MQX_ROOTDIR)/mqx/source/tad
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/usb/device/source/classes/include
INCLUDE += $(MQX_ROOTDIR)/usb/common/include
INCLUDE += $(MQX_ROOTDIR)/usb/device/source/include
INCLUDE += $(MQX_ROOTDIR)/usb/device/source/include/khci
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/host/khci
INCLUDE += $(MQX_ROOTDIR)/usb/device/source/include/rtos
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/adc/kadc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/adc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/cm
INCLUDE += $(MQX_ROOTDIR)/config/twrk20d50m
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/dma
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/flashx/freescale
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/flashx
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/gpio/kgpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/gpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/hwtimer
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c/polled
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/bsp/twrk20d50m
INCLUDE += $(MQX_ROOTDIR)/config/common
INCLUDE += $(MQX_ROOTDIR)/mqx/source/include
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/debug
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/io_mem
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/io_null
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pipe
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lpm
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lwadc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lwgpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb/mqxa
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/rtc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sai
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sai/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/serial
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/spi
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/tfs
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/timer
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/usb
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m/cpu
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m/core/M4
INCLUDE += $(MQX_ROOTDIR)/mqx/source/tad
endif



#-----------------------------------------------------------
# 'audio' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/usb/device/source/classes/audio/usb_audio.c

#-----------------------------------------------------------
# 'cdc' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/usb/device/source/classes/cdc/usb_cdc.c
SOURCES += $(MQX_ROOTDIR)/usb/device/source/classes/cdc/usb_cdc_pstn.c

#-----------------------------------------------------------
# 'class' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/usb/device/source/classes/common/usb_class.c

#-----------------------------------------------------------
# 'framework' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/usb/device/source/classes/common/usb_framework.c

#-----------------------------------------------------------
# 'hid' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/usb/device/source/classes/hid/usb_hid.c

#-----------------------------------------------------------
# 'khci' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/usb/device/source/device/khci/callback_khci.c
SOURCES += $(MQX_ROOTDIR)/usb/device/source/device/khci/khci_dev_main.c

#-----------------------------------------------------------
# 'msd' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/usb/device/source/classes/msd/usb_msc.c
SOURCES += $(MQX_ROOTDIR)/usb/device/source/classes/msd/usb_msc_scsi.c

#-----------------------------------------------------------
# 'phdc' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/usb/device/source/classes/phdc/usb_phdc.c

#-----------------------------------------------------------
# 'source' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/usb/device/source/device/dev_cncl.c
SOURCES += $(MQX_ROOTDIR)/usb/device/source/device/dev_ep_deinit.c
SOURCES += $(MQX_ROOTDIR)/usb/device/source/device/dev_main.c
SOURCES += $(MQX_ROOTDIR)/usb/device/source/device/dev_recv.c
SOURCES += $(MQX_ROOTDIR)/usb/device/source/device/dev_send.c
SOURCES += $(MQX_ROOTDIR)/usb/device/source/device/dev_shut.c
SOURCES += $(MQX_ROOTDIR)/usb/device/source/device/dev_utl.c
SOURCES += $(MQX_ROOTDIR)/usb/device/source/device/dev_main.c


#----------------------------------------------------------
# load global settings
#----------------------------------------------------------
include $(MQX_ROOTDIR)/build/common/make/global.mak
-include $(MQX_ROOTDIR)/build/$(BOARD)/make/tools/$(TOOL).mak
include $(MQX_ROOTDIR)/build/common/make/verify.mak
include $(MQX_ROOTDIR)/build/common/make/lib-process.mak

