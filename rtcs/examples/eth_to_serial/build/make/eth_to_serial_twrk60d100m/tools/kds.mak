#-----------------------------------------------------------
# libraries
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk60d100m.kds/debug/bsp/bsp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk60d100m.kds/debug/psp/psp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk60d100m.kds/debug/rtcs/rtcs.a
endif
ifeq ($(CONFIG),release)
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk60d100m.kds/release/bsp/bsp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk60d100m.kds/release/psp/psp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk60d100m.kds/release/rtcs/rtcs.a
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
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60d100m.kds/debug/bsp/Generated_Code
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60d100m.kds/debug/bsp/Sources
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60d100m.kds/debug
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60d100m.kds/debug/bsp
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60d100m.kds/debug/psp
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60d100m.kds/debug/rtcs
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60d100m.kds/release/bsp/Generated_Code
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60d100m.kds/release/bsp/Sources
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60d100m.kds/release
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60d100m.kds/release/bsp
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60d100m.kds/release/psp
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60d100m.kds/release/rtcs
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





