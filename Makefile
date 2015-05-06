.PHONY: all help parse_kconfig write_config build run test clean clean_linux clean_buildroot mlinux mbuildroot deflinux distclean_linux distclean_buildroot distclean picosat init initialize

-include .conf.mk

all: parse_kconfig write_config picosat

help:
	@echo "all         - Builds basic programs and prints message about next steps."
	@echo "help        - Prints this text"
	@echo "mbuildroot  - Calls 'make menuconfig' in buildroot folder. Use this for"
	@echo "              buildroot configuration."
	@echo "mlinux      - Calls 'make menuconfig' in linux folder. Use this for linux"
	@echo "              configuration."
	@echo "deflinux    - Executes 'make defconfig' in linux folder. This generates default"
	@echo "              linux configuration for architecture specified in conf.py"
	@echo "initialize  - Executes only initialization. Depending on configuration this"
	@echo "              can take various amount of time."
	@echo "test        - Executes boot and benchmark test. You should use this before"
	@echo "              target run. This target is for testing if initial kernel"
	@echo "              configuration, buildroot configuration and benchmark are"
	@echo "              configured right."
	@echo "run         - Executes loop of kernel building, booting and benchmark execution."
	@echo "evaluate    - Creating result statistics from generated data."
	@echo
	@echo "clean               - Cleans all generated files. Except those in"
	@echo "                      linux and buildroot."
	@echo "distclean           - Cleans all configurations and generated files."
	@echo "                      Including linux and buildroot."
	@echo "clean_linux         - Executes 'make clean' in linux folder."
	@echo "distclean_linux     - Executes 'make distclean' in linux folder."
	@echo "clean_buildroot     - Executes 'make clean' in buildroot folder."
	@echo "distclean_buildroot - Executes 'make distclean' in buildroot folder."

mbuildroot: scripts/buildroot/.config scripts/buildroot/system/skeleton/usr/bin/linux-conf-perf $(BENCHMARK_FILES)
	$(MAKE) -C scripts/buildroot menuconfig

mlinux:
	ARCH=$(SRCARCH) $(MAKE) -C linux menuconfig

deflinux:
	ARCH=$(SRCARCH) $(MAKE) -C linux defconfig

init: initialize
initialize:
	scripts/initialize.py

test: $(BUILDROOT_INITRAM) parse_kconfig
	scripts/test.py

run: parse_kconfig write_config picosat $(BUILDROOT_INITRAM)
	scripts/loop.py

evaluate:
	@ #TODO

clean:
	@$(MAKE) -C scripts/parse_kconfig clean
	@$(MAKE) -C scripts/write_config clean
	@if [ -e scripts/picosat-959/makefile ]; then $(MAKE) -C scripts/picosat-959 clean; fi
	$(RM) .conf.mk
	$(RM) -r build
	$(RM) $(NBSCRIPT)

distclean: clean distclean_linux distclean_buildroot
	$(RM) .conf.mk

clean_linux:
	@$(MAKE) -C linux clean

distclean_linux:
	@$(MAKE) -C linux distclean

clean_buildroot:
	@$(MAKE) -C scripts/buildroot clean

distclean_buildroot:
	@$(MAKE) -C scripts/buildroot distclean

#######################################

.conf.mk: conf.py
	scripts/confmk.py

parse_kconfig:
	@$(MAKE) -C scripts/parse_kconfig/

write_config:
	@$(MAKE) -C scripts/write_config/

$(BUILDROOT_INITRAM): scripts/buildroot/.config scripts/buildroot/system/skeleton/usr/bin/linux-conf-perf
	@$(MAKE) -C scripts/buildroot

scripts/buildroot/.config:
	cp $(BUILDROOT_DEF_CONFIG) $@

scripts/buildroot/system/skeleton/usr/bin/linux-conf-perf:
	cp $(BUILDROOT_INITSCRIPT) $@
	cat $(BUILDROOT_INITTAB_DIRECTIVE) >> scripts/buildroot/system/skeleton/etc/inittab

picosat: scripts/picosat-959/picosat
scripts/picosat-959/picosat:
	cd scripts/picosat-959 && ./configure
	$(MAKE) -C scripts/picosat-959

#######################################

$(PHASE_FILE): initialize
$(SYMBOL_MAP_FILE): initialize
$(RULES_FILE): initialize
$(VARIABLE_COUNT_FILE): initialize
