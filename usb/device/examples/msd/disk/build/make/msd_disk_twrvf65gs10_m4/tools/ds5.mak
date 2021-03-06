#-----------------------------------------------------------
# libraries
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
LIBRARIES += $(MQX_ROOTDIR)/lib/twrvf65gs10_m4.ds5/debug/bsp/bsp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/twrvf65gs10_m4.ds5/debug/bsp/bsp.a(vectors.o)
LIBRARIES += $(MQX_ROOTDIR)/lib/twrvf65gs10_m4.ds5/debug/psp/psp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/twrvf65gs10_m4.ds5/debug/psp/psp.a(linker_symbols.o)
LIBRARIES += $(MQX_ROOTDIR)/lib/twrvf65gs10_m4.ds5/debug/usb/usbd.a
endif
ifeq ($(CONFIG),release)
LIBRARIES += $(MQX_ROOTDIR)/lib/twrvf65gs10_m4.ds5/release/bsp/bsp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/twrvf65gs10_m4.ds5/release/bsp/bsp.a(vectors.o)
LIBRARIES += $(MQX_ROOTDIR)/lib/twrvf65gs10_m4.ds5/release/psp/psp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/twrvf65gs10_m4.ds5/release/psp/psp.a(linker_symbols.o)
LIBRARIES += $(MQX_ROOTDIR)/lib/twrvf65gs10_m4.ds5/release/usb/usbd.a
endif


#-----------------------------------------------------------
# runtime libraries
#-----------------------------------------------------------


#-----------------------------------------------------------
# runtime library paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
RT_PATHS += $(TOOLCHAIN_ROOTDIR)/lib/
endif
ifeq ($(CONFIG),release)
RT_PATHS += $(TOOLCHAIN_ROOTDIR)/lib/
endif


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(MQX_ROOTDIR)/lib/twrvf65gs10_m4.ds5/debug/bsp/Generated_Code
INCLUDE += $(MQX_ROOTDIR)/lib/twrvf65gs10_m4.ds5/debug/bsp/Sources
INCLUDE += $(MQX_ROOTDIR)/lib/twrvf65gs10_m4.ds5/debug
INCLUDE += $(MQX_ROOTDIR)/lib/twrvf65gs10_m4.ds5/debug/bsp
INCLUDE += $(MQX_ROOTDIR)/lib/twrvf65gs10_m4.ds5/debug/psp
INCLUDE += $(MQX_ROOTDIR)/lib/twrvf65gs10_m4.ds5/debug/usb
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/lib/twrvf65gs10_m4.ds5/release/bsp/Generated_Code
INCLUDE += $(MQX_ROOTDIR)/lib/twrvf65gs10_m4.ds5/release/bsp/Sources
INCLUDE += $(MQX_ROOTDIR)/lib/twrvf65gs10_m4.ds5/release
INCLUDE += $(MQX_ROOTDIR)/lib/twrvf65gs10_m4.ds5/release/bsp
INCLUDE += $(MQX_ROOTDIR)/lib/twrvf65gs10_m4.ds5/release/psp
INCLUDE += $(MQX_ROOTDIR)/lib/twrvf65gs10_m4.ds5/release/usb
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





