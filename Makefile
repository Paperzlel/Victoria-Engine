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

# Build options
export COMPILER :=
export PYTHON :=
export ARCH :=
export BUILD_DIR := bin
# If false, uses MSVC. If not, will throw an error.
export USE_MINGW := no
export USE_MSVC := no

# Linux platform detection
ifeq ($(PLATFORM), linux)
	VERSION := $(shell python3 version.py)
	PYTHON := python3
endif

# Windows platform detection
ifeq ($(PLATFORM), win32)
	VERSION := $(shell python version.py)
	PYTHON := python

ifeq ($(USE_MINGW), no)
ifneq ("$(shell where cl)","")
ifneq ($(COMPILER), clang)
	COMPILER := msvc
endif
	USE_MSVC := yes
endif
else
# Use MinGW
	TMP := $(shell echo MinGW is currently not supported.)
endif
endif

# Ignore commit hash on certain builds.
ifneq ($(IGNORE_VERSION_HASH), yes)
	VERSION := $(VERSION).$(shell git describe --always)
endif


# Compiler options. None by default.
export CC :=
export CXX :=
export LD :=
export CCFLAGS :=
export CFLAGS :=
export CPPFLAGS :=
export INCLUDES :=
export LDFLAGS :=
export DEFINES :=

ifeq ($(COMPILER), gcc)
	CXX := g++
	CC := gcc
	LD := ld
endif

# MSVC only requires its own special variables. Ignore for everybody else.
ifeq ($(USE_MSVC), yes)
ifeq ($(COMPILER), clang)
	CC := clang-cl
	CXX := clang-cl
	LD := lld-link
else
ifeq ($(COMPILER), msvc)
	CC := cl
	CXX := cl
	LD := link
endif
	CCFLAGS += /Wall 
	CFLAGS += /std:c17
	CPPFLAGS += /std:c++17
endif
# NOTE: MinGW might need an early exit to use the below commands.
else
# Real world
	CCFLAGS += -Wall -Werror
	CFLAGS += -std=c17
	CPPFLAGS += -std=c++17
endif 

# Confirm compiler options
ifeq ($(DEBUG), yes)
	DEFINES += -DDEBUG
ifeq ($(USE_MSVC), yes)
	CCFLAGS += /Zi /Z7 
	LDFLAGS += /DEBUG:FULL
	DEFINES += /D_DEBUG
else 
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
endif
else
	LDFLAGS += -Wl,-s
	CCFLAGS += -O2
endif

ifeq ($(RUN_TESTS), yes)
	DEFINES += -DTESTS
endif

# Confirm platform-specific compiler options
ifeq ($(PLATFORM), win32)
ifeq ($(USE_MSVC), yes)
	DEFINES += /D_CRT_SECURE_NO_WARNINGS
else
	DEFINES += -D_CRT_SECURE_NO_WARNINGS
endif
endif

# Globalize build directory
BUILD_DIR := $(abspath $(BUILD_DIR))

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
