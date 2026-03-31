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

export PYTHON :=
export FIND :=

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
