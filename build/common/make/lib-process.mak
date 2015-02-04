#----------------------------------------------------------
# put object/dependency files to these directories
#----------------------------------------------------------
override OBJECTS_DIR = $(LIBRARY_DIR)/obj
override DEPENDS_DIR = $(LIBRARY_DIR)/dep


#----------------------------------------------------------
# create list of objects/dependency files by transforming 
# path/[file].c, path/[file].s to [file].o 
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
# supported targets: 'help', 'build', 'clean', 'debugme'
#----------------------------------------------------------
.PHONY: build clean rebuild debugme help


#----------------------------------------------------------
# target : print help information
#----------------------------------------------------------
help:
	$(call PRINT, "usage    : make TOOL=<tool> CONFIG=<debug|release> build")
	$(call PRINT, "         : make TOOL=<tool> CONFIG=<debug|release> clean")
	$(call PRINT, "example  : make TOOL=gcc_cs CONFIG=debug build")
	$(call PRINT, "")
	$(call PRINT, "TOOL     : name of tool, related to makefile placed in 'build/<board>/make/tools/' ")
	$(call PRINT, "CONFIG   : name of build configuration, defined in 'build/<board>/make/tools/<tool>.mak' ")


#----------------------------------------------------------
# target: create directories
#----------------------------------------------------------
$(OBJECTS_DIR) $(DEPENDS_DIR) $(LIBRARY_DIR):
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
# include dependencies if any
#----------------------------------------------------------
-include $(DEPENDS)


#----------------------------------------------------------
# target: build $(LIBRARY_FILE)
#----------------------------------------------------------
$(LIBRARY_FILE): $(LIBRARY_DIR) $(OBJECTS_DIR) $(DEPENDS_DIR) $(OBJECTS)
	$(call LSDIR,$(OBJECTS_DIR)) > $(LIBRARY_DIR)/objects.lst
	$(call AR_PROCESS,$@,$(OBJECTS),$(LIBRARY_DIR)/objects.lst)
	$(call POSTBUILD_CMD)
	$(call PRINT)
	$(call PRINT)
	$(call PRINT, 'Build done')
	$(call PRINT)

build: $(LIBRARY_FILE)


#----------------------------------------------------------
# target: clean
# need $(LIBRARY_DIR) dependency to suppress error
# if $(LIBRARY_DIR) doesn't exist
#----------------------------------------------------------
clean: $(LIBRARY_DIR)
	$(call RMDIR,$(LIBRARY_DIR))
	$(call PRINT)
	$(call PRINT, 'Clean done')
	$(call PRINT)


#----------------------------------------------------------
# target: rebuild
# perform clean and build
#----------------------------------------------------------
rebuild: clean build



# ----------------------------------------------------------
# target: debugme
# ----------------------------------------------------------
debugme:
	$(call PRINT, 'include : $(INCLUDE) ')
	$(call PRINT)
	$(call PRINT, 'sources : $(SOURCES) ')
	$(call PRINT)


