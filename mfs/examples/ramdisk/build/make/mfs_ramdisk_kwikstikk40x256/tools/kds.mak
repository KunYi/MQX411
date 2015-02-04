#-----------------------------------------------------------
# libraries
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
LIBRARIES += $(MQX_ROOTDIR)/lib/kwikstikk40x256.kds/debug/bsp/bsp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/kwikstikk40x256.kds/debug/psp/psp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/kwikstikk40x256.kds/debug/mfs/mfs.a
LIBRARIES += $(MQX_ROOTDIR)/lib/kwikstikk40x256.kds/debug/shell/shell.a
endif
ifeq ($(CONFIG),release)
LIBRARIES += $(MQX_ROOTDIR)/lib/kwikstikk40x256.kds/release/bsp/bsp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/kwikstikk40x256.kds/release/psp/psp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/kwikstikk40x256.kds/release/mfs/mfs.a
LIBRARIES += $(MQX_ROOTDIR)/lib/kwikstikk40x256.kds/release/shell/shell.a
endif


#-----------------------------------------------------------
# runtime libraries
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/toolchain/lib/gcc/arm-none-eabi/4.8.0/m4/libgcc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/toolchain/arm-none-eabi/lib/m4/libc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/toolchain/arm-none-eabi/lib/m4/libsupc++.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/toolchain/arm-none-eabi/lib/m4/libm.a
endif
ifeq ($(CONFIG),release)
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/toolchain/lib/gcc/arm-none-eabi/4.8.0/m4/libgcc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/toolchain/arm-none-eabi/lib/m4/libc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/toolchain/arm-none-eabi/lib/m4/libsupc++.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/toolchain/arm-none-eabi/lib/m4/libm.a
endif


#-----------------------------------------------------------
# runtime library paths
#-----------------------------------------------------------


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(MQX_ROOTDIR)/lib/kwikstikk40x256.kds/debug/bsp/Generated_Code
INCLUDE += $(MQX_ROOTDIR)/lib/kwikstikk40x256.kds/debug/bsp/Sources
INCLUDE += $(MQX_ROOTDIR)/lib/kwikstikk40x256.kds/debug
INCLUDE += $(MQX_ROOTDIR)/lib/kwikstikk40x256.kds/debug/bsp
INCLUDE += $(MQX_ROOTDIR)/lib/kwikstikk40x256.kds/debug/psp
INCLUDE += $(MQX_ROOTDIR)/lib/kwikstikk40x256.kds/debug/mfs
INCLUDE += $(MQX_ROOTDIR)/lib/kwikstikk40x256.kds/debug/shell
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/lib/kwikstikk40x256.kds/release/bsp/Generated_Code
INCLUDE += $(MQX_ROOTDIR)/lib/kwikstikk40x256.kds/release/bsp/Sources
INCLUDE += $(MQX_ROOTDIR)/lib/kwikstikk40x256.kds/release
INCLUDE += $(MQX_ROOTDIR)/lib/kwikstikk40x256.kds/release/bsp
INCLUDE += $(MQX_ROOTDIR)/lib/kwikstikk40x256.kds/release/psp
INCLUDE += $(MQX_ROOTDIR)/lib/kwikstikk40x256.kds/release/mfs
INCLUDE += $(MQX_ROOTDIR)/lib/kwikstikk40x256.kds/release/shell
endif


#-----------------------------------------------------------
# runtime search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/toolchain/lib/gcc/arm-none-eabi/4.8.0/include
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/toolchain/lib/gcc/arm-none-eabi/4.8.0/include-fixed
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/toolchain/arm-none-eabi/include
endif
ifeq ($(CONFIG),release)
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/toolchain/lib/gcc/arm-none-eabi/4.8.0/include
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/toolchain/lib/gcc/arm-none-eabi/4.8.0/include-fixed
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/toolchain/arm-none-eabi/include
endif





