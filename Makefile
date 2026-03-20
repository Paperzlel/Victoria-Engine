# Find out what OS the user is running prior to building.
export DETECTED_OS :=

ifeq ($(OS), Windows_NT)
	DETECTED_OS := WINDOWS
else
	DETECTED_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
endif

ifeq ($(DETECTED_OS), Linux)
	DETECTED_OS := LINUX
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
export PYTHON := python


# Auto-check platform options
ifeq ($(DETECTED_OS), LINUX)
	VERSION := $(shell python3 version.py)
	PYTHON := python3
endif
ifeq ($(DETECTED_OS), WINDOWS)
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
export CFLAGS := -std=c17 -Wall -Werror
export CPPFLAGS := -std=c++17 -Wall -Werror
export INCLUDES :=
export LINKER_FLAGS :=
export DEFINES :=


# Confirm compiler options
ifeq ($(DEBUG), yes)
	DEFINES += -DDEBUG
	CFLAGS += -MD
	CPPFLAGS += -MD
ifeq ($(COMPILER), gcc)
	CFLAGS += -ggdb
	CPPFLAGS += -ggdb
endif
ifeq ($(COMPILER), clang)
	CFLAGS += -gdwarf-5
	CPPFLAGS += -gdwarf-5
endif
endif
ifeq ($(RUN_TESTS), yes)
	DEFINES += -DTESTS
endif

ifeq ($(COMPILER), gcc)
	CXX := g++
	CC := gcc
	CPPFLAGS += -Wno-sign-compare -Wno-dangling-pointer
endif

# Confirm platform-specific compiler options
ifeq ($(DETECTED_OS), WINDOWS)
	LINKER_FLAGS += -luser32 -lopengl32 -lgdi32
	DEFINES += -D_CRT_SECURE_NO_WARNINGS
ifeq ($(DEBUG), yes)
	DEFINES += -D_DEBUG
# Introduce these in the RID chapter. Links to the Visual Studio libraries, so in theory shouldn't appear in GCC builds on Windows.
	LINKER_FLAGS += -g -lmsvcrtd -lvcruntimed -lucrtd
endif
endif

# Globalize build directory
BUILD_DIR := $(abspath $(BUILD_DIR))

.PHONY: all clean
all:
ifeq ($(DETECTED_OS), WINDOWS)
	-@mkdir $(BUILD_DIR), 2>NUL || cd .
	-@xcopy -r assets $(BUILD_DIR)
endif
ifeq ($(DETECTED_OS), LINUX)
	@mkdir -p $(BUILD_DIR)
	@cp -r assets $(BUILD_DIR)
endif

	@$(MAKE) -C engine
	@$(MAKE) -C editor

clean:
	@$(MAKE) clean -C engine
	@$(MAKE) clean -C editor
ifeq ($(DETECTED_OS), WINDOWS)
	@rmdir $(BUILD_DIR) /S /Q
endif
ifeq ($(DETECTED_OS), LINUX)
	@rm -rf $(BUILD_DIR)
endif
