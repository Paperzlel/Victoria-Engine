include ../utils/core.mk
include ../utils/general.mk

TARGET := glad
INCLUDES += -Iinclude
DEFINES += -DGLAD_API_CALL_EXPORT -DGLAD_API_CALL_EXPORT_BUILD

GLX_USED := no

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
ifeq ($(shell pkg-config --exists x11; echo $$?),0)
	GLX_USED := yes
	DEFINES += -DGLAD_GLX_NO_X11
endif
endif

ifeq ($(PLATFORM), win32)
	LDFLAGS += -luser32 -lopengl32 -lgdi32
# Force MSVC variables to their proper form.
ifeq ($(USE_MSVC), yes)
	INCLUDES := $(patsubst -I%,/I%,$(INCLUDES))
	LDFLAGS := $(patsubst -l%,%.lib,$(LDFLAGS))
	DEFINES := $(patsubst -D%,/D%,$(DEFINES))
endif
endif

CFLAGS += $(CCFLAGS)

# Setup GLAD file whitelist
C_FILES := src/gl.c \
	src/egl.c
# GLX will fail to compile on Windows, preventative measure.
ifeq ($(GLX_USED),yes)
	C_FILES += src/glx.c
endif

# Collect object files
OBJ_FILES := $(patsubst src/%.c, $(BUILD_DIR)/$(TARGET)/%.o, $(C_FILES))

# Setup final binary
OUTPUT_NAME := libGLAD
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
endif
ifeq ($(PLATFORM), linux)
	@mkdir -p $(BUILD_DIR)/$(TARGET)
endif
	@echo Done.
	@echo Compiling...

generate_compile_commands:
	@echo Getting compile commands from $(TARGET)...
	@$(PYTHON) ../utils/gen_tmp_commands.py --extension .c --arg_path src --args $(CC) $(CFLAGS) -c $(DEFINES) $(INCLUDES)
	@$(PYTHON) ../utils/gen_tmp_commands.py --sources $(C_FILES) --objects $(OBJ_FILES)

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

-include $(OBJ_FILES:.o=.d)