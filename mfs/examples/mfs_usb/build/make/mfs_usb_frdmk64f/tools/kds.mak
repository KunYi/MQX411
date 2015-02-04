#-----------------------------------------------------------
# libraries
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
LIBRARIES += $(MQX_ROOTDIR)/lib/frdmk64f.kds/debug/bsp/bsp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/frdmk64f.kds/debug/psp/psp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/frdmk64f.kds/debug/mfs/mfs.a
LIBRARIES += $(MQX_ROOTDIR)/lib/frdmk64f.kds/debug/shell/shell.a
LIBRARIES += $(MQX_ROOTDIR)/lib/frdmk64f.kds/debug/usb/usbh.a
endif
ifeq ($(CONFIG),release)
LIBRARIES += $(MQX_ROOTDIR)/lib/frdmk64f.kds/release/bsp/bsp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/frdmk64f.kds/release/psp/psp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/frdmk64f.kds/release/mfs/mfs.a
LIBRARIES += $(MQX_ROOTDIR)/lib/frdmk64f.kds/release/shell/shell.a
LIBRARIES += $(MQX_ROOTDIR)/lib/frdmk64f.kds/release/usb/usbh.a
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
INCLUDE += $(MQX_ROOTDIR)/lib/frdmk64f.kds/debug/bsp/Generated_Code
INCLUDE += $(MQX_ROOTDIR)/lib/frdmk64f.kds/debug/bsp/Sources
INCLUDE += $(MQX_ROOTDIR)/lib/frdmk64f.kds/debug
INCLUDE += $(MQX_ROOTDIR)/lib/frdmk64f.kds/debug/bsp
INCLUDE += $(MQX_ROOTDIR)/lib/frdmk64f.kds/debug/psp
INCLUDE += $(MQX_ROOTDIR)/lib/frdmk64f.kds/debug/mfs
INCLUDE += $(MQX_ROOTDIR)/lib/frdmk64f.kds/debug/shell
INCLUDE += $(MQX_ROOTDIR)/lib/frdmk64f.kds/debug/usb
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/lib/frdmk64f.kds/release/bsp/Generated_Code
INCLUDE += $(MQX_ROOTDIR)/lib/frdmk64f.kds/release/bsp/Sources
INCLUDE += $(MQX_ROOTDIR)/lib/frdmk64f.kds/release
INCLUDE += $(MQX_ROOTDIR)/lib/frdmk64f.kds/release/bsp
INCLUDE += $(MQX_ROOTDIR)/lib/frdmk64f.kds/release/psp
INCLUDE += $(MQX_ROOTDIR)/lib/frdmk64f.kds/release/mfs
INCLUDE += $(MQX_ROOTDIR)/lib/frdmk64f.kds/release/shell
INCLUDE += $(MQX_ROOTDIR)/lib/frdmk64f.kds/release/usb
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





