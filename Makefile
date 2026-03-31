export PYTHON := python

.PHONY: all all-debug all-release generate_compile_commands clean

all-debug:
	@$(MAKE) -f $(lastword $(MAKEFILE_LIST)) all DEBUG=yes

all-release:
	@$(MAKE) -f $(lastword $(MAKEFILE_LIST)) all DEBUG=no

all:
	@$(MAKE) all -C engine 
	@$(MAKE) all -C editor

generate_compile_commands:
	@$(MAKE) generate_compile_commands -C engine
	@$(MAKE) generate_compile_commands -C editor
	@$(PYTHON) misc/gen_compile_commands.py --search editor engine

clean:
	@$(MAKE) clean -C engine
	@$(MAKE) clean -C editor
