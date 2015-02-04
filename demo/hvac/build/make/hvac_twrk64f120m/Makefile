#----------------------------------------------------------
# setup required variables 
#----------------------------------------------------------
MQX_ROOTDIR         = $(abspath ../../../../..)
LIBRARY_ROOTDIR     = $(MQX_ROOTDIR)/lib/$(BOARD).$(TOOL)
APPLICATION_DIR     = ./$(TOOL)/$(LOADNAME)_$(CONFIG)
APPLICATION_FILE    = $(APPLICATION_DIR)/$(NAME)_$(BOARD).$(APPLICATION_EXT)
LINKER_FILE         = $(call GET_BSP_LINKER_FILE)
TYPE                = application
NAME                = hvac
BOARD               = twrk64f120m


#-----------------------------------------------------------
# load tool settings
#-----------------------------------------------------------
-include tools/$(TOOL).mak


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(MQX_ROOTDIR)/demo/hvac
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/demo/hvac
endif



#-----------------------------------------------------------
# common 'demo_hvac' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/demo/hvac/command_lists.c
SOURCES += $(MQX_ROOTDIR)/demo/hvac/hvac_io.c
SOURCES += $(MQX_ROOTDIR)/demo/hvac/hvac_shell_commands.c
SOURCES += $(MQX_ROOTDIR)/demo/hvac/hvac_task.c
SOURCES += $(MQX_ROOTDIR)/demo/hvac/hvac_util.c
SOURCES += $(MQX_ROOTDIR)/demo/hvac/logging_task.c
SOURCES += $(MQX_ROOTDIR)/demo/hvac/rtcs.c
SOURCES += $(MQX_ROOTDIR)/demo/hvac/shell_task.c
SOURCES += $(MQX_ROOTDIR)/demo/hvac/switch_task.c
SOURCES += $(MQX_ROOTDIR)/demo/hvac/tasks.c
SOURCES += $(MQX_ROOTDIR)/demo/hvac/usb_file.c
SOURCES += $(MQX_ROOTDIR)/demo/hvac/usb_task.c
SOURCES += $(MQX_ROOTDIR)/demo/hvac/usb_classes.c



#----------------------------------------------------------
# include partial makefiles
#----------------------------------------------------------
include $(MQX_ROOTDIR)/build/common/make/global.mak
-include $(MQX_ROOTDIR)/build/$(BOARD)/make/tools/$(TOOL).mak
include $(MQX_ROOTDIR)/build/common/make/verify.mak
include $(MQX_ROOTDIR)/build/common/make/app-process.mak
#----------------------------------------------------------
