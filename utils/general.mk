# Declare everything in a singular shell.
.ONESHELL:

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
export DEBUG := no
export RUN_TESTS := no

# Build options
export COMPILER :=
export PYTHON :=
export ARCH :=
# Build binary files in the "lib" directory
export BUILD_DIR := ../bin
# If false, uses MSVC. If not, will throw an error.
export USE_MINGW := no
export USE_MSVC := no

# Linux platform detection
ifeq ($(PLATFORM), linux)
	PYTHON := python3
endif

# Windows platform detection
ifeq ($(PLATFORM), win32)
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
# Disable MSVC options. There's a bunch and they suck.
# In order:
# - C4244: Conversion is normal. The compiler can handle it and we expect it.
# - C4267: Conversion is normal.
# - C4251: This error is partially true, but most of the code it was worrying about was template classes. Should be fine.
# - C4305: Conversion from double to float. 
	CCFLAGS += /wd4244\
	/wd4267\
	/wd4251\
	/wd4305
endif
	CCFLAGS += /nologo /W3 /permissive- /Zc:__cplusplus /EHsc
	CFLAGS += /std:c17
	CPPFLAGS += /std:c++17
	LDFLAGS += /nologo
endif
# NOTE: MinGW might need an early exit to use the below commands.
else
ifeq ("$(COMPILER)","")
	COMPILER := clang
	CXX := clang++
	CC := clang
	LD := clang++
endif
# Real world
	CCFLAGS += -Wall -Werror
	CFLAGS += -std=c17
	CPPFLAGS += -std=c++17
endif 

# Confirm compiler options
ifeq ($(DEBUG), yes)
	DEFINES += -DDEBUG
ifeq ($(USE_MSVC), yes)
	CCFLAGS += /Zi /FS 
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
	DEFINES += -D_CRT_SECURE_NO_WARNINGS
endif

# Globalize build directory
BUILD_DIR := $(abspath $(BUILD_DIR))
