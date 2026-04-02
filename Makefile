include utils/core.mk

.PHONY: all all-debug all-release generate_compile_commands clean

all-debug:
	@$(MAKE) -f $(firstword $(MAKEFILE_LIST)) all DEBUG=yes

all-release:
	@$(MAKE) -f $(firstword $(MAKEFILE_LIST)) all DEBUG=no

all:
	@$(MAKE) all -C engine 
	@$(MAKE) all -C editor

generate_compile_commands:
	@$(MAKE) generate_compile_commands -C engine
	@$(MAKE) generate_compile_commands -C editor
	@$(PYTHON) utils/gen_compile_commands.py --search editor engine

clean:
	@$(MAKE) clean -C engine
	@$(MAKE) clean -C editor
