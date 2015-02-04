#----------------------------------------------------------
# put object/dependency files to these directories
#----------------------------------------------------------
override OBJECTS_DIR = $(APPLICATION_DIR)/obj
override DEPENDS_DIR = $(APPLICATION_DIR)/dep
override LOADNAME    = $(basename $(notdir $(LINKER_FILE)))


#----------------------------------------------------------
# create list of objects/dependency files by transforming 
# path/[file].c to [file].o
#----------------------------------------------------------
override OBJECTS := $(addprefix $(OBJECTS_DIR)/,$(notdir $(SOURCES)))
override OBJECTS := $(OBJECTS:.c=.o)
override OBJECTS := $(OBJECTS:.S=.o)
override OBJECTS := $(OBJECTS:.cpp=.o)
override DEPENDS := $(sort $(addprefix $(DEPENDS_DIR)/,$(notdir $(SOURCES))))
override DEPENDS := $(DEPENDS:.c=.d)
override DEPENDS := $(DEPENDS:.S=.d)
override DEPENDS := $(DEPENDS:.cpp=.d)


#----------------------------------------------------------
# set list of directories for each vpath pattern
# e.g where specific files are placed
#----------------------------------------------------------
vpath %.c $(dir $(SOURCES))
vpath %.S $(dir $(SOURCES))
vpath %.cpp $(dir $(SOURCES))
vpath %.o $(OBJECTS_DIR)


#----------------------------------------------------------
# make 'build', 'clean', 'debugme'
#----------------------------------------------------------
.PHONY: build clean rebuild debugme help


#----------------------------------------------------------
# target : print help information
#----------------------------------------------------------
help:
	$(call PRINT, "usage        : make BOARD=<board> TOOL=<tool> CONFIG=<debug|release> LOAD=<intflash> build")
	$(call PRINT, "             : make BOARD=<board> TOOL=<tool> CONFIG=<debug|release> LOAD=<intflash> clean")
	$(call PRINT, "example      : make BOARD=twrk60n512 TOOL=gcc_cw CONFIG=debug LOAD=intflash build")
	$(call PRINT, "")
	$(call PRINT, 'BOARD        : name of board, related to the directory placed in 'build/')
	$(call PRINT, "TOOL         : name of tool, related to makefile placed in 'build/<board>/make/tools/' ")
	$(call PRINT, "CONFIG       : name of build configuration, defined in 'build/<board>/make/tools/<tool>.mak' ")
	$(call PRINT, "LOAD         : basename of mqx linker command file placed in lib/<board>.<tool>/<config>/bsp/ ' ")
	$(call PRINT, "LINKER_FILE  : path to custom linker command file ")


#----------------------------------------------------------
# target: create directories
#----------------------------------------------------------
$(OBJECTS_DIR) $(DEPENDS_DIR) $(APPLICATION_DIR):
	$(call PRINT)
	$(call PRINT, 'creating directory : $@')
	$(call MKDIR,$@)


#----------------------------------------------------------
# target: compile c files
#----------------------------------------------------------
$(OBJECTS_DIR)/%.o: %.c
	$(call PRINT)
	$(call PRINT, 'Compile c file: $< to $@')
	$(call CC_PROCESS,$@,$<)


#----------------------------------------------------------
# target: compile cpp files
#----------------------------------------------------------
$(OBJECTS_DIR)/%.o: %.cpp
	$(call PRINT)
	$(call PRINT, 'Compile cpp file: $< to $@')
	$(call CX_PROCESS,$@,$<)


#----------------------------------------------------------
# target: compile s files
#----------------------------------------------------------
$(OBJECTS_DIR)/%.o: %.S 
	$(call PRINT)
	$(call PRINT, 'Compile s file: $< to $@')
	$(call AS_PROCESS,$@,$<)


#----------------------------------------------------------
# include dependencies
#----------------------------------------------------------
-include $(DEPENDS)


#----------------------------------------------------------
# target: build $(APPLICATION_FILE)
#----------------------------------------------------------
$(APPLICATION_FILE): $(OBJECTS_DIR) $(DEPENDS_DIR) $(OBJECTS) $(LIBRARIES)
	$(call LSDIR,$(OBJECTS_DIR)) > $(APPLICATION_DIR)/objects.lst
	$(call LD_PROCESS,$@,$(OBJECTS),$(APPLICATION_DIR)/objects.lst)
	$(call POST_BUILD_CMD)
	$(call PRINT)
	$(call PRINT)
	$(call PRINT, 'Build done')
	$(call PRINT)

build: $(APPLICATION_FILE)


#----------------------------------------------------------
# target: clean
# need $(APPLICATION_DIR) dependency to suppress error
# if $(APPLICATION_DIR) doesn't exist
#----------------------------------------------------------
clean: $(APPLICATION_DIR)
	$(call RMDIR,$(APPLICATION_DIR))
	$(call PRINT)
	$(call PRINT, 'Clean done')
	$(call PRINT)


#----------------------------------------------------------
# target: rebuild
# perform clean and build
#----------------------------------------------------------
rebuild: clean build



#----------------------------------------------------------
# target: debugme
#----------------------------------------------------------
debugme:
	$(call PRINT, 'include : $(INCLUDE) ')
	$(call PRINT)
	$(call PRINT, 'sources : $(SOURCES) ')
	$(call PRINT)


