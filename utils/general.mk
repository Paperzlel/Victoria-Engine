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
export FIND :=
export ARCH :=
# Build binary files in the "lib" directory
export BUILD_DIR := ../bin
# If false, uses MSVC. If not, will throw an error.
export USE_MINGW := no
export USE_MSVC := no

# Linux platform detection
ifeq ($(PLATFORM), linux)
	PYTHON := python3
	FIND := whereis
endif

# Windows platform detection
ifeq ($(PLATFORM), win32)
	PYTHON := python
	FIND := where
endif

ifeq ($(USE_MSVC), yes)
	COMPILER := msvc
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

# Check compiler availability. If none is selected, the "COMPILER" variable will be empty and an error should
# be thrown at the earliest convenience. 
export ERROR_THROW := no

# Check MSVC support first. Only Windows is supported.
# windows
ifeq ($(PLATFORM), win32)
# cl is present
ifneq ("$(shell $(FIND) cl)","")
	COMPILER := msvc
	USE_MSVC := yes
else
# cl not present, clang-cl is present
ifneq ("$(shell $(FIND) clang-cl)","")
	COMPILER := clang
	USE_MSVC := yes
endif
endif
endif

# has msvc
ifeq ($(USE_MSVC), yes)
# compiler is cl
ifeq ($(COMPILER), msvc)
# cl not present
ifeq ("$(shell $(FIND) cl)","")
	TMP := $(shell echo Unable to use the MSVC compiler as it is not present on the system PATH. Try launching this makefiles from Visual Studio, or by using the VS command prompt. Attempting to fall back to clang-cl.)
	COMPILER := clang
endif
endif
# compiler is clang-cl
ifeq ($(COMPILER), clang)
# clang-cl not present
ifeq ("$(shell $(FIND) clang-cl)","")
	TMP := $(shell echo Unable to use clang-cl as it is not present on the system PATH. Falling back to regular clang.)
	USE_MSVC := no
endif
endif
endif

# Check GCC after MSVC. If present, try to use it by default on non-Windows distros.
# not windows
ifneq ($(PLATFORM), win32)
# gcc is present
ifneq ("$(shell $(FIND) gcc)","")
# no compiler specified
ifeq ("$(COMPILER)","")
	COMPILER := gcc
endif
# gcc not present. assume clang.
else
	COMPILER := clang
endif
endif

# compiler is gcc
ifeq ($(COMPILER), gcc)
# gcc not present (assume overridden)
ifeq ("$(shell $(FIND) gcc)","")
	TMP := $(shell echo Unable to use gcc as it is not present on the system PATH. Falling back to clang.)
	COMPILER := clang
endif
endif

# Check regular clang last. This will always be the default on Windows if MSVC and GCC are not present.
# is clang
ifeq ($(COMPILER), clang)
# clang not present
ifeq ("$(shell $(FIND) clang)","")
	TMP := $(shell echo Unable to use clang as it is not present on the system PATH. No compiler is available unable to build.)
	ERROR_THROW := yes
endif
endif

# Set compiler settings and their respective defaults.

# MSVC only requires its own special variables. Ignore for everybody else.
ifeq ($(USE_MSVC), yes)
ifeq ($(COMPILER), clang)
	CC := clang-cl
	CXX := clang-cl
	LD := lld-link
else
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
	CCFLAGS += /nologo /W3 /Zc:__cplusplus /EHsc
	LDFLAGS += /nologo
endif
# Disable universal cl stuff
	CCFLAGS += /permissive-
	CFLAGS += /std:c17
	CPPFLAGS += /std:c++17
else
# Not MSVC
ifeq ($(COMPILER), gcc)
	CC := gcc
	CXX := g++
	LD := ld
endif
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
# Pass -MD to clang directly, MSVC doesn't autogen otherwise.
# TODO: Clang-cl bug prevents this from working properly, so it's disabled for now.
# ifeq ($(COMPILER), clang)
# 	CCFLAGS += /clang:-MD
# endif
else
	CCFLAGS += -MD -O0
	LDFLAGS += -g
ifeq ($(PLATFORM), win32)
ifeq ($(COMPILER), clang)
	CCFLAGS += -gdwarf-4
else
	CCFLAGS += -gdwarf-5
endif
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
