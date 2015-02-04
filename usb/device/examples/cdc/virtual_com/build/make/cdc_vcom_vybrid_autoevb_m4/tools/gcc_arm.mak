#-----------------------------------------------------------
# libraries
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
LIBRARIES += $(MQX_ROOTDIR)/lib/vybrid_autoevb_m4.gcc_arm/debug/bsp/bsp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/vybrid_autoevb_m4.gcc_arm/debug/psp/psp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/vybrid_autoevb_m4.gcc_arm/debug/usb/usbd.a
endif
ifeq ($(CONFIG),release)
LIBRARIES += $(MQX_ROOTDIR)/lib/vybrid_autoevb_m4.gcc_arm/release/bsp/bsp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/vybrid_autoevb_m4.gcc_arm/release/psp/psp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/vybrid_autoevb_m4.gcc_arm/release/usb/usbd.a
endif


#-----------------------------------------------------------
# runtime libraries
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/$(GCC_REV)/armv7e-m/fpu/libgcc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/fpu/libc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/fpu/libsupc++.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/fpu/libm.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/fpu/libnosys.a
endif
ifeq ($(CONFIG),release)
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/$(GCC_REV)/armv7e-m/fpu/libgcc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/fpu/libc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/fpu/libsupc++.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/fpu/libm.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/fpu/libnosys.a
endif


#-----------------------------------------------------------
# runtime library paths
#-----------------------------------------------------------


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(MQX_ROOTDIR)/lib/vybrid_autoevb_m4.gcc_arm/debug/bsp/Generated_Code
INCLUDE += $(MQX_ROOTDIR)/lib/vybrid_autoevb_m4.gcc_arm/debug/bsp/Sources
INCLUDE += $(MQX_ROOTDIR)/lib/vybrid_autoevb_m4.gcc_arm/debug
INCLUDE += $(MQX_ROOTDIR)/lib/vybrid_autoevb_m4.gcc_arm/debug/bsp
INCLUDE += $(MQX_ROOTDIR)/lib/vybrid_autoevb_m4.gcc_arm/debug/psp
INCLUDE += $(MQX_ROOTDIR)/lib/vybrid_autoevb_m4.gcc_arm/debug/usb
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/lib/vybrid_autoevb_m4.gcc_arm/release/bsp/Generated_Code
INCLUDE += $(MQX_ROOTDIR)/lib/vybrid_autoevb_m4.gcc_arm/release/bsp/Sources
INCLUDE += $(MQX_ROOTDIR)/lib/vybrid_autoevb_m4.gcc_arm/release
INCLUDE += $(MQX_ROOTDIR)/lib/vybrid_autoevb_m4.gcc_arm/release/bsp
INCLUDE += $(MQX_ROOTDIR)/lib/vybrid_autoevb_m4.gcc_arm/release/psp
INCLUDE += $(MQX_ROOTDIR)/lib/vybrid_autoevb_m4.gcc_arm/release/usb
endif


#-----------------------------------------------------------
# runtime search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/$(GCC_REV)/include-fixed
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/$(GCC_REV)/include
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/include
endif
ifeq ($(CONFIG),release)
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/$(GCC_REV)/include-fixed
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/$(GCC_REV)/include
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/include
endif





