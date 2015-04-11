
all: kconfig_parser

parse_kconfig:
	@$(MAKE) -C scripts/parse_kconfig/

main_loop: kconfig_parser
	scripts/main_loop.py


clean:
	@$(MAKE) -C scripts/parse_kconfig/ clean
	$(RM) linux/.config
	$(RM) -r build

# Linux has separate clean option because it takes more time and because in most of the time it is not required clean
clean_linux:
	@$(MAKE) -C linux clean
