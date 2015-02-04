#-----------------------------------------------------------
# libraries
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk60f120m.kds/debug/bsp/bsp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk60f120m.kds/debug/psp/psp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk60f120m.kds/debug/mfs/mfs.a
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk60f120m.kds/debug/ffs/ffs.a
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk60f120m.kds/debug/shell/shell.a
endif
ifeq ($(CONFIG),release)
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk60f120m.kds/release/bsp/bsp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk60f120m.kds/release/psp/psp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk60f120m.kds/release/mfs/mfs.a
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk60f120m.kds/release/ffs/ffs.a
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk60f120m.kds/release/shell/shell.a
endif


#-----------------------------------------------------------
# runtime libraries
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/toolchain/lib/gcc/arm-none-eabi/4.8.0/m4/fp/v4-sp-d16/libgcc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/toolchain/arm-none-eabi/lib/m4/fp/v4-sp-d16/libc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/toolchain/arm-none-eabi/lib/m4/fp/v4-sp-d16/libsupc++.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/toolchain/arm-none-eabi/lib/m4/fp/v4-sp-d16/libm.a
endif
ifeq ($(CONFIG),release)
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/toolchain/lib/gcc/arm-none-eabi/4.8.0/m4/fp/v4-sp-d16/libgcc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/toolchain/arm-none-eabi/lib/m4/fp/v4-sp-d16/libc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/toolchain/arm-none-eabi/lib/m4/fp/v4-sp-d16/libsupc++.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/toolchain/arm-none-eabi/lib/m4/fp/v4-sp-d16/libm.a
endif


#-----------------------------------------------------------
# runtime library paths
#-----------------------------------------------------------


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60f120m.kds/debug
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60f120m.kds/debug/bsp
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60f120m.kds/debug/bsp/Generated_Code
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60f120m.kds/debug/psp
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60f120m.kds/debug/mfs
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60f120m.kds/debug/ffs
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60f120m.kds/debug/shell
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60f120m.kds/release
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60f120m.kds/release/bsp
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60f120m.kds/release/bsp/Generated_Code
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60f120m.kds/release/psp
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60f120m.kds/release/mfs
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60f120m.kds/release/ffs
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60f120m.kds/release/shell
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





