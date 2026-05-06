TARGET := freetype
include ../../utils/core.mk
include ../../utils/general.mk

GLX_USED := no

INCLUDES += -Iinclude
DEFINES += -DFT2_BUILD_LIBRARY -DDLL_EXPORT
# Remove errors from compilation
CCFLAGS := $(subst -Wall -Werror,,$(CCFLAGS))

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

CFLAGS += $(CCFLAGS)

DIRECTORIES :=

ifeq ($(PLATFORM), win32)
	DIRECTORIES := $(patsubst src,,$(patsubst src\\%,%,$(subst $(shell cd)\,,$(shell dir src /s /AD /B)))))
else
ifeq ($(PLATFORM), linux)
	DIRECTORIES := $(patsubst src/%,%,$(shell find src -type d -not -path "src"))
endif
endif

# Setup freetype file whitelist
FT_SRC := src
C_FILES := $(FT_SRC)/base/ftsystem.c 		\
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

# Collect object files
OBJ_FILES := $(patsubst src/%.c, $(BUILD_DIR)/$(TARGET)/%.o, $(C_FILES))

# Setup final binary
OUTPUT_NAME := libfreetype
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
	@echo Getting compile commands from $(TARGET)...
	@$(PYTHON) ../../utils/gen_tmp_commands.py --extension .c --arg_path src --args $(CC) $(CFLAGS) -c $(DEFINES) $(INCLUDES)
	@$(PYTHON) ../../utils/gen_tmp_commands.py --sources $(C_FILES) --objects $(OBJ_FILES)

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
	@$(PYTHON) ../../utils/windows/gen_dependency_files.py $< -o $@
else
	@$(CC) $< $(CFLAGS) -c -o $@ $(DEFINES) $(INCLUDES)
endif

-include $(OBJ_FILES:.o=.d)