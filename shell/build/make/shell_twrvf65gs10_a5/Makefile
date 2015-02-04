#----------------------------------------------------------
# setup variables
#----------------------------------------------------------
MQX_ROOTDIR             = $(abspath ../../../..)
TYPE                    = library
NAME                    = shell
BOARD                   = twrvf65gs10_a5
LIBRARY_ROOTDIR         = $(MQX_ROOTDIR)/lib/$(BOARD).$(TOOL)
LIBRARY_DIR             = $(LIBRARY_ROOTDIR)/$(CONFIG)/shell
LIBRARY_FILE            = $(LIBRARY_DIR)/shell.$(LIBRARY_EXT)
POSTBUILD_CMD           = $(if $(findstring $(HOSTENV),WINDOWS),$(MQX_ROOTDIR)/shell/build/bat/shell_twrvf65gs10_a5.bat $(MQX_ROOTDIR) $(LIBRARY_DIR) $(TOOL),$(MQX_ROOTDIR)/shell/build/bat/shell_twrvf65gs10_a5.sh $(MQX_ROOTDIR) $(LIBRARY_DIR) $(TOOL))


#-----------------------------------------------------------
# load tool settings
#-----------------------------------------------------------
-include tools/$(TOOL).mak


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(MQX_ROOTDIR)/shell/source/include
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
INCLUDE += $(MQX_ROOTDIR)/rtcs/source/include
INCLUDE += $(MQX_ROOTDIR)/usb/device/source/classes/include
INCLUDE += $(MQX_ROOTDIR)/usb/common/include
INCLUDE += $(MQX_ROOTDIR)/usb/device/source/include/ehci
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/host/ehci
INCLUDE += $(MQX_ROOTDIR)/usb/device/source/include
INCLUDE += $(MQX_ROOTDIR)/usb/device/source/include/rtos
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/audio
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/cdc
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/hid
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/hub
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/include
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/msd
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/msd/mfs
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/rtos/mqx
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/phdc
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/printer
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/shell/source/include
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
INCLUDE += $(MQX_ROOTDIR)/rtcs/source/include
INCLUDE += $(MQX_ROOTDIR)/usb/device/source/classes/include
INCLUDE += $(MQX_ROOTDIR)/usb/common/include
INCLUDE += $(MQX_ROOTDIR)/usb/device/source/include/ehci
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/host/ehci
INCLUDE += $(MQX_ROOTDIR)/usb/device/source/include
INCLUDE += $(MQX_ROOTDIR)/usb/device/source/include/rtos
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/audio
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/cdc
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/hid
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/hub
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/include
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/msd
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/msd/mfs
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/rtos/mqx
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/phdc
INCLUDE += $(MQX_ROOTDIR)/usb/host/source/classes/printer
endif



#-----------------------------------------------------------
# 'mfs' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/shell/source/mfs/sh_cache.c
SOURCES += $(MQX_ROOTDIR)/shell/source/mfs/sh_cd.c
SOURCES += $(MQX_ROOTDIR)/shell/source/mfs/sh_compare.c
SOURCES += $(MQX_ROOTDIR)/shell/source/mfs/sh_copy.c
SOURCES += $(MQX_ROOTDIR)/shell/source/mfs/sh_create.c
SOURCES += $(MQX_ROOTDIR)/shell/source/mfs/sh_del.c
SOURCES += $(MQX_ROOTDIR)/shell/source/mfs/sh_df.c
SOURCES += $(MQX_ROOTDIR)/shell/source/mfs/sh_di.c
SOURCES += $(MQX_ROOTDIR)/shell/source/mfs/sh_dir.c
SOURCES += $(MQX_ROOTDIR)/shell/source/mfs/sh_dirent.c
SOURCES += $(MQX_ROOTDIR)/shell/source/mfs/sh_disect.c
SOURCES += $(MQX_ROOTDIR)/shell/source/mfs/sh_flush.c
SOURCES += $(MQX_ROOTDIR)/shell/source/mfs/sh_format.c
SOURCES += $(MQX_ROOTDIR)/shell/source/mfs/sh_mkdir.c
SOURCES += $(MQX_ROOTDIR)/shell/source/mfs/sh_pwd.c
SOURCES += $(MQX_ROOTDIR)/shell/source/mfs/sh_read.c
SOURCES += $(MQX_ROOTDIR)/shell/source/mfs/sh_readtest.c
SOURCES += $(MQX_ROOTDIR)/shell/source/mfs/sh_rename.c
SOURCES += $(MQX_ROOTDIR)/shell/source/mfs/sh_rmdir.c
SOURCES += $(MQX_ROOTDIR)/shell/source/mfs/sh_type.c
SOURCES += $(MQX_ROOTDIR)/shell/source/mfs/sh_write.c
SOURCES += $(MQX_ROOTDIR)/shell/source/mfs/sh_writetest.c

#-----------------------------------------------------------
# 'mqx' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/shell/source/mqx/sh_abort.c
SOURCES += $(MQX_ROOTDIR)/shell/source/mqx/sh_kill.c
SOURCES += $(MQX_ROOTDIR)/shell/source/mqx/sh_pause.c
SOURCES += $(MQX_ROOTDIR)/shell/source/mqx/sh_tad.c

#-----------------------------------------------------------
# 'rtcs' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_arpdisp.c
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_clock.c
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_echod.c
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_eds.c
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_enet.c
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_exec.c
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_ftpc.c
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_ftpd.c
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_gate.c
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_getroute.c
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_ghbn.c
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_ipconfig.c
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_iwconfig.c
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_load.c
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_nat.c
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_nstat.c
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_ping.c
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_ppp.c
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_rtcs.c
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_sendto.c
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_smtp.c
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_snmpd.c
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_tftp.c
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_tftpd.c
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_tnet.c
SOURCES += $(MQX_ROOTDIR)/shell/source/rtcs/sh_tnetd.c

#-----------------------------------------------------------
# 'shell' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/shell/source/shell/sh_cmdlist.c
SOURCES += $(MQX_ROOTDIR)/shell/source/shell/sh_exit.c
SOURCES += $(MQX_ROOTDIR)/shell/source/shell/sh_help.c
SOURCES += $(MQX_ROOTDIR)/shell/source/shell/sh_sh.c
SOURCES += $(MQX_ROOTDIR)/shell/source/shell/sh_util.c
SOURCES += $(MQX_ROOTDIR)/shell/source/shell/shell.c


#----------------------------------------------------------
# load global settings
#----------------------------------------------------------
include $(MQX_ROOTDIR)/build/common/make/global.mak
-include $(MQX_ROOTDIR)/build/$(BOARD)/make/tools/$(TOOL).mak
include $(MQX_ROOTDIR)/build/common/make/verify.mak
include $(MQX_ROOTDIR)/build/common/make/lib-process.mak

