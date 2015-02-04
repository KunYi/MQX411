#----------------------------------------------------------
# setup required variables 
#----------------------------------------------------------
MQX_ROOTDIR         = $(abspath ../../../../../..)
LIBRARY_ROOTDIR     = $(MQX_ROOTDIR)/lib/$(BOARD).$(TOOL)
APPLICATION_DIR     = ./$(TOOL)/$(LOADNAME)_$(CONFIG)
APPLICATION_FILE    = $(APPLICATION_DIR)/$(NAME)_$(BOARD).$(APPLICATION_EXT)
LINKER_FILE         = $(call GET_BSP_LINKER_FILE)
TYPE                = application
NAME                = lwdemo_usr
BOARD               = twrk60d100m


#-----------------------------------------------------------
# load tool settings
#-----------------------------------------------------------
-include tools/$(TOOL).mak


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(MQX_ROOTDIR)/mqx/examples/lwdemo_usr
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/mqx/examples/lwdemo_usr
endif



#-----------------------------------------------------------
# common 'mqx_lwdemo_usr' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/examples/lwdemo_usr/lwdemo.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/lwdemo_usr/lweventa.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/lwdemo_usr/lweventb.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/lwdemo_usr/lwsema.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/lwdemo_usr/lwsemb.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/lwdemo_usr/main.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/lwdemo_usr/responde.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/lwdemo_usr/sender.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/lwdemo_usr/ttl.c



#----------------------------------------------------------
# include partial makefiles
#----------------------------------------------------------
include $(MQX_ROOTDIR)/build/common/make/global.mak
-include $(MQX_ROOTDIR)/build/$(BOARD)/make/tools/$(TOOL).mak
include $(MQX_ROOTDIR)/build/common/make/verify.mak
include $(MQX_ROOTDIR)/build/common/make/app-process.mak
#----------------------------------------------------------
