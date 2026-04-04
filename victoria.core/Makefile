include ../utils/core.mk
include ../utils/general.mk

TARGET := victoria_core
# We keep third-party, platform and driver specific code hidden from the user.
INCLUDES += -Isrc -I../glad/include -Isrc/thirdparty
LDFLAGS += -lGLAD
DEFINES += -DVEXPORT

# Define flags that are applied only to third-party libraries. Essentially a boiled-down version of before.
ifeq ($(USE_MSVC), yes)
ifeq ($(DEBUG), yes)
	CCFLAGS += /MDd
	LDFLAGS += /NATVIS:src/platform/windows/libvictoria.natvis
else
	CCFLAGS += /MD
endif
endif

# Utilities

# Make does not offer a recursive wildcard function, so here's one:
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# Platform-specific options
export USE_WAYLAND := no
export USE_X11 := no

# Wayland generator folder
WAYLAND_PROTOCOLS := src/platform/linux/wayland/protocols

ifeq ($(PLATFORM), linux)
	CCFLAGS += -fPIC -fvisibility=hidden
	THIRDPARTY_CFLAGS += -fPIC -fvisibility=hidden
ifeq ($(shell pkg-config --exists wayland-client; echo $$?),0)
# Require Wayland EGL support
ifeq ($(shell pkg-config --exists wayland-egl; echo $$?),0)
	USE_WAYLAND := yes
endif
else
	USE_WAYLAND := no
endif
ifeq ($(shell pkg-config --exists x11; echo $$?),1)
	USE_X11 := no
else
	USE_X11 := yes
endif
endif

# Compiler-specific options
ifeq ($(COMPILER), gcc)
	LDFLAGS += -shared -rdynamic
endif
ifeq ($(COMPILER), clang)
	LDFLAGS += --shared
endif

ifeq ($(USE_MSVC), yes)
	LDFLAGS += /DLL
endif

# OS-specific options
ifeq ($(PLATFORM), linux)
ifeq ($(USE_WAYLAND), yes)
	DEFINES += -DWAYLAND_ENABLED
	LDFLAGS += $(shell pkg-config --libs wayland-egl)
	CFLAGS += -Wno-unused-const-variable
	TMP := $(shell $(PYTHON) src/platform/linux/wayland/generate_protocols.py)
endif
ifeq ($(USE_X11), yes)
	DEFINES += -DX11_ENABLED
	LDFLAGS += $(shell pkg-config --libs x11)
endif
endif

ifeq ($(PLATFORM), win32)
# We only want ANGLE libs on Windows
	INCLUDES += -Isrc/thirdparty/angle/include
	LDFLAGS += -luser32 -lopengl32 -lgdi32
# Force MSVC variables to their proper form.
ifeq ($(USE_MSVC), yes)
	INCLUDES := $(patsubst -I%,/I%,$(INCLUDES))
	LDFLAGS := $(patsubst -l%,%.lib,$(LDFLAGS))
	DEFINES := $(patsubst -D%,/D%,$(DEFINES))
endif
endif

# Merge CFLAGS, CPPFLAGS and CCFLAGS
CFLAGS += $(CCFLAGS)
CPPFLAGS += $(CCFLAGS)

# Source files and directories
C_FILES :=
CPP_FILES :=
DIRECTORIES :=

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
OUTPUT_NAME := libvictoria.core
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

.PHONY: test scaffold generate_compile_commands clean

all: test scaffold $(OBJ_FILES) $(BUILD_DIR)/$(OUTPUT_FILE)
	@echo All tasks built successfully.

# Look for if certain files are downloaded prior to building
test:
ifeq ($(PLATFORM), linux)
ifeq ("$(and $(USE_X11), $(USE_WAYLAND))","")
	@echo Cannot compile as neither X11 nor Wayland is available on the system.
	@exit 10
endif
endif


scaffold: test
	@echo Creating project structure...
ifeq ($(PLATFORM), win32)
	-@mkdir $(subst /,\,$(BUILD_DIR)), 2>NUL || cd .
	-@mkdir $(subst /,\,$(BUILD_DIR)/$(TARGET)) 2>NUL || cd .
	-@mkdir $(subst /,\,$(addprefix $(BUILD_DIR)/$(TARGET)/,$(DIRECTORIES))) 2>NUL || cd .
	-@xcopy "assets\" "$(subst /,\,$(BUILD_DIR)/assets)\" /q /y 2>NUL || cd .
endif
ifeq ($(PLATFORM), linux)
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/$(TARGET)
	@mkdir -p $(addprefix $(BUILD_DIR)/$(TARGET)/,$(DIRECTORIES))
	@cp -r assets $(BUILD_DIR)
endif
	@echo Done.
	@echo Compiling...


generate_compile_commands:
	@echo Getting compile commands from $(TARGET)...
	@$(PYTHON) ../utils/gen_tmp_commands.py --extension .cpp --arg_path src --args $(CXX) $(CPPFLAGS) -c $(DEFINES) $(INCLUDES)
	@$(PYTHON) ../utils/gen_tmp_commands.py --extension .c --arg_path src --args $(CC) $(CFLAGS) -c $(DEFINES) $(INCLUDES)
	@$(PYTHON) ../utils/gen_tmp_commands.py --sources $(CPP_FILES) $(C_FILES) --objects $(OBJ_FILES)

clean:
ifeq ($(PLATFORM), win32)
	@rmdir $(subst /,\,$(BUILD_DIR)/$(TARGET)) /S /Q 2>NUL || cd .
	@del /F /Q $(subst /,\,$(BUILD_DIR)/$(OUTPUT_NAME).*) /Q 2>NUL || cd .
	@del /F /Q $(subst /,\,$(BUILD_DIR)/assets) 2>NUL || cd .
	@rmdir $(subst /,\,$(BUILD_DIR)/assets) /S /Q 2>NUL || cd .
endif
ifeq ($(PLATFORM), linux)
	@rm -rf $(BUILD_DIR)/$(TARGET)
	@rm -f $(BUILD_DIR)/$(OUTPUT_FILE)
	@rm -rf $(WAYLAND_PROTOCOLS)
	@rm -rf $(BUILD_DIR)/assets
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
