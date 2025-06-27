# Command-line arguments
IS_DEBUG := no

BUILD_DIR := bin

# Compiler flags
COMPILER_FLAGS := -g -MD -Wall -Werror
# Defines
DEFINES := -D_CRT_SECURE_NO_WARNINGS
ifeq ($(IS_DEBUG), yes)
	DEFINES := $(DEFINES) -DDEBUG -D_DEBUG
endif

# Linker flags
LINKER_FLAGS := -luser32 -lopengl32 -lgdi32
ifeq ($(IS_DEBUG), yes)
	LINKER_FLAGS := $(LINKER_FLAGS) -g -lmsvcrtd -lvcruntimed -lucrtd
endif

# Include flags
INCLUDES := -I$(shell cd)

# Make does not offer a recursive wildcard function, so here's one:
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

CPP_FILES := $(call rwildcard,*,*.cpp)
C_FILES := $(call rwildcard,*,*.c)
OBJ_FILES := $(CPP_FILES:%=$(BUILD_DIR)/%.o) $(C_FILES:%=$(BUILD_DIR)/%.o)

DIRECTORIES := $(subst $(BUILD_DIR),,$(subst $(shell cd)\,,$(shell dir /s /AD /B)))

FINAL_DIRS := $(addprefix $(BUILD_DIR)\, $(DIRECTORIES))

all: compile link 

.PHONY: compile
compile:
	@echo Creating project bin structure...
ifeq ("$(wildcard $(BUILD_DIR))", "")
	@mkdir bin
endif
ifeq ("$(wildcard $(FINAL_DIRS))", "")
	@mkdir $(FINAL_DIRS)
endif
	@echo Done.
	@echo Compiling...

.PHONY: link
link: compile $(OBJ_FILES)
	@echo Done.
	@echo Building binary...
	@clang++ $(OBJ_FILES) -o bin/victoria.exe $(LINKER_FLAGS)

.PHONY: clean
clean:
	@rmdir $(BUILD_DIR) /S /Q

$(BUILD_DIR)/%.c.o: %.c
	@echo   $<...
	@clang++ -x c++ $< $(COMPILER_FLAGS) -c -o $@ $(DEFINES) $(INCLUDES)

$(BUILD_DIR)/%.cpp.o: %.cpp
	@echo   $<...
	@clang++ $< $(COMPILER_FLAGS) -c -o $@ $(DEFINES) $(INCLUDES)

-include $(OBJ_FILES:.o=.d)