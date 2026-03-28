
.PHONY: all all-debug all-release generate_compile_commands clean

all-debug:
	@$(MAKE) -f $(lastword $(MAKEFILE_LIST)) all DEBUG=yes

all-release:
	@$(MAKE) -f $(lastword $(MAKEFILE_LIST)) all DEBUG=no

all:
ifeq ($(PLATFORM), win32)
	-@mkdir $(subst /,\,$(BUILD_DIR)), 2>NUL || cd .
	-@xcopy "assets\" "$(subst /,\,$(BUILD_DIR)/assets)\" /q /y
ifeq ($(USE_MSVC), yes)
ifeq ($(COMPILER), clang)
ifeq ("$(shell where clang-cl)","")
	@echo Unable to locate a valid clang-cl installation. Are you sure it is on the path?
endif
else
ifeq ("$(shell where cl)","")
	@echo Unable to locate a valid cl installation. Are you sure you have set up the development environment correctly?
endif
endif
endif
endif
ifeq ($(PLATFORM), linux)
	@mkdir -p $(BUILD_DIR)
	@cp -r assets $(BUILD_DIR)
endif

	@$(MAKE) -C engine
	@$(MAKE) -C editor

generate_compile_commands:
	@$(MAKE) generate_compile_commands -C engine
	@$(MAKE) generate_compile_commands -C editor
	@$(PYTHON) misc/gen_compile_commands.py --search editor engine

clean:
	@$(MAKE) clean -C engine
	@$(MAKE) clean -C editor
ifeq ($(PLATFORM), win32)
	@del /F /Q $(subst /,\,$(BUILD_DIR)/assets) 2>NUL || cd .
	@rmdir $(subst /,\,$(BUILD_DIR)/assets) /S /Q 2>NUL || cd .
endif
ifeq ($(PLATFORM), linux)
	@rm -rf $(BUILD_DIR)/assets
endif
