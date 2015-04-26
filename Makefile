.PHONY: all help parse_kconfig write_config build run test clean clean_linux clean_buildroot mlinux mbuildroot deflinux

-include .conf.mk

all: parse_kconfig write_config

help:
	@echo "all         - Builds basic programs and prints message about next steps."
	@echo "help        - Prints this text"
	@echo "mbuildroot  - Calls 'make menuconfig' in buildroot folder. Use this for"
	@echo "              buildroot configuration."
	@echo "mlinux      - Calls 'make menuconfig' in linux folder. Use this for linux"
	@echo "              configuration."
	@echo "deflinux    - Executes 'make defconfig' in linux folder. This generates default"
	@echo "              linux configuration for architecture specified in conf.py"
	@echo "test        - Executes boot and benchmark test. You should use this before"
	@echo "              target run. This target is for testing if initial kernel"
	@echo "              configuration, buildroot configuration and benchmark are"
	@echo "              configured right."
	@echo "run         - Executes loop of kernel building, booting and benchmark execution."
	@echo "evaluate    - Creating result statistics from generated data."
	@echo
	@echo "clean           - Cleans all generated files. Except those in linux and buildroot."
	@echo "clean_linux     - Executes 'make clean' in linux folder."
	@echo "clean_buildroot - Executes 'make clean' in buildroot folder."

mbuildroot:
	$(MAKE) -C scripts/buildroot menuconfig

mlinux:
	ARCH=$(SRCARCH) $(MAKE) -C linux menuconfig

deflinux:
	ARCH=$(SRCARCH) $(MAKE) -C linux defconfig

test: $(INITRAM)
	@ #TODO

run: kconfig_parser write_config $(INITRAM)
	scripts/main_loop.py

evaluate:
	@ #TODO

clean:
	@$(MAKE) -C scripts/parse_kconfig/ clean
	@$(MAKE) -C scripts/write_config/ clean
	$(RM) -r build

clean_linux:
	@$(MAKE) -C linux clean

clean_buildroot:
	@$(MAKE) -C scripts/buildroot clean

#######################################

.conf.mk: conf.py
	scripts/confmk.py

parse_kconfig:
	@$(MAKE) -C scripts/parse_kconfig/

write_config:
	@$(MAKE) -C scripts/write_config/

%:
	mkdir -p $@

$(BUILDROOT_INITRAM): scripts/buildroot/.config
	@$(MAKE) -C scripts/buildroot

$(INITRAM): $(BUILDROOT_INITRAM) $${@D}
	mv $^ $@

scripts/buildroot/.config: mbuildroot
	@
