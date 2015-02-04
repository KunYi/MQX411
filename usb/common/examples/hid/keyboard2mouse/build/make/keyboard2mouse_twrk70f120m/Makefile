#----------------------------------------------------------
# setup required variables 
#----------------------------------------------------------
MQX_ROOTDIR         = $(abspath ../../../../../../../..)
LIBRARY_ROOTDIR     = $(MQX_ROOTDIR)/lib/$(BOARD).$(TOOL)
APPLICATION_DIR     = ./$(TOOL)/$(LOADNAME)_$(CONFIG)
APPLICATION_FILE    = $(APPLICATION_DIR)/$(NAME)_$(BOARD).$(APPLICATION_EXT)
LINKER_FILE         = $(call GET_BSP_LINKER_FILE)
TYPE                = application
NAME                = keyboard2mouse
BOARD               = twrk70f120m


#-----------------------------------------------------------
# load tool settings
#-----------------------------------------------------------
-include tools/$(TOOL).mak


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(MQX_ROOTDIR)/usb/common/examples/hid/keyboard2mouse
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/usb/common/examples/hid/keyboard2mouse
endif



#-----------------------------------------------------------
# common 'keyboard2mouse' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/usb/common/examples/hid/keyboard2mouse/hidkeyboard.c
SOURCES += $(MQX_ROOTDIR)/usb/common/examples/hid/keyboard2mouse/mouse.c
SOURCES += $(MQX_ROOTDIR)/usb/common/examples/hid/keyboard2mouse/usb_classes.c
SOURCES += $(MQX_ROOTDIR)/usb/common/examples/hid/keyboard2mouse/usb_descriptor.c



#----------------------------------------------------------
# include partial makefiles
#----------------------------------------------------------
include $(MQX_ROOTDIR)/build/common/make/global.mak
-include $(MQX_ROOTDIR)/build/$(BOARD)/make/tools/$(TOOL).mak
include $(MQX_ROOTDIR)/build/common/make/verify.mak
include $(MQX_ROOTDIR)/build/common/make/app-process.mak
#----------------------------------------------------------
