include ../utils/core.mk
include ../utils/general.mk

TARGET := victoria_runtime

INCLUDES += -Isrc -Isrc/thirdparty -I../glad/include -I../victoria.core/src
LDFLAGS += -lGLAD -lvictoria.core
DEFINES += -DVEXPORT -DGLAD_GLX_NO_X11


# Module options
export USE_FREETYPE := yes

# Module-specific options
ifeq ($(USE_FREETYPE), yes)
	DEFINES += -DFREETYPE_ENABLED -DDLL_IMPORT
	LDFLAGS += -lfreetype
	INCLUDES += -I../freetype/include
endif

ifeq ($(USE_MSVC), yes)
ifeq ($(DEBUG), yes)
	CCFLAGS += /MDd
else
	CCFLAGS += /MD
endif
	LDFLAGS += /DLL
endif 

ifeq ($(COMPILER), gcc)
	LDFLAGS += -shared -rdynamic
endif
ifeq ($(COMPILER), clang)
	LDFLAGS += --shared
endif

ifeq ($(PLATFORM), linux)
	CCFLAGS += -fPIC -fvisibility=hidden
endif
# Force MSVC variables to their proper form.
ifeq ($(USE_MSVC), yes)
	INCLUDES := $(patsubst -I%,/I%,$(INCLUDES))
	LDFLAGS := $(patsubst -l%,%.lib,$(LDFLAGS))
	DEFINES := $(patsubst -D%,/D%,$(DEFINES))
endif

# Merge CFLAGS, CPPFLAGS and CCFLAGS
CFLAGS += $(CCFLAGS)
CPPFLAGS += $(CCFLAGS)

ifeq ($(PLATFORM), win32)
	C_FILES := $(call rwildcard,*,*.c)
	CPP_FILES := $(call rwildcard,*,*.cpp)
	DIRECTORIES := $(patsubst src,,$(patsubst src\\%,%,$(subst $(shell cd)\,,$(shell dir src /s /AD /B)))))
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
OUTPUT_NAME := libvictoria.runtime
OUTPUT_FILE := $(OUTPUT_NAME)
BINARY_COMMAND :=
ifeq ($(PLATFORM), win32)
	OUTPUT_FILE := $(OUTPUT_FILE).dll
else
ifeq ($(PLATFORM), linux)
	OUTPUT_FILE := $(OUTPUT_FILE).so
endif
endif

ifeq ($(USE_MSVC), yes)
	BINARY_COMMAND := /OUT:$(BUILD_DIR)/$(OUTPUT_FILE)
else
	BINARY_COMMAND := -o $(BUILD_DIR)/$(OUTPUT_FILE)
endif

.PHONY: scaffold generate_compile_commands clean

all: scaffold $(OBJ_FILES) $(BUILD_DIR)/$(OUTPUT_FILE)
	@echo All tasks built successfully.

scaffold:
ifeq ($(PLATFORM), win32)
	-@mkdir $(subst /,\,$(BUILD_DIR)/$(TARGET)) 2>NUL || cd .
	-@mkdir $(subst /,\,$(addprefix $(BUILD_DIR)/$(TARGET)/,$(DIRECTORIES))) 2>NUL || cd .
endif
ifeq ($(PLATFORM), linux)
	@mkdir -p $(BUILD_DIR)/$(TARGET)
	@mkdir -p $(addprefix $(BUILD_DIR)/$(TARGET)/,$(DIRECTORIES))
endif
	@echo Done.
	@echo Compiling...

generate_compile_commands:
	@$(PYTHON) ../utils/gen_tmp_commands.py --extension .cpp --arg_path src --args $(CXX) $(CPPFLAGS) -c $(DEFINES) $(INCLUDES)
	@$(PYTHON) ../utils/gen_tmp_commands.py --extension .c --arg_path src --args $(CC) $(CFLAGS) -c $(DEFINES) $(INCLUDES)
	@$(PYTHON) ../utils/gen_tmp_commands.py --sources $(CPP_FILES) $(C_FILES) --objects $(OBJ_FILES)

clean:
ifeq ($(PLATFORM), win32)
	@rmdir $(subst /,\,$(BUILD_DIR)/$(TARGET)) /S /Q 2>NUL || cd .
	@del /F /Q $(subst /,\,$(BUILD_DIR)/$(OUTPUT_NAME).*) /Q 2>NUL || cd .
endif
ifeq ($(PLATFORM), linux)
	@rm -rf $(BUILD_DIR)/$(TARGET)
	@rm -f $(BUILD_DIR)/$(OUTPUT_FILE)
endif

$(BUILD_DIR)/$(OUTPUT_FILE): $(OBJ_FILES)
	@echo Done.
	@echo Building binary...
	@$(LD) $(OBJ_FILES) $(BINARY_COMMAND) $(LDFLAGS)
	@echo $(OUTPUT_FILE) built successfully.

$(BUILD_DIR)/$(TARGET)/%.o: src/%.c
	@echo   $<...
ifeq ($(COMPILER), msvc)
	@$(CC) $< $(CFLAGS) /c /Fo:$@ /Fd$(subst .o,.pdb,$@) /sourceDependencies $(dir $@) $(DEFINES) $(INCLUDES)
	@$(PYTHON) ../utils/windows/gen_dependency_files.py $< -o $@
else
	@$(CC) $< $(CFLAGS) -c -o $@ $(DEFINES) $(INCLUDES)
endif

$(BUILD_DIR)/$(TARGET)/%.o: src/%.cpp
	@echo   $<...
ifeq ($(COMPILER), msvc)
	@$(CXX) $< $(CPPFLAGS) /c /Fo:$@ /Fd$(subst .o,.pdb,$@) /sourceDependencies $(dir $@) $(DEFINES) $(INCLUDES)
	@$(PYTHON) ../utils/windows/gen_dependency_files.py $< -o $@
else
	@$(CXX) $< $(CPPFLAGS) -c -o $@ $(DEFINES) $(INCLUDES)
endif

-include $(OBJ_FILES:.o=.d)
