#----------------------------------------------------------
# setup required variables 
#----------------------------------------------------------
MQX_ROOTDIR         = $(abspath ../../../../../..)
LIBRARY_ROOTDIR     = $(MQX_ROOTDIR)/lib/$(BOARD).$(TOOL)
APPLICATION_DIR     = ./$(TOOL)/$(LOADNAME)_$(CONFIG)
APPLICATION_FILE    = $(APPLICATION_DIR)/$(NAME)_$(BOARD).$(APPLICATION_EXT)
LINKER_FILE         = $(call GET_BSP_LINKER_FILE)
TYPE                = application
NAME                = lwdemo
BOARD               = twrk40d100m


#-----------------------------------------------------------
# load tool settings
#-----------------------------------------------------------
-include tools/$(TOOL).mak


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(MQX_ROOTDIR)/mqx/examples/lwdemo
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/mqx/examples/lwdemo
endif



#-----------------------------------------------------------
# common 'mqx_lwdemo' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/examples/lwdemo/ttl.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/lwdemo/sender.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/lwdemo/responde.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/lwdemo/main.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/lwdemo/lwsemb.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/lwdemo/lwsema.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/lwdemo/lweventb.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/lwdemo/lweventa.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/lwdemo/lwdemo.c



#----------------------------------------------------------
# include partial makefiles
#----------------------------------------------------------
include $(MQX_ROOTDIR)/build/common/make/global.mak
-include $(MQX_ROOTDIR)/build/$(BOARD)/make/tools/$(TOOL).mak
include $(MQX_ROOTDIR)/build/common/make/verify.mak
include $(MQX_ROOTDIR)/build/common/make/app-process.mak
#----------------------------------------------------------
