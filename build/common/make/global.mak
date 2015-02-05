#----------------------------------------------------------
# internal host enviroment
# valid values: WINDOWS UNIX
#----------------------------------------------------------
HOSTENV = UNIX
# HOSTENV = WINDOWS
ifeq ($(GCC_REV),)
GCC_REV = 4.9.3
endif
#----------------------------------------------------------
# set default path to toolchain
#----------------------------------------------------------
# ifeq ($(TOOL),cw10)
# TOOLCHAIN_ROOTDIR = C:/FREESC~1/CWMCUV~1.2
# endif
# ifeq ($(TOOL),uv4)
# TOOLCHAIN_ROOTDIR = C:/KEIL
# endif
# ifeq ($(TOOL),iar)
# TOOLCHAIN_ROOTDIR = C:/PROGRA~1/IARSYS~1/EMBEDD~1.4
# endif
# ifeq ($(TOOL),cw10gcc)
# TOOLCHAIN_ROOTDIR = C:/FREESC~1/CWMCUV~1.4
# endif
# ifeq ($(TOOL),gcc_cs)
# TOOLCHAIN_ROOTDIR = C:/PROGRA~1/CODESO~1/SOURCE~2
# endif
ifeq ($(TOOL),gcc_arm)
# GCC ARM Embedded 4.9 2014q4
ifeq ("$(GCC_REV)", "4.9.3")
TOOLCHAIN_ROOTDIR = /home/kunyi/emgcc/gcc-arm-none-eabi-4_9-2014q4
endif
# GCC ARM Embedded 4.8.4 2014q3
ifeq ("$(GCC_REV)", "4.8.4")
TOOLCHAIN_ROOTDIR = /home/kunyi/emgcc/gcc-arm-none-eabi-4_8-2014q3
endif
# GCC ARM Embedded 4.8.3 2014q1	
ifeq ("$(GCC_REV)", "4.8.3")
TOOLCHAIN_ROOTDIR = /home/kunyi/emgcc/gcc-arm-none-eabi-4_8-2014q1
endif
endif
# ifeq ($(TOOL),kds)
# TOOLCHAIN_ROOTDIR = C:/FREESC~1/KDS_1.0
# endif
# ifeq ($(TOOL),ds5)
# TOOLCHAIN_ROOTDIR = C:/PROGRA~1/DS-5
# endif

#----------------------------------------------------------
# macro, convert slash to backslash
# $(1) - processed string
#----------------------------------------------------------
CONVERT2BACKSLASH = $(subst /,\,$(1))

#----------------------------------------------------------
# macro, convert backslash to slash
# $(1) - processed string
#----------------------------------------------------------
CONVERT2SLASH = $(subst \,/,$(1))

#----------------------------------------------------------
# macro, get linker command file from global variables
#----------------------------------------------------------
ifneq ($(LOAD),)
GET_BSP_LINKER_FILE = $(firstword $(wildcard $(LIBRARY_ROOTDIR)/$(CONFIG)/bsp/$(LOAD).*))
else
GET_BSP_LINKER_FILE = 
endif

#----------------------------------------------------------
# macro, get sed - like tool
# $(1) - param
#----------------------------------------------------------
ifeq ($(HOSTENV),WINDOWS)
# using a full path to sed instead of adding "c:/MinGW/msys/1.0/bin"
# to PATH. msys/1.0/bin directory contains unix utilities which
# are in conflict with native windows utilities
SED = c:/MinGW/msys/1.0/bin/sed.exe $(1)
endif
ifeq ($(HOSTENV),UNIX)
SED = sed $(1)
endif

#----------------------------------------------------------
# macro, create recursive directory
# $(1) - path to directory
#----------------------------------------------------------
ifeq ($(HOSTENV),WINDOWS)
MKDIR = mkdir $(call CONVERT2BACKSLASH,$(1))
endif
ifeq ($(HOSTENV),UNIX)
MKDIR = mkdir -p $(1)
endif

#----------------------------------------------------------
# macro, force remove recursive directory
# $(1) - path to directory
#----------------------------------------------------------
ifeq ($(HOSTENV),WINDOWS)
RMDIR = rd /s /q $(call CONVERT2BACKSLASH,$(1))
endif
ifeq ($(HOSTENV),UNIX)
RMDIR = rm -rf $(1)
endif

#----------------------------------------------------------
# macro, list directory
# $(1) - path to directory
#----------------------------------------------------------
ifeq ($(HOSTENV),WINDOWS)
LSDIR = dir $(call CONVERT2BACKSLASH,$(1)) /w/b/s
endif
ifeq ($(HOSTENV),UNIX)
LSDIR = find $(1) -type f
endif

#----------------------------------------------------------
# macro, print message
# $(1) - message
#----------------------------------------------------------
ifeq ($(HOSTENV),WINDOWS)
PRINT = $(if $(1),@echo $(1),@cmd /C "@echo:")
endif
ifeq ($(HOSTENV),UNIX)
PRINT = @echo $(1)
endif

