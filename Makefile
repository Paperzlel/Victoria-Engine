# Find out what OS the user is running prior to building.
export PLATFORM :=

ifeq ($(OS), Windows_NT)
	PLATFORM := win32
else
	PLATFORM := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
endif

ifeq ($(PLATFORM), Linux)
	PLATFORM := linux
endif
# NOTE: Add other options here, but we ignore them for now.

# Utilities

# Make does not offer a recursive wildcard function, so here's one:
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# General options
export VERSION :=
export IGNORE_VERSION_HASH := no
export DEBUG := no
export RUN_TESTS := no
export BUILD_DIR := bin
export COMPILER := clang
export PYTHON :=


# Auto-check platform options
ifeq ($(PLATFORM), linux)
	VERSION := $(shell python3 version.py)
	PYTHON := python3
endif
ifeq ($(PLATFORM), win32)
	VERSION := $(shell python version.py)
	PYTHON := python
endif

# Ignore commit hash on certain builds.
ifneq ($(IGNORE_VERSION_HASH), yes)
	VERSION := $(VERSION).$(shell git describe --always)
endif


# Compiler options
export CC := clang
export CXX := clang++
export CCFLAGS := -Wall -Werror
export CFLAGS := -std=c17
export CPPFLAGS := -std=c++17
export INCLUDES :=
export LDFLAGS :=
export DEFINES :=


# Confirm compiler options
ifeq ($(DEBUG), yes)
	DEFINES += -DDEBUG
	CCFLAGS += -MD -O0
	LDFLAGS += -g
ifeq ($(PLATFORM), win32)
# ifeq ($(COMPILER), clang)
# 	CCFLAGS += -g
# else
# 	CCFLAGS += -g
# endif
	CCFLAGS += -g
	DEFINES += -D_DEBUG
	LDFLAGS += -lucrtd -lmsvcrtd -lvcruntimed
else
	CCFLAGS += -gdwarf-4 -g3
# NOTE: Should enable -g2 for non-dev builds that want debugging symbols
endif
else
	LDFLAGS += -Wl,-s
	CCFLAGS += -O2
endif

ifeq ($(RUN_TESTS), yes)
	DEFINES += -DTESTS
endif

ifeq ($(COMPILER), gcc)
	CXX := g++
	CC := gcc
endif

# Confirm platform-specific compiler options
ifeq ($(PLATFORM), win32)
	DEFINES += -D_CRT_SECURE_NO_WARNINGS
endif

# Globalize build directory
BUILD_DIR := $(abspath $(BUILD_DIR))

.PHONY: all generate_compile_commands clean
all:
ifeq ($(PLATFORM), win32)
	-@mkdir $(subst /,\,$(BUILD_DIR)), 2>NUL || cd .
	-@xcopy "assets\" "$(subst /,\,$(BUILD_DIR)/assets)\" /q /y
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