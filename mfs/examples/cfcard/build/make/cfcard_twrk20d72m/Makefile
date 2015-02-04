#----------------------------------------------------------
# setup required variables 
#----------------------------------------------------------
MQX_ROOTDIR         = $(abspath ../../../../../..)
LIBRARY_ROOTDIR     = $(MQX_ROOTDIR)/lib/$(BOARD).$(TOOL)
APPLICATION_DIR     = ./$(TOOL)/$(LOADNAME)_$(CONFIG)
APPLICATION_FILE    = $(APPLICATION_DIR)/$(NAME)_$(BOARD).$(APPLICATION_EXT)
LINKER_FILE         = $(call GET_BSP_LINKER_FILE)
TYPE                = application
NAME                = cfcard
BOARD               = twrk20d72m


#-----------------------------------------------------------
# load tool settings
#-----------------------------------------------------------
-include tools/$(TOOL).mak


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(MQX_ROOTDIR)/mfs/examples/cfcard
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/mfs/examples/cfcard
endif



#-----------------------------------------------------------
# common 'mfs_cfcard' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mfs/examples/cfcard/cfcard_file.c
SOURCES += $(MQX_ROOTDIR)/mfs/examples/cfcard/cfcard_task.c
SOURCES += $(MQX_ROOTDIR)/mfs/examples/cfcard/shell_task.c
SOURCES += $(MQX_ROOTDIR)/mfs/examples/cfcard/tasks.c



#----------------------------------------------------------
# include partial makefiles
#----------------------------------------------------------
include $(MQX_ROOTDIR)/build/common/make/global.mak
-include $(MQX_ROOTDIR)/build/$(BOARD)/make/tools/$(TOOL).mak
include $(MQX_ROOTDIR)/build/common/make/verify.mak
include $(MQX_ROOTDIR)/build/common/make/app-process.mak
#----------------------------------------------------------
