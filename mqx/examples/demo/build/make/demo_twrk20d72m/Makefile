#----------------------------------------------------------
# setup required variables 
#----------------------------------------------------------
MQX_ROOTDIR         = $(abspath ../../../../../..)
LIBRARY_ROOTDIR     = $(MQX_ROOTDIR)/lib/$(BOARD).$(TOOL)
APPLICATION_DIR     = ./$(TOOL)/$(LOADNAME)_$(CONFIG)
APPLICATION_FILE    = $(APPLICATION_DIR)/$(NAME)_$(BOARD).$(APPLICATION_EXT)
LINKER_FILE         = $(call GET_BSP_LINKER_FILE)
TYPE                = application
NAME                = demo
BOARD               = twrk20d72m


#-----------------------------------------------------------
# load tool settings
#-----------------------------------------------------------
-include tools/$(TOOL).mak


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(MQX_ROOTDIR)/mqx/examples/demo
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/mqx/examples/demo
endif



#-----------------------------------------------------------
# common 'mqx_demo' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/examples/demo/ttl.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/demo/sender.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/demo/semb.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/demo/sema.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/demo/responde.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/demo/mutexb.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/demo/mutexa.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/demo/main.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/demo/eventb.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/demo/eventa.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/demo/demo.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/demo/btimesli.c
SOURCES += $(MQX_ROOTDIR)/mqx/examples/demo/atimesli.c



#----------------------------------------------------------
# include partial makefiles
#----------------------------------------------------------
include $(MQX_ROOTDIR)/build/common/make/global.mak
-include $(MQX_ROOTDIR)/build/$(BOARD)/make/tools/$(TOOL).mak
include $(MQX_ROOTDIR)/build/common/make/verify.mak
include $(MQX_ROOTDIR)/build/common/make/app-process.mak
#----------------------------------------------------------
