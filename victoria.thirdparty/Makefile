include ../utils/core.mk

.PHONY: all generate_compile_commands clean

all:
	@$(MAKE) all -C freetype
	@$(MAKE) all -C glad

generate_compile_commands:
	@$(MAKE) generate_compile_commands -C freetype
	@$(MAKE) generate_compile_commands -C glad

clean:
	@$(MAKE) clean -C freetype
	@$(MAKE) clean -C glad
