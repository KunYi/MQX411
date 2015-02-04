#------------------------------------------------------------
# use specific TOOLCHAIN_ROOTDIR if not set in global.mak
#------------------------------------------------------------
#TOOLCHAIN_ROOTDIR = C:/PROGRA~1/DS-5

#------------------------------------------------------------
# toolchain settings for windows
#
# XX_PROCESS macros:
# $(1) - output file path
# $(2) - list of object files
# $(3) - file contains list of object files
#------------------------------------------------------------
ifeq ($(HOSTENV),WINDOWS)

AS = $(TOOLCHAIN_ROOTDIR)/bin/armasm.exe 
CC = $(TOOLCHAIN_ROOTDIR)/bin/armcc.exe 
CX = $(TOOLCHAIN_ROOTDIR)/bin/armcc.exe 
AR = $(TOOLCHAIN_ROOTDIR)/bin/armar.exe 
LD = $(TOOLCHAIN_ROOTDIR)/bin/armlink.exe 

AS_PROCESS = $(AS) $(AS_FLAGS) --cpreproc_opts='$(addprefix -D,$(AS_DEFINE))' $(addprefix -I,$(INCLUDE)) $(addprefix -I,$(RT_INCLUDE)) $(2) -o $(1) 
CC_PROCESS = $(CC) $(CC_FLAGS) $(addprefix -D,$(CC_DEFINE)) $(addprefix -I,$(INCLUDE)) $(addprefix -I,$(RT_INCLUDE)) -c $(2) -o $(1) 
CX_PROCESS = $(CX) $(CX_FLAGS) $(addprefix -D,$(CX_DEFINE)) $(addprefix -I,$(INCLUDE)) $(addprefix -I,$(RT_INCLUDE)) -c $(2) -o $(1) 
AR_PROCESS = $(AR) $(AR_FLAGS) -r $(1) --via=$(3) 
LD_PROCESS = $(LD) $(LD_FLAGS) --list=$(basename $(1)).map --via=$(3) \
--scatter=$(LINKER_FILE) \
$(addprefix --libpath=,$(RT_PATHS)) \
$(LIBRARIES) \
-o $(1) 

endif


#------------------------------------------------------------
# toolchain settings for unix
#
# XX_PROCESS macros:
# $(1) - output file path
# $(2) - list of object files
# $(3) - file contains list of object files
#------------------------------------------------------------
ifeq ($(HOSTENV),UNIX)
$(error "no ds5 settings for unix")
endif


#------------------------------------------------------------
# tool extensions 
#------------------------------------------------------------
LIBRARY_EXT     = a
APPLICATION_EXT = axf


#------------------------------------------------------------
# libraries tool options 
#------------------------------------------------------------
ifeq ($(TYPE),library)

# common settings
AS_FLAGS += --cpu=Cortex-A5.neon --fpu=vfpv4 -g --cpreproc --apcs=/interwork --diag_suppress=1608,1876 
CC_FLAGS += --cpu=Cortex-A5.neon --fpu=vfpv4 --vectorize --diag_suppress=1296,186 -g --c99 --thumb --apcs=/interwork 
CX_FLAGS += --cpu=Cortex-A5.neon --fpu=vfpv4 --vectorize --diag_suppress=1296,186 -g --cpp --thumb --apcs=/interwork 
LD_FLAGS += --cpu=Cortex-A5.neon --fpu=vfpv4 --keep linker_symbols.o(KERNEL_DATA_START) --keep linker_symbols.o(KERNEL_DATA_END) --keep linker_symbols.o(BOOT_STACK) --entry __boot --map --debug --symbols --info debug,sizes,totals,unused,veneers --diag_suppress=6314,6329 


# common 'debug' configuration settings
ifeq ($(CONFIG),debug)
CC_FLAGS += -O0 
CX_FLAGS += -O0 
AS_DEFINE += _DEBUG=1 
CC_DEFINE += _DEBUG=1 
CX_DEFINE += _DEBUG=1 
endif

# common 'release' configuration settings
ifeq ($(CONFIG),release)
CC_FLAGS += -O3 
CX_FLAGS += -O3 
endif

endif


#------------------------------------------------------------
# application tool options 
#------------------------------------------------------------
ifeq ($(TYPE),application)

# common settings
AS_FLAGS += --cpu=Cortex-A5.neon --fpu=vfpv4 -g --cpreproc --apcs=/interwork --diag_suppress=1608,1876 
CC_FLAGS += --cpu=Cortex-A5.neon --fpu=vfpv4 --vectorize --diag_suppress=1296,186 -g --c99 --thumb --apcs=/interwork 
CX_FLAGS += --cpu=Cortex-A5.neon --fpu=vfpv4 --vectorize --diag_suppress=1296,186 -g --cpp --thumb --apcs=/interwork 
LD_FLAGS += --cpu=Cortex-A5.neon --fpu=vfpv4 --keep linker_symbols.o(KERNEL_DATA_START) --keep linker_symbols.o(KERNEL_DATA_END) --keep linker_symbols.o(BOOT_STACK) --entry __boot --map --debug --symbols --info debug,sizes,totals,unused,veneers --diag_suppress=6314,6329 


# common 'debug' configuration settings
ifeq ($(CONFIG),debug)
CC_FLAGS += -O0 
CX_FLAGS += -O0 
AS_DEFINE += _DEBUG=1 
CC_DEFINE += _DEBUG=1 
CX_DEFINE += _DEBUG=1 
endif

# common 'release' configuration settings
ifeq ($(CONFIG),release)
CC_FLAGS += -O3 
CX_FLAGS += -O3 
endif

endif

