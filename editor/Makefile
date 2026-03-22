# Marginally more generic version of the Makefile we'll let end-users use to compile their own projects.
TARGET := editor
INCLUDES += -Iinclude -I../engine/include
LDFLAGS += -L$(BUILD_DIR)
DIR := 

ifeq ($(PLATFORM), linux)
	LDFLAGS += -lvictoria -Wl,-rpath='$$ORIGIN'
	DIR := $(PWD)
endif

ifeq ($(PLATFORM), win32)
	LDFLAGS += -llibvictoria
	DIR := $(subst \,/,$(shell cd))
endif

# Make does not offer a recursive wildcard function, so here's one:
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# Merge CFLAGS, CPPFLAGS and CCFLAGS
CFLAGS += $(CCFLAGS)
CPPFLAGS += $(CCFLAGS)

# Source files and directories
C_FILES :=
CPP_FILES :=
DIRECTORIES :=

ifeq ($(PLATFORM), win32)
	C_FILES += $(call rwildcard,*,*.c)
	CPP_FILES += $(call rwildcard,*,*.cpp)
	DIRECTORIES += $(addprefix $(BUILD_DIR)\, $(subst $(BUILD_DIR),,$(subst $(shell cd)\,,$(shell dir /s /AD /B))))
else
ifeq ($(PLATFORM), linux)
	CPP_FILES := $(shell find src -type f -name "*.cpp")
	C_FILES := $(shell find src -type f -name "*.c")
	DIRECTORIES := $(patsubst src/%,%,$(shell find src -type d -not -path "src"))
endif
endif


# Collect object files
OBJ_FILES := $(patsubst src/%.cpp, $(BUILD_DIR)/$(TARGET)/%.o, $(CPP_FILES)) $(patsubst src/%.c, $(BUILD_DIR)/$(TARGET)/%.o, $(C_FILES))


# Setup final binary
OUTPUT_NAME := victoria
OUTPUT_FILE := $(OUTPUT_NAME)
ifeq ($(PLATFORM), win32)
	OUTPUT_FILE := $(OUTPUT_FILE).exe
else
ifeq ($(PLATFORM), linux)
	OUTPUT_FILE := $(OUTPUT_FILE).x86_64
endif
endif


.PHONY: scaffold compile link generate_compile_commands clean
all: scaffold compile link


scaffold:
	@echo Creating project structure...
ifeq ($(PLATFORM), win32)
	-@mkdir $(subst /,\,$(BUILD_DIR)/$(TARGET)) 2>NUL || cd .
	-@mkdir $(subst /,\,$(addprefix $(BUILD_DIR)/$(TARGET)/,$(DIRECTORIES))) 2>NUL || cd .
endif
ifeq ($(PLATFORM), linux)
	@mkdir -p $(BUILD_DIR)/$(TARGET)
# Check that there's at least one subdirectory in the source tree.
ifneq ("$(DIRECTORIES)","")
	@mkdir -p $(addprefix $(BUILD_DIR)/$(TARGET)/,$(DIRECTORIES))
endif
endif
	@echo Done.


compile: scaffold
	@echo Compiling...


link: compile $(OBJ_FILES)
	@echo Done.
	@echo Building binary...
	@$(CXX) $(OBJ_FILES) -o $(BUILD_DIR)/$(OUTPUT_FILE) $(LDFLAGS)
	@echo $(OUTPUT_FILE) built successfully.

generate_compile_commands:
	@echo Getting compile commands from $(TARGET)...
	@$(PYTHON) ../misc/gen_tmp_commands.py --extension .cpp --arg_path src --args $(CXX) $(CPPFLAGS) -c $(DEFINES) $(INCLUDES)
	@$(PYTHON) ../misc/gen_tmp_commands.py --sources $(CPP_FILES) $(C_FILES) --objects $(OBJ_FILES)

clean:
ifeq ($(PLATFORM), win32)
	@rmdir $(subst /,\,$(BUILD_DIR)/$(TARGET)) /S /Q 2>NUL || cd .
	@del /F /Q $(subst /,\,$(BUILD_DIR)/$(OUTPUT_NAME).*) /Q 2>NUL || cd .
endif
ifeq ($(PLATFORM), linux)
	@rm -rf $(BUILD_DIR)/$(TARGET)
	@rm -f $(BUILD_DIR)/$(OUTPUT_FILE)
endif


$(BUILD_DIR)/$(TARGET)/%.o: src/%.cpp
	@echo   $<...
	@$(CXX) $< $(CPPFLAGS) -c -o $@ $(DEFINES) $(INCLUDES)

-include $(OBJ_FILES:.o=.d)
