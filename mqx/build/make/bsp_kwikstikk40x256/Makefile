#----------------------------------------------------------
# setup variables
#----------------------------------------------------------
MQX_ROOTDIR             = $(abspath ../../../..)
TYPE                    = library
NAME                    = bsp
BOARD                   = kwikstikk40x256
LIBRARY_ROOTDIR         = $(MQX_ROOTDIR)/lib/$(BOARD).$(TOOL)
LIBRARY_DIR             = $(LIBRARY_ROOTDIR)/$(CONFIG)/bsp
LIBRARY_FILE            = $(LIBRARY_DIR)/bsp.$(LIBRARY_EXT)
POSTBUILD_CMD           = $(if $(findstring $(HOSTENV),WINDOWS),$(MQX_ROOTDIR)/mqx/build/bat/bsp_kwikstikk40x256.bat $(MQX_ROOTDIR) $(LIBRARY_DIR) $(TOOL),$(MQX_ROOTDIR)/mqx/build/bat/bsp_kwikstikk40x256.sh $(MQX_ROOTDIR) $(LIBRARY_DIR) $(TOOL))


#-----------------------------------------------------------
# load tool settings
#-----------------------------------------------------------
-include tools/$(TOOL).mak


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/adc/kadc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/adc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/cm
INCLUDE += $(MQX_ROOTDIR)/config/kwikstikk40x256
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/dma
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/esdhc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/flashx/freescale
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/flashx
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/can/flexcan
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/gpio/kgpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/gpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/hmi
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/hmi/TSS/shared
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/hwtimer
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c/polled
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2s
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2s/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/bsp/kwikstikk40x256
INCLUDE += $(MQX_ROOTDIR)/config/common
INCLUDE += $(MQX_ROOTDIR)/mqx/source/include
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/debug
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/io_mem
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/io_null
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pipe
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lcd
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lpm
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lwadc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lwgpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb/mqxa
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/rtc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sdcard/sdcard_spi
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sdcard/sdcard_esdhc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sdcard
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/serial
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/spi
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
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/adc/kadc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/adc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/cm
INCLUDE += $(MQX_ROOTDIR)/config/kwikstikk40x256
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/dma
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/esdhc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/flashx/freescale
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/flashx
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/can/flexcan
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/gpio/kgpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/gpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/hmi
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/hmi/TSS/shared
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/hwtimer
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c/polled
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2s
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2s/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/bsp/kwikstikk40x256
INCLUDE += $(MQX_ROOTDIR)/config/common
INCLUDE += $(MQX_ROOTDIR)/mqx/source/include
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/debug
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/io_mem
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/io_null
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pipe
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lcd
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lpm
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lwadc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lwgpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb/mqxa
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/rtc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sdcard/sdcard_spi
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sdcard/sdcard_esdhc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sdcard
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/serial
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/spi
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
endif



#-----------------------------------------------------------
# 'adc' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/adc/kadc/adc_mk40.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/adc/kadc/adc_kadc.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/adc/kadc/adt_kpdb.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/adc/adc.c

#-----------------------------------------------------------
# 'cm' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/cm/cm.c

#-----------------------------------------------------------
# 'dma' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/dma/edma_kinetis.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/dma/dma.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/dma/edma.c

#-----------------------------------------------------------
# 'esdhc' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/esdhc/esdhc_mk40.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/esdhc/esdhc.c

#-----------------------------------------------------------
# 'flashx' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/flashx/freescale/flash_mk40.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/flashx/freescale/flash_ftfl.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/flashx/flashx.c

#-----------------------------------------------------------
# 'flexcan' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/can/flexcan/flexcan_mk40.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/can/flexcan/kflexcan.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/can/flexcan/kflexcan_int.c

#-----------------------------------------------------------
# 'gpio' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/gpio/kgpio/io_gpio_kgpio.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/gpio/io_gpio.c

#-----------------------------------------------------------
# 'hmi' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/hmi/btnled.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/hmi/hmi_client.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/hmi/hmi_lwgpio_provider.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/hmi/hmi_tss_provider.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/hmi/TSS/shared/TSS_Freemaster.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/hmi/TSS/shared/TSS_Sensor.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/hmi/TSS/shared/TSS_SensorGPIO.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/hmi/TSS/shared/TSS_SensorTSI.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/hmi/TSS/shared/TSS_SensorTSIL.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/hmi/TSS/shared/TSS_SystemSetupData.c

#-----------------------------------------------------------
# 'hwtimer' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/hwtimer/hwtimer_lpt.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/hwtimer/hwtimer_systick.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/hwtimer/hwtimer_pit.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/hwtimer/hwtimer_kinetis.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/hwtimer/hwtimer.c

#-----------------------------------------------------------
# 'i2c' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/i2c/i2c_mk40.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/i2c/polled/i2c_pol_ki2c.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/i2c/int/i2c_int_ki2c_legacy.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/i2c/polled/i2c_pol.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/i2c/int/i2c_int.c

#-----------------------------------------------------------
# 'i2s' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/i2s/i2s_mk40.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/i2s/int/i2s_int_ki2s.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/i2s/int/i2s_int.c

#-----------------------------------------------------------
# 'init' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/kwikstikk40x256/bsp_cm.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/kwikstikk40x256/init_adc.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/kwikstikk40x256/init_lwadc.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/kwikstikk40x256/init_esdhc.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/kwikstikk40x256/init_flashx.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/kwikstikk40x256/init_gpio.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/kwikstikk40x256/init_hw.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/kwikstikk40x256/init_hmi.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/kwikstikk40x256/init_i2c.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/kwikstikk40x256/init_i2s.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/kwikstikk40x256/init_iodebug.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/kwikstikk40x256/init_lpm.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/kwikstikk40x256/init_sci.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/kwikstikk40x256/init_sdcard.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/kwikstikk40x256/init_spi.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/kwikstikk40x256/init_usb.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/kwikstikk40x256/init_usb_dcd.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/kwikstikk40x256/init_dma.c

#-----------------------------------------------------------
# 'init-common' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/kwikstikk40x256/init_bsp.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/kwikstikk40x256/mqx_init.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/kwikstikk40x256/mqx_main.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/kwikstikk40x256/vectors.c

#-----------------------------------------------------------
# 'iodebug' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/debug/iodebug.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/debug/iodebug_asm.S

#-----------------------------------------------------------
# 'iomem' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/io_mem/io_mem.c

#-----------------------------------------------------------
# 'ionull' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/io_null/io_null.c

#-----------------------------------------------------------
# 'iopipe' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/pipe/io_pipe.c

#-----------------------------------------------------------
# 'lcd' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/lcd/Fonts_LCD.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/lcd/slcd_kwikstik-k40x256.c

#-----------------------------------------------------------
# 'lpm' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/lpm/lpm_mc.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/lpm/lpm.c

#-----------------------------------------------------------
# 'lwadc' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/lwadc/lwadc_kadc.c

#-----------------------------------------------------------
# 'lwgpio' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/lwgpio/lwgpio_kgpio.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/lwgpio/lwgpio.c

#-----------------------------------------------------------
# 'pcb' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/pcb/io_pcb2.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/pcb/io_pcb.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/pcb/iop_inst.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/pcb/mqxa/pcb_mqxa.c

#-----------------------------------------------------------
# 'rtc' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/rtc/krtc.c

#-----------------------------------------------------------
# 'sdcard' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/sdcard/sdcard_spi/sdcard_spi.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/sdcard/sdcard_esdhc/sdcard_esdhc.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/sdcard/sdcard.c

#-----------------------------------------------------------
# 'serial' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/serial/serl_mk40.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/serial/int/serl_int_kuart.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/serial/polled/serl_pol_kuart.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/serial/dma/serl_dma_kuart.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/serial/io_sinit.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/serial/polled/serl_pol.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/serial/int/serl_int.c

#-----------------------------------------------------------
# 'spi' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/spi/spi_mk40.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/spi/spi_dspi_dma.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/spi/spi_dspi_common.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/spi/spi.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/spi/spi_dspi.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/spi/spi_dspi_common.c

#-----------------------------------------------------------
# 'tfs' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/tfs/tfs.c

#-----------------------------------------------------------
# 'timer' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/timer/timer_qpit.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/timer/timer_kinetis.c

#-----------------------------------------------------------
# 'usb' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/usb/usb_mk40.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/usb/usb_mk40_device.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/usb/usb_install.c

#-----------------------------------------------------------
# 'usbdcd' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/usb_dcd/usb_dcd_mk40.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/usb_dcd/polled/usb_dcd_pol_kn.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/usb_dcd/int/usb_dcd_int_kn.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/usb_dcd/polled/usb_dcd_pol.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/usb_dcd/int/usb_dcd_int.c


#----------------------------------------------------------
# load global settings
#----------------------------------------------------------
include $(MQX_ROOTDIR)/build/common/make/global.mak
-include $(MQX_ROOTDIR)/build/$(BOARD)/make/tools/$(TOOL).mak
include $(MQX_ROOTDIR)/build/common/make/verify.mak
include $(MQX_ROOTDIR)/build/common/make/lib-process.mak

