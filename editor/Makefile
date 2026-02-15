# Marginally more generic version of the Makefile we'll let end-users use to compile their own projects.
TARGET := editor
INCLUDES += -Iinclude -I../engine/include
LINKER_FLAGS += -L$(BUILD_DIR) -lvictoria

ifeq ($(DETECTED_OS), LINUX)
	LINKER_FLAGS += -Wl,-rpath='$$ORIGIN'
endif

ifeq ($(COMPILER), clang)
	CPPFLAGS += -fdeclspec
endif

# Source files and directories
C_FILES :=
CPP_FILES :=
DIRECTORIES :=

ifeq ($(DETECTED_OS), WINDOWS)
	C_FILES += $(call rwildcard,*,*.c)
	CPP_FILES += $(call rwildcard,*,*.cpp)
	DIRECTORIES += $(addprefix $(BUILD_DIR)\, $(subst $(BUILD_DIR),,$(subst $(shell cd)\,,$(shell dir /s /AD /B))))
else
ifeq ($(DETECTED_OS), LINUX)
	CPP_FILES := $(shell find src -type f -name "*.cpp")
	C_FILES := $(shell find src -type f -name "*.c")
	DIRECTORIES := $(patsubst src/%,%,$(shell find src -type d -not -path "src"))
endif
endif


# Collect object files
OBJ_FILES := $(patsubst src/%.cpp, $(BUILD_DIR)/$(TARGET)/%.o, $(CPP_FILES)) $(patsubst src/%.c, $(BUILD_DIR)/$(TARGET)/%.o, $(C_FILES))


# Setup final binary
OUTPUT_FILE := victoria
ifeq ($(DETECTED_OS), WINDOWS)
	OUTPUT_FILE := $(OUTPUT_FILE).exe
else
ifeq ($(DETECTED_OS), LINUX)
	OUTPUT_FILE := $(OUTPUT_FILE).x86_64
endif
endif


.PHONY: scaffold compile link clean
all: scaffold compile link


scaffold:
	@echo Creating project structure...
ifeq ($(DETECTED_OS), WINDOWS)
	-@mkdir $(BUILD_DIR)/$(TARGET) 2>NUL || cd .
	-@mkdir $(addprefix $(BUILD_DIR)/$(TARGET)/,$(DIRECTORIES)) 2>NUL || cd .
endif
ifeq ($(DETECTED_OS), LINUX)
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
	@$(CXX) $(OBJ_FILES) -o $(BUILD_DIR)/$(OUTPUT_FILE) $(LINKER_FLAGS)
	@echo $(OUTPUT_FILE) built successfully.


clean:
ifeq ($(DETECTED_OS), WINDOWS)
	@rmdir $(BUILD_DIR)/$(TARGET) /S /Q
endif
ifeq ($(DETECTED_OS), LINUX)
	@rm -rf $(BUILD_DIR)/$(TARGET)
endif


$(BUILD_DIR)/$(TARGET)/%.o: src/%.cpp
	@echo   $<...
	@$(CXX) $< $(CPPFLAGS) -c -o $@ $(DEFINES) $(INCLUDES)

-include $(OBJ_FILES:.o=.d)
