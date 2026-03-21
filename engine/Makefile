TARGET := engine
# We want to hide as much of the API as possible, so we set the visibility to hidden by default. This allows our own API to be exposed properly.
CCFLAGS += -fPIC -fvisibility=hidden
# We keep third-party, platform and driver specific code hidden from the user.
INCLUDES += -Iinclude -Isrc/thirdparty/freetype/include -Isrc/thirdparty/glad/include -Isrc/thirdparty
DEFINES += -DVEXPORT

# Define flags that are applied only to third-party libraries. Essentially a boiled-down version of before.
THIRDPARTY_CFLAGS := -fPIC -fvisibility=hidden
ifeq ($(DEBUG), yes)
	THIRDPARTY_CFLAGS += -O0
ifeq ($(PLATFORM), win32)
ifeq ($(COMPILER), clang)
	THIRDPARTY_CFLAGS += -gdwarf-4
else
	THIRDPARTY_CFLAGS += -gdwarf-5
endif
else
	THIRDPARTY_CFLAGS += -gdwarf-4 -g3
endif
else
	THIRDPARTY_CFLAGS += -O2
endif

# Utilities

# Make does not offer a recursive wildcard function, so here's one:
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# Module options
export USE_FREETYPE := yes

# Platform-specific options
export USE_WAYLAND := no
export USE_X11 := no

# Wayland generator folder
WAYLAND_PROTOCOLS := src/platform/linux/wayland/protocols

ifeq ($(PLATFORM), linux)
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

# Module-specific options
ifeq ($(USE_FREETYPE), yes)
	DEFINES += -DFREETYPE_ENABLED -DFT2_BUILD_LIBRARY
endif


# OS-specific options
ifeq ($(PLATFORM), linux)
ifeq ($(USE_WAYLAND), yes)
	DEFINES += -DWAYLAND_ENABLED
	LDFLAGS += $(shell pkg-config --libs wayland-egl)
	CFLAGS += $(shell pkg-config --cflags-only-other wayland-egl) -Wno-unused-const-variable
	INCLUDES += $(shell pkg-config --cflags-only-I wayland-egl)
endif
ifeq ($(USE_X11), yes)
	DEFINES += -DX11_ENABLED
	LDFLAGS += $(shell pkg-config --libs x11)
endif
endif

# Merge CFLAGS, CPPFLAGS and CCFLAGS
CFLAGS += $(CCFLAGS)
CPPFLAGS += $(CCFLAGS)

# Source files and directories
DIR := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
C_FILES :=
CPP_FILES :=
DIRECTORIES :=

# Pre-build generation steps (required for collecting build files)
ifeq ($(PLATFORM), linux)
ifeq ("$(wildcard $(WAYLAND_PROTOCOLS))","")
	TEMP := $(shell mkdir -p $(WAYLAND_PROTOCOLS); $(PYTHON) src/platform/linux/wayland/generate_dynwrappers.py)
endif
endif

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

# Setup GLAD file whitelist
C_FILES := $(patsubst src/thirdparty/glad/%.c,,$(C_FILES))
C_FILES += src/thirdparty/glad/src/gl.c \
	src/thirdparty/glad/src/egl.c
# GLX will fail to compile on Windows, preventative measure.
ifeq ($(USE_X11),yes)
	C_FILES += src/thirdparty/glad/src/glx.c
endif 

# Setup freetype file whitelist
C_FILES := $(patsubst src/thirdparty/freetype/%.c,,$(C_FILES))
FT_SRC := src/thirdparty/freetype/src
ifeq ($(USE_FREETYPE), yes)
	C_FILES += $(FT_SRC)/base/ftsystem.c 		\
				$(FT_SRC)/base/ftinit.c 		\
				$(FT_SRC)/base/ftdebug.c		\
				$(FT_SRC)/base/ftbase.c			\
				$(FT_SRC)/base/ftbbox.c			\
				$(FT_SRC)/base/ftglyph.c		\
				$(FT_SRC)/base/ftbdf.c			\
				$(FT_SRC)/base/ftbitmap.c		\
				$(FT_SRC)/base/ftcid.c			\
				$(FT_SRC)/base/ftfstype.c		\
				$(FT_SRC)/base/ftgasp.c			\
				$(FT_SRC)/base/ftgxval.c		\
				$(FT_SRC)/base/ftmm.c			\
				$(FT_SRC)/base/ftotval.c		\
				$(FT_SRC)/base/ftpatent.c		\
				$(FT_SRC)/base/ftpfr.c			\
				$(FT_SRC)/base/ftstroke.c		\
				$(FT_SRC)/base/ftsynth.c		\
				$(FT_SRC)/base/fttype1.c		\
				$(FT_SRC)/base/ftwinfnt.c		\
												\
				$(FT_SRC)/bdf/bdf.c				\
				$(FT_SRC)/cff/cff.c				\
				$(FT_SRC)/cid/type1cid.c		\
				$(FT_SRC)/pcf/pcf.c				\
				$(FT_SRC)/pfr/pfr.c				\
				$(FT_SRC)/sfnt/sfnt.c			\
				$(FT_SRC)/truetype/truetype.c	\
				$(FT_SRC)/type1/type1.c			\
				$(FT_SRC)/type42/type42.c		\
				$(FT_SRC)/winfonts/winfnt.c		\
												\
				$(FT_SRC)/smooth/smooth.c		\
				$(FT_SRC)/raster/raster.c		\
				$(FT_SRC)/sdf/sdf.c				\
												\
				$(FT_SRC)/autofit/autofit.c		\
				$(FT_SRC)/cache/ftcache.c		\
				$(FT_SRC)/gzip/ftgzip.c			\
				$(FT_SRC)/lzw/ftlzw.c			\
				$(FT_SRC)/bzip2/ftbzip2.c		\
				$(FT_SRC)/gxvalid/gxvalid.c		\
				$(FT_SRC)/otvalid/otvalid.c		\
				$(FT_SRC)/psaux/psaux.c			\
				$(FT_SRC)/pshinter/pshinter.c	\
				$(FT_SRC)/psnames/psnames.c		\
				$(FT_SRC)/svg/ftsvg.c
endif

# Collect object files
OBJ_FILES := $(patsubst src/%.cpp, $(BUILD_DIR)/$(TARGET)/%.o, $(CPP_FILES)) $(patsubst src/%.c, $(BUILD_DIR)/$(TARGET)/%.o, $(C_FILES))

# Setup final binary
OUTPUT_NAME := libvictoria
OUTPUT_FILE := $(OUTPUT_NAME)
ifeq ($(PLATFORM), win32)
	OUTPUT_FILE := $(OUTPUT_FILE).dll
else
ifeq ($(PLATFORM), linux)
	OUTPUT_FILE := $(OUTPUT_FILE).so
endif
endif

.PHONY: test scaffold compile link generate_compile_commands clean
all: test scaffold compile link

# Look for if certain files are downloaded prior to building
test:
ifeq ($(PLATFORM), linux)
ifeq ($(shell pkg-config --exists wayland-client; echo $$?),1)
ifeq ($(USE_WAYLAND), yes)
	@echo Cannot compile for Wayland whilst the drivers are not installed. Please install the development packages before compiling.
	@exit 10
endif
endif
ifeq ($(wildcard pkg-config --exists x11; echo $$?),1)
ifeq ($(USE_X11), yes)
	@echo Cannot compile with X11 support whilst the drivers are not installed. Please install the development packages before compiling.
	@exit 10
endif
endif
ifeq ("$(and $(USE_X11), $(USE_WAYLAND))","")
	@echo Cannot compile as neither X11 nor Wayland is available on the system.
	@exit 10
endif
endif


scaffold: test
	@echo Creating project structure...
ifeq ($(PLATFORM), win32)
	-@mkdir $(BUILD_DIR)/$(TARGET) 2>NUL || cd .
	-@mkdir $(addprefix $(BUILD_DIR)/$(TARGET)/,$(DIRECTORIES)) 2>NUL || cd .
endif
ifeq ($(PLATFORM), linux)
	@mkdir -p $(BUILD_DIR)/$(TARGET)
	@mkdir -p $(addprefix $(BUILD_DIR)/$(TARGET)/,$(DIRECTORIES))
endif
	@echo Done.


compile: scaffold $(WAYLAND_PROTOCOLS)
	@echo Compiling...


link: compile $(OBJ_FILES)
	@echo Done.
	@echo Building binary...
	@$(CXX) $(OBJ_FILES) -o $(BUILD_DIR)/$(OUTPUT_FILE) $(LDFLAGS)
	@echo $(OUTPUT_FILE) built successfully.

generate_compile_commands:
	@echo Getting compile commands from $(TARGET)...
	@$(PYTHON) ../misc/gen_tmp_commands.py --extension .cpp --arg_path src --args $(CXX) $(CPPFLAGS) -c $(DEFINES) $(INCLUDES)
	@$(PYTHON) ../misc/gen_tmp_commands.py --extension .c --arg_path src --args $(CXX) $(CFLAGS) -c $(DEFINES) $(INCLUDES)
	@$(PYTHON) ../misc/gen_tmp_commands.py --extension .c --arg_path src/thirdparty --args $(CXX) $(THIRDPARTY_CFLAGS) -c $(DEFINES) $(INCLUDES)
	@$(PYTHON) ../misc/gen_tmp_commands.py --sources $(CPP_FILES) $(C_FILES) --objects $(OBJ_FILES)

clean:
ifeq ($(PLATFORM), win32)
	@rmdir $(BUILD_DIR)/$(TARGET) /S /Q
	@del /F /Q $(subst /,\,$(BUILD_DIR)/$(OUTPUT_NAME).*) 2>NUL || cd .
endif
ifeq ($(PLATFORM), linux)
	@rm -rf $(BUILD_DIR)/$(TARGET)
	@rm -f $(BUILD_DIR)/$(OUTPUT_FILE)
	@rm -rf $(WAYLAND_PROTOCOLS)
endif


$(BUILD_DIR)/$(TARGET)/%.o: src/%.c
	@echo   $<...
	@$(CC) $< $(CFLAGS) -c -o $@ $(DEFINES) $(INCLUDES)

$(BUILD_DIR)/$(TARGET)/%.o: src/%.cpp
	@echo   $<...
	@$(CXX) $< $(CPPFLAGS) -c -o $@ $(DEFINES) $(INCLUDES)

# Third-party code has all warnings and errors disabled, except debugging options
$(BUILD_DIR)/$(TARGET)/thirdparty/%.o: src/thirdparty/%.c
	@echo 	$<...
	@$(CC) $< $(THIRDPARTY_CFLAGS) -c -o $@ $(DEFINES) $(INCLUDES)

-include $(OBJ_FILES:.o=.d)
