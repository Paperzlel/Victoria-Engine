TARGET := engine
# We want to hide as much of the API as possible, so we set the visibility to hidden by default. This allows our own API to be exposed properly.
CFLAGS += -fPIC -fvisibility=hidden
CPPFLAGS += -fPIC -fvisibility=hidden
INCLUDES += -Iinclude -Iinclude/thirdparty/freetype/include -Iinclude/thirdparty/glad
DEFINE += -DVEXPORT

# Utilities

# Make does not offer a recursive wildcard function, so here's one:
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# Module options
export USE_FREETYPE := yes

# Platform-specific options
export USE_WAYLAND := no
export USE_X11 := no

ifeq ($(DETECTED_OS), LINUX)
ifeq ("$(wildcard /usr/include/wayland-client.h)", "")
	USE_WAYLAND := no
else
	USE_WAYLAND := yes
endif

ifeq ("$(wildcard /usr/include/X11/Xlib.h)", "")
	USE_X11 := no
else
	USE_X11 := yes
endif
endif

# Compiler-specific options
ifeq ($(COMPILER), gcc)
	LINKER_FLAGS += -shared -rdynamic
endif
ifeq ($(COMPILER), clang)
	CFLAGS += -fdeclspec
	CPPFLAGS += -fdeclspec
	LINKER_FLAGS += --shared
endif

# Module-specific options
ifeq ($(USE_FREETYPE), yes)
	DEFINES += -DFREETYPE_ENABLED -DFT2_BUILD_LIBRARY
endif


# OS-specific options
ifeq ($(DETECTED_OS), LINUX)
ifeq ($(USE_WAYLAND), yes)
	DEFINES += -DWAYLAND_ENABLED
	LINKER_FLAGS += -lwayland-client -lwayland-egl
	CFLAGS += -Wno-unused-const-variable
endif
ifeq ($(USE_X11), yes)
	DEFINES += -DX11_ENABLED
	LINKER_FLAGS += -lX11
endif
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
OUTPUT_FILE := libvictoria
ifeq ($(DETECTED_OS), WINDOWS)
	OUTPUT_FILE := $(OUTPUT_FILE).dll
else
ifeq ($(DETECTED_OS), LINUX)
	OUTPUT_FILE := $(OUTPUT_FILE).so
endif
endif

.PHONY: test scaffold compile link clean
all: test scaffold compile link

# Look for if certain files are downloaded prior to building
test:
ifeq ($(DETECTED_OS), LINUX)
ifeq ("$(wildcard /usr/include/wayland-client.h)", "")
ifeq ($(USE_WAYLAND), yes)
	@echo Cannot compile for Wayland whilst the drivers are not installed. Please install the development packages before compiling.
	@exit 10
endif
endif
ifeq ("$(wildcard /usr/include/X11/Xlib.h)", "")
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
ifeq ($(DETECTED_OS), WINDOWS)
	-@mkdir $(BUILD_DIR)/$(TARGET) 2>NUL || cd .
	-@mkdir $(addprefix $(BUILD_DIR)/$(TARGET)/,$(DIRECTORIES)) 2>NUL || cd .
endif
ifeq ($(DETECTED_OS), LINUX)
ifeq ($(USE_WAYLAND), yes)
# Add sources only if not present
ifeq ("$(wildcard src/platform/linux/wayland/protocols)","")
	@mkdir -p src/platform/linux/wayland/protocols
	@python3 src/platform/linux/wayland/generate_dynwrappers.py
endif
endif
	@mkdir -p $(BUILD_DIR)/$(TARGET)
	@mkdir -p $(addprefix $(BUILD_DIR)/$(TARGET)/,$(DIRECTORIES))

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


$(BUILD_DIR)/$(TARGET)/%.o: src/%.c
	@echo   $<...
	@$(CC) $< $(CFLAGS) -c -o $@ $(DEFINES) $(INCLUDES)

$(BUILD_DIR)/$(TARGET)/%.o: src/%.cpp
	@echo   $<...
	@$(CXX) $< $(CPPFLAGS) -c -o $@ $(DEFINES) $(INCLUDES)


-include $(OBJ_FILES:.o=.d)
