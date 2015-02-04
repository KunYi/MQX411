#----------------------------------------------------------
# make in versions 3.80 3.81 doesn't work with absolute 
# windows paths - problem with $(abspath),$(abspath),vpath
#----------------------------------------------------------
# # # if make version is less than 3.68
# ifeq ($(MAKE_VERSION),)
# $(warning "your version of 'make' is unsupported, please use 382 or higher")
# else
# # if make version is less than expected
# 
# MAKEVERSION = $(MAKE_VERSION)
# MAKEVERSION := $(subst ., ,$(MAKEVERSION))
# MAKEVERSION := $(word 1,$(MAKEVERSION))$(word 2,$(MAKEVERSION))
# ifneq (382, $(word 1,$(sort 382 $(MAKEVERSION))))
# $(warning "your version of 'make' is unsupported, please use 382 or higher")
# endif
# endif


#----------------------------------------------------------
# check for valid board
# - obtain expected board-directory list
# - remove invalid items
# - strip - use single space delimiter
# - add space to end of the list because of $(find)
#----------------------------------------------------------
ifneq ($(MAKECMDGOALS),)
BOARD_LIST := $(dir $(wildcard $(MQX_ROOTDIR)/build/*/make/))
BOARD_LIST := $(subst $(MQX_ROOTDIR)/build/,,$(BOARD_LIST))
BOARD_LIST := $(subst /make/,,$(BOARD_LIST))
BOARD_LIST := $(subst common,,$(BOARD_LIST))
BOARD_LIST := $(strip $(BOARD_LIST))
BOARD_LIST := $(BOARD_LIST) 

ifeq ($(BOARD),)
$(error "BOARD not set, use one of : $(BOARD_LIST) ")
endif
ifneq ($(findstring $(BOARD) ,$(BOARD_LIST)),$(BOARD) )
$(error "BOARD [$(BOARD)] is not supported, use one of : $(BOARD_LIST) ")
endif
endif


#----------------------------------------------------------
# check for valid tool
# - obtain expected board-directory list
# - strip - use single space delimiter
# - add space to end of the list because of $(find)
#----------------------------------------------------------
ifneq ($(MAKECMDGOALS),)
TOOL_LIST := $(wildcard $(MQX_ROOTDIR)/build/$(BOARD)/make/tools/*.mak)
TOOL_LIST := $(basename $(notdir $(TOOL_LIST)))
TOOL_LIST := $(strip $(TOOL_LIST))
TOOL_LIST := $(TOOL_LIST) 

ifeq ($(TOOL),)
$(error "TOOL not set, use one of: $(TOOL_LIST) ")
endif
ifneq ($(findstring $(TOOL) ,$(TOOL_LIST)),$(TOOL) )
$(error "TOOL [$(TOOL)] is not supported, use one of : $(TOOL_LIST) ")
endif
endif


#----------------------------------------------------------
# check whether CONFIG is valid to avoid typo
# CONFIG_LIST is set in $(TOOL).mak
# - strip - use single space delimiter
# - add space to end of the list because of $(find)
#----------------------------------------------------------
# ifneq ($(MAKECMDGOALS),)
# CONFIG_LIST := $(strip $(CONFIG_LIST))
# CONFIG_LIST := $(CONFIG_LIST) 

# ifeq ($(CONFIG),)
# $(error "CONFIG not set, use one of : $(CONFIG_LIST) ")
# endif
# ifneq ($(findstring $(CONFIG) ,$(CONFIG_LIST)),$(CONFIG) )
# $(error "CONFIG [$(CONFIG)] is not supported, use one of : $(CONFIG_LIST) ")
# endif
# endif


#----------------------------------------------------------
# check toolchain directory & tool binaries
#----------------------------------------------------------
ifeq ($(MAKECMDGOALS),build)
ifeq ($(TOOLCHAIN_ROOTDIR),)
$(error "TOOLCHAIN_ROOTDIR is empty, modify/uncomment variable in: global.mak or build/$(BOARD)/make/tools/$(TOOL).mak")
endif
ifeq ($(wildcard $(CC)),)
$(error "CC path doesn't exists:  $(CC)")
endif
ifeq ($(wildcard $(AS)),)
$(error "AS path doesn't exists:  $(AS)")
endif
ifeq ($(wildcard $(AR)),)
$(error "AR path doesn't exists:  $(AR)")
endif
ifeq ($(wildcard $(LD)),)
$(error "LD path doesn't exists:  $(LD)")
endif
endif


#----------------------------------------------------------
# check linker command file
# LOAD is always defined for an application Makefile(s)
# LINKER_FILE contains full path to linker command file
#----------------------------------------------------------
ifneq ($(MAKECMDGOALS),)
ifeq ($(TYPE),application)
ifeq ($(LINKER_FILE),)
ifeq ($(LOAD),)
$(error "LINKER_FILE is empty. Set LINKER_FILE directly or use LOAD")
else
$(error "LINKER_FILE is empty. Cannot find linker file by LOAD: $(LOAD)")
endif
else
ifeq ($(wildcard $(LINKER_FILE)),)
$(error "LINKER_FILE doesn't exists: $(LINKER_FILE)")
endif
endif
endif
endif


#----------------------------------------------------------
# setup sed
#----------------------------------------------------------
ifeq ($(MAKECMDGOALS),build)
# if SED macro is empty
ifeq ($(SED),)
$(error "Please setup 'SED' in global.mak. '$(SED)' does not exists")
endif
# if SED is filepath and filepath does not exists
ifneq ($(notdir $(SED)),$(strip $(SED)))
ifeq ($(wildcard $(SED)),)
$(error "Please setup 'SED' in global.mak. '$(SED)' does not exists")
endif
endif
endif
