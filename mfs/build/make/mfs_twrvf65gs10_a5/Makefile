#----------------------------------------------------------
# setup variables
#----------------------------------------------------------
MQX_ROOTDIR             = $(abspath ../../../..)
TYPE                    = library
NAME                    = mfs
BOARD                   = twrvf65gs10_a5
LIBRARY_ROOTDIR         = $(MQX_ROOTDIR)/lib/$(BOARD).$(TOOL)
LIBRARY_DIR             = $(LIBRARY_ROOTDIR)/$(CONFIG)/mfs
LIBRARY_FILE            = $(LIBRARY_DIR)/mfs.$(LIBRARY_EXT)
POSTBUILD_CMD           = $(if $(findstring $(HOSTENV),WINDOWS),$(MQX_ROOTDIR)/mfs/build/bat/mfs_twrvf65gs10_a5.bat $(MQX_ROOTDIR) $(LIBRARY_DIR) $(TOOL),$(MQX_ROOTDIR)/mfs/build/bat/mfs_twrvf65gs10_a5.sh $(MQX_ROOTDIR) $(LIBRARY_DIR) $(TOOL))


#-----------------------------------------------------------
# load tool settings
#-----------------------------------------------------------
-include tools/$(TOOL).mak


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(MQX_ROOTDIR)/mfs/source/include
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
INCLUDE += $(MQX_ROOTDIR)/mfs/source/include
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
# 'source' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mfs/source/generic/dir_read.c
SOURCES += $(MQX_ROOTDIR)/mfs/source/generic/mfs_attribute.c
SOURCES += $(MQX_ROOTDIR)/mfs/source/generic/mfs_cluster.c
SOURCES += $(MQX_ROOTDIR)/mfs/source/generic/mfs_create.c
SOURCES += $(MQX_ROOTDIR)/mfs/source/generic/mfs_data_sector.c
SOURCES += $(MQX_ROOTDIR)/mfs/source/generic/mfs_delete.c
SOURCES += $(MQX_ROOTDIR)/mfs/source/generic/mfs_dir_entry.c
SOURCES += $(MQX_ROOTDIR)/mfs/source/generic/mfs_directory.c
SOURCES += $(MQX_ROOTDIR)/mfs/source/generic/mfs_fat.c
SOURCES += $(MQX_ROOTDIR)/mfs/source/generic/mfs_format.c
SOURCES += $(MQX_ROOTDIR)/mfs/source/generic/mfs_handle.c
SOURCES += $(MQX_ROOTDIR)/mfs/source/generic/mfs_init.c
SOURCES += $(MQX_ROOTDIR)/mfs/source/generic/mfs_lfn.c
SOURCES += $(MQX_ROOTDIR)/mfs/source/generic/mfs_misc.c
SOURCES += $(MQX_ROOTDIR)/mfs/source/generic/mfs_open.c
SOURCES += $(MQX_ROOTDIR)/mfs/source/generic/mfs_parse.c
SOURCES += $(MQX_ROOTDIR)/mfs/source/generic/mfs_read.c
SOURCES += $(MQX_ROOTDIR)/mfs/source/generic/mfs_rename.c
SOURCES += $(MQX_ROOTDIR)/mfs/source/generic/mfs_rw.c
SOURCES += $(MQX_ROOTDIR)/mfs/source/generic/mfs_search.c
SOURCES += $(MQX_ROOTDIR)/mfs/source/generic/mfs_time.c
SOURCES += $(MQX_ROOTDIR)/mfs/source/generic/mfs_valdev.c
SOURCES += $(MQX_ROOTDIR)/mfs/source/generic/mfs_write.c
SOURCES += $(MQX_ROOTDIR)/mfs/source/generic/part_mgr.c


#----------------------------------------------------------
# load global settings
#----------------------------------------------------------
include $(MQX_ROOTDIR)/build/common/make/global.mak
-include $(MQX_ROOTDIR)/build/$(BOARD)/make/tools/$(TOOL).mak
include $(MQX_ROOTDIR)/build/common/make/verify.mak
include $(MQX_ROOTDIR)/build/common/make/lib-process.mak

