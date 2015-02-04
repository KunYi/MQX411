#----------------------------------------------------------
# setup required variables 
#----------------------------------------------------------
MQX_ROOTDIR         = $(abspath ../../../../../../../..)
LIBRARY_ROOTDIR     = $(MQX_ROOTDIR)/lib/$(BOARD).$(TOOL)
APPLICATION_DIR     = ./$(TOOL)/$(LOADNAME)_$(CONFIG)
APPLICATION_FILE    = $(APPLICATION_DIR)/$(NAME)_$(BOARD).$(APPLICATION_EXT)
LINKER_FILE         = $(call GET_BSP_LINKER_FILE)
TYPE                = application
NAME                = speaker_host
BOARD               = frdmk64f


#-----------------------------------------------------------
# load tool settings
#-----------------------------------------------------------
-include tools/$(TOOL).mak


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(MQX_ROOTDIR)/usb/host/examples/audio/speaker
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/usb/host/examples/audio/speaker
endif



#-----------------------------------------------------------
# common 'usbh_audio_speaker' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/usb/host/examples/audio/speaker/audio_speaker.c
SOURCES += $(MQX_ROOTDIR)/usb/host/examples/audio/speaker/sh_audio.c
SOURCES += $(MQX_ROOTDIR)/usb/host/examples/audio/speaker/sd_card.c
SOURCES += $(MQX_ROOTDIR)/usb/host/examples/audio/speaker/hidkeyboard.c
SOURCES += $(MQX_ROOTDIR)/usb/host/examples/audio/speaker/main.c
SOURCES += $(MQX_ROOTDIR)/usb/host/examples/audio/speaker/usb_classes.c
SOURCES += $(MQX_ROOTDIR)/usb/host/examples/audio/speaker/wav.c



#----------------------------------------------------------
# include partial makefiles
#----------------------------------------------------------
include $(MQX_ROOTDIR)/build/common/make/global.mak
-include $(MQX_ROOTDIR)/build/$(BOARD)/make/tools/$(TOOL).mak
include $(MQX_ROOTDIR)/build/common/make/verify.mak
include $(MQX_ROOTDIR)/build/common/make/app-process.mak
#----------------------------------------------------------
