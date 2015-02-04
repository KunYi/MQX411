#----------------------------------------------------------
# setup required variables 
#----------------------------------------------------------
MQX_ROOTDIR         = $(abspath ../../../../..)
LIBRARY_ROOTDIR     = $(MQX_ROOTDIR)/lib/$(BOARD).$(TOOL)
APPLICATION_DIR     = ./$(TOOL)/$(LOADNAME)_$(CONFIG)
APPLICATION_FILE    = $(APPLICATION_DIR)/$(NAME)_$(BOARD).$(APPLICATION_EXT)
LINKER_FILE         = $(call GET_BSP_LINKER_FILE)
TYPE                = application
NAME                = web_hvac
BOARD               = twrk60f120m


#-----------------------------------------------------------
# load tool settings
#-----------------------------------------------------------
-include tools/$(TOOL).mak


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(MQX_ROOTDIR)/demo/web_hvac
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/demo/web_hvac
endif



#-----------------------------------------------------------
# common 'demo_web_hvac' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/demo/web_hvac/command_lists.c
SOURCES += $(MQX_ROOTDIR)/demo/web_hvac/hvac_io.c
SOURCES += $(MQX_ROOTDIR)/demo/web_hvac/hvac_shell_commands.c
SOURCES += $(MQX_ROOTDIR)/demo/web_hvac/hvac_task.c
SOURCES += $(MQX_ROOTDIR)/demo/web_hvac/hvac_util.c
SOURCES += $(MQX_ROOTDIR)/demo/web_hvac/logging_task.c
SOURCES += $(MQX_ROOTDIR)/demo/web_hvac/rtcs.c
SOURCES += $(MQX_ROOTDIR)/demo/web_hvac/shell_task.c
SOURCES += $(MQX_ROOTDIR)/demo/web_hvac/switch_task.c
SOURCES += $(MQX_ROOTDIR)/demo/web_hvac/tasks.c
SOURCES += $(MQX_ROOTDIR)/demo/web_hvac/usb_file.c
SOURCES += $(MQX_ROOTDIR)/demo/web_hvac/usb_task.c
SOURCES += $(MQX_ROOTDIR)/demo/web_hvac/cgi_hvac.c
SOURCES += $(MQX_ROOTDIR)/demo/web_hvac/cgi_index.c
SOURCES += $(MQX_ROOTDIR)/demo/web_hvac/tfs_data.c
SOURCES += $(MQX_ROOTDIR)/demo/web_hvac/usb_classes.c



#----------------------------------------------------------
# include partial makefiles
#----------------------------------------------------------
include $(MQX_ROOTDIR)/build/common/make/global.mak
-include $(MQX_ROOTDIR)/build/$(BOARD)/make/tools/$(TOOL).mak
include $(MQX_ROOTDIR)/build/common/make/verify.mak
include $(MQX_ROOTDIR)/build/common/make/app-process.mak
#----------------------------------------------------------
