TARGET := utils

COMPILEGEN := compilegen
COMPILEGEN_FLAGS := -std=gnu17 -Wall -Werror -O3
ifeq ($(PLATFORM), win32)
	COMPILEGEN := $(COMPILEGEN).exe
	COMPILEGEN_FLAGS += -D_CRT_SECURE_NO_WARNINGS
endif 

.PHONY: all clean

all: $(BUILD_DIR)/$(COMPILEGEN)

$(BUILD_DIR)/$(COMPILEGEN): src/compilegen.c
	@echo Building $@...
	@$(CC) src/compilegen.c -o $@ $(COMPILEGEN_FLAGS)
