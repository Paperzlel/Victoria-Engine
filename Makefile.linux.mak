#Command-line arguments
IS_DEBUG := no

BUILD_DIR := bin

# Set compiler flags
COMPILER_FLAGS := -g -MD -fPIC

# Set defines
DEFINES :=
ifeq ($(IS_DEBUG), yes)
	DEFINES := $(DEFINES) -DDEBUG
endif

# Set linker flags
LINKER_FLAGS := -lGL -lX11 -L/usr/X11R6/lib -Wl,-rpath,.

# Set includes
INCLUDES := -I$(PWD)

CPP_FILES := $(shell find -type f -name "*.cpp") # Find all the .cpp source files
C_FILES := $(shell find -type f -name "*.c") # Find all the .c source files
OBJ_FILES := $(CPP_FILES:%=$(BUILD_DIR)/%.o) $(C_FILES:%=$(BUILD_DIR)/%.o) # Find all the .o files

DIRECTORIES := $(subst $(BUILD_DIR),,$(shell find -type d)) # Find all the directories in the project

all: compile link

.PHONY: compile
compile:
	@echo Creating project structure...
	@mkdir -p bin
	@mkdir -p $(addprefix $(BUILD_DIR)/,$(DIRECTORIES))
	@echo Done.
	@echo Compiling...

.PHONY: link
link: compile $(OBJ_FILES)
	@echo Done.
	@echo Building binary...
	@clang++ $(OBJ_FILES) -o bin/victoria.x86_64 $(LINKER_FLAGS)

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)

$(BUILD_DIR)/%.c.o: %.c
	@echo   $<...
	@clang++ -x c++ $(C_FILES) -c -o $@ $(DEFINES) $(INCLUDES)

$(BUILD_DIR)/%.cpp.o: %.cpp
	@echo   $<...
	@clang++ $< $(COMPILER_FLAGS) -c -o $@ $(DEFINES) $(INCLUDES)

-include $(OBJ_FILES:.o=.d)