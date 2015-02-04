#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_a/compiler/rv_ds5
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_a/compiler/rv_ds5
endif


#-----------------------------------------------------------
# runtime search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/include
endif
ifeq ($(CONFIG),release)
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/include
endif


#-----------------------------------------------------------
# ds5 sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/psp/cortex_a/compiler/rv_ds5/comp.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/psp/cortex_a/compiler/rv_ds5/linker_symbols.S


#-----------------------------------------------------------
# 'debug' configuration settings
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
AS_DEFINE += PSP_HAS_SUPPORT_STRUCT=1 
CC_DEFINE += MQX_DISABLE_CONFIG_CHECK=1 PSP_HAS_SUPPORT_STRUCT=1 
CX_DEFINE += MQX_DISABLE_CONFIG_CHECK=1 PSP_HAS_SUPPORT_STRUCT=1 
endif


#-----------------------------------------------------------
# 'release' configuration settings
#-----------------------------------------------------------
ifeq ($(CONFIG),release)
AS_DEFINE += PSP_HAS_SUPPORT_STRUCT=1 
CC_DEFINE += MQX_DISABLE_CONFIG_CHECK=1 PSP_HAS_SUPPORT_STRUCT=1 
CX_DEFINE += MQX_DISABLE_CONFIG_CHECK=1 PSP_HAS_SUPPORT_STRUCT=1 
endif


