#-----------------------------------------------------------
# libraries
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk60n512.gcc_arm/debug/bsp/bsp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk60n512.gcc_arm/debug/psp/psp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk60n512.gcc_arm/debug/rtcs/rtcs.a
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk60n512.gcc_arm/debug/shell/shell.a
endif
ifeq ($(CONFIG),release)
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk60n512.gcc_arm/release/bsp/bsp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk60n512.gcc_arm/release/psp/psp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk60n512.gcc_arm/release/rtcs/rtcs.a
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk60n512.gcc_arm/release/shell/shell.a
endif


#-----------------------------------------------------------
# runtime libraries
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/4.8.3/armv7e-m/libgcc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/libc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/libsupc++.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/libm.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/libnosys.a
endif
ifeq ($(CONFIG),release)
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/4.8.3/armv7e-m/libgcc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/libc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/libsupc++.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/libm.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/libnosys.a
endif


#-----------------------------------------------------------
# runtime library paths
#-----------------------------------------------------------


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60n512.gcc_arm/debug/bsp/Generated_Code
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60n512.gcc_arm/debug/bsp/Sources
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60n512.gcc_arm/debug
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60n512.gcc_arm/debug/bsp
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60n512.gcc_arm/debug/psp
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60n512.gcc_arm/debug/rtcs
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60n512.gcc_arm/debug/shell
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60n512.gcc_arm/release/bsp/Generated_Code
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60n512.gcc_arm/release/bsp/Sources
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60n512.gcc_arm/release
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60n512.gcc_arm/release/bsp
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60n512.gcc_arm/release/psp
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60n512.gcc_arm/release/rtcs
INCLUDE += $(MQX_ROOTDIR)/lib/twrk60n512.gcc_arm/release/shell
endif


#-----------------------------------------------------------
# runtime search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/4.8.3/include
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/4.8.3/include-fixed
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/include
endif
ifeq ($(CONFIG),release)
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/4.8.3/include
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/4.8.3/include-fixed
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/include
endif





