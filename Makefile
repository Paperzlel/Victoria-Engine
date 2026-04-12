include utils/core.mk

.PHONY: all all-debug all-release generate_compile_commands clean clean-all

all-debug:
	@$(MAKE) -f $(firstword $(MAKEFILE_LIST)) all DEBUG=yes

all-release:
	@$(MAKE) -f $(firstword $(MAKEFILE_LIST)) all DEBUG=no

all:
	@$(MAKE) all -C freetype
	@$(MAKE) all -C glad
	@$(MAKE) all -C victoria.core
	@$(MAKE) all -C victoria.runtime
	@$(MAKE) all -C victoria.tests 
	@$(MAKE) all -C victoria.testbed

generate_compile_commands:
	@$(MAKE) generate_compile_commands -C freetype
	@$(MAKE) generate_compile_commands -C glad
	@$(MAKE) generate_compile_commands -C victoria.core
	@$(MAKE) generate_compile_commands -C victoria.runtime
	@$(MAKE) generate_compile_commands -C victoria.tests
	@$(MAKE) generate_compile_commands -C victoria.testbed
	@$(PYTHON) utils/gen_compile_commands.py --search victoria.testbed victoria.tests victoria.core victoria.runtime glad freetype

clean:
	@$(MAKE) clean -C victoria.core
	@$(MAKE) clean -C victoria.runtime
	@$(MAKE) clean -C victoria.testbed
	@$(MAKE) clean -C victoria.tests

clean-all: clean
	@$(MAKE) clean -C freetype
	@$(MAKE) clean -C glad