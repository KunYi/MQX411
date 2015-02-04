#----------------------------------------------------------
# setup variables
#----------------------------------------------------------
MQX_ROOTDIR             = $(abspath ../../../..)
TYPE                    = library
NAME                    = bsp
BOARD                   = twrvf65gs10_a5
LIBRARY_ROOTDIR         = $(MQX_ROOTDIR)/lib/$(BOARD).$(TOOL)
LIBRARY_DIR             = $(LIBRARY_ROOTDIR)/$(CONFIG)/bsp
LIBRARY_FILE            = $(LIBRARY_DIR)/bsp.$(LIBRARY_EXT)
POSTBUILD_CMD           = $(if $(findstring $(HOSTENV),WINDOWS),$(MQX_ROOTDIR)/mqx/build/bat/bsp_twrvf65gs10_a5.bat $(MQX_ROOTDIR) $(LIBRARY_DIR) $(TOOL),$(MQX_ROOTDIR)/mqx/build/bat/bsp_twrvf65gs10_a5.sh $(MQX_ROOTDIR) $(LIBRARY_DIR) $(TOOL))


#-----------------------------------------------------------
# load tool settings
#-----------------------------------------------------------
-include tools/$(TOOL).mak


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
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
endif
ifeq ($(CONFIG),release)
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
endif



#-----------------------------------------------------------
# 'cm' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/cm/vybrid/bsp_clk_nodes.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/cm/vybrid/bsp_clk_api.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/cm/cm.c

#-----------------------------------------------------------
# 'core-mutex' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/core_mutex/sema4_vybrid.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/core_mutex/core_mutex_sema4.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/core_mutex/core_mutex.c

#-----------------------------------------------------------
# 'dcu4' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/dcu4/dcu4_vybrid_a5.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/dcu4/dcu4.c

#-----------------------------------------------------------
# 'dma' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/dma/edma_vybrid_a5.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/dma/dma.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/dma/edma.c

#-----------------------------------------------------------
# 'enet' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/macnet/macnet_vybrid.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/phy/phy_dp83xxx.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/macnet/macnet_init.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/macnet/macnet_join.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/macnet/macnet_mii.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/macnet/macnet_receive.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/macnet/macnet_rejoin.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/macnet/macnet_send.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/macnet/macnet_stop.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/macnet/macnet_util.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/macnet/macnet_1588.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/enaddr.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/enclose.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/enerr.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/eninit.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/enjoin.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/enleave.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/enmediactl.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/enopen.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/enrecv.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/ensend.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/enstat.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/enstop.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/enutil.c

#-----------------------------------------------------------
# 'esdhc' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/esdhc/esdhc_vf65gs10_a5.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/esdhc/esdhc.c

#-----------------------------------------------------------
# 'fbdev' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/fbdev/fbdev_dcu4.c

#-----------------------------------------------------------
# 'flashx' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/flashx/freescale/flash_quadspi_spansion.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/flashx/freescale/flash_quadspi.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/flashx/flashx.c

#-----------------------------------------------------------
# 'flexcan' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/can/flexcan/fsl_flexcan_hal.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/can/flexcan/fsl_flexcan_driver.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/can/flexcan/fsl_flexcan_int.c

#-----------------------------------------------------------
# 'hwtimer' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/hwtimer/hwtimer_vybrid_a5.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/hwtimer/hwtimer_gtim.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/hwtimer/hwtimer_pit.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/hwtimer/hwtimer.c

#-----------------------------------------------------------
# 'i2c' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/i2c/i2c_qi2c_vybrid_a5.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/i2c/polled/i2c_pol_qi2c.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/i2c/int/i2c_int_qi2c.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/i2c/polled/i2c_pol.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/i2c/int/i2c_int.c

#-----------------------------------------------------------
# 'init' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5/bsp_cm.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5/init_enet.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5/init_dcu4.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5/init_fbdev.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5/init_esdhc.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5/init_gic.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5/init_gpio.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5/init_hw.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5/init_i2c.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5/init_lpm.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5/init_nandflash.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5/init_sai.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5/init_sci.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5/init_sdcard.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5/init_spi.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5/init_qspi.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5/init_lwadc.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5/init_pcb_shm.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5/init_flashx.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5/init_usb.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5/init_dma.c

#-----------------------------------------------------------
# 'init-common' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5/init_bsp.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5/mqx_init.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5/mqx_main.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/twrvf65gs10_a5/init_core_mutex.c

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
# 'lpm' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/lpm/vybrid/lpm_vybrid.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/lpm/vybrid/vybrid_wkpu.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/lpm/lpm.c

#-----------------------------------------------------------
# 'lwadc' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/lwadc/lwadc_vybrid.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/lwadc/lwadc_vadc.c

#-----------------------------------------------------------
# 'lwgpio' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/lwgpio/lwgpio_vgpio.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/lwgpio/lwgpio.c

#-----------------------------------------------------------
# 'nandflash' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/nandflash/nfc/nfc_vf65gs10.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/nandflash/nfc/nfc.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/nandflash/nand_devices/MT29F2G16.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/nandflash/nandflash.c

#-----------------------------------------------------------
# 'pcb' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/pcb/shmem/pcb_shm_vybrid_a5.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/pcb/shmem/pcb_shm.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/pcb/io_pcb2.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/pcb/io_pcb.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/pcb/iop_inst.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/pcb/mqxa/pcb_mqxa.c

#-----------------------------------------------------------
# 'qspi' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/qspi/qspi_vybrid_a5.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/qspi/qspi.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/qspi/qspi_quadspi.c

#-----------------------------------------------------------
# 'rtc' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/rtc/rtc_vf65gs10_a5.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/rtc/srtc.c

#-----------------------------------------------------------
# 'sai' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/sai/sai_vf65gs10.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/sai/int/sai_int_ksai.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/sai/dma/sai_dma_ksai.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/sai/int/sai_int.c

#-----------------------------------------------------------
# 'sdcard' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/sdcard/sdcard_esdhc/sdcard_esdhc.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/sdcard/sdcard.c

#-----------------------------------------------------------
# 'serial' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/serial/serl_vybrid.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/serial/int/serl_int_kuart.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/serial/polled/serl_pol_kuart.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/serial/dma/serl_dma_kuart.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/serial/io_sinit.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/serial/polled/serl_pol.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/serial/int/serl_int.c

#-----------------------------------------------------------
# 'spi' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/spi/spi_vybrid_a5.c
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
# 'usb' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/usb/usb_vybrid.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/usb/usb_vybrid_device.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/usb/usb_install.c


#----------------------------------------------------------
# load global settings
#----------------------------------------------------------
include $(MQX_ROOTDIR)/build/common/make/global.mak
-include $(MQX_ROOTDIR)/build/$(BOARD)/make/tools/$(TOOL).mak
include $(MQX_ROOTDIR)/build/common/make/verify.mak
include $(MQX_ROOTDIR)/build/common/make/lib-process.mak

