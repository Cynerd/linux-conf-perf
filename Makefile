
all: kconfig_parser

kconfig_parser:
	@$(MAKE) -C scripts/kconfig_parser/

main_loop: kconfig_parser
	scripts/main_loop.py


clean:
	@$(MAKE) -C scripts/kconfig_parser/ clean
	$(RM) linux/.config
	$(RM) -r build

# Linux has separate clean option because it takes more time and because in most of the time it is not required clean
clean_linux:
	@$(MAKE) -C linux clean
