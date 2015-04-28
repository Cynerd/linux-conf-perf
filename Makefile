.PHONY: all help parse_kconfig write_config build run test clean clean_linux clean_buildroot mlinux mbuildroot deflinux distclean_linux distclean_buildroot distclean

-include .conf.mk

BENCHMARK_FILES := $(patsubst benchmark/%,scripts/buildroot/system/skeleton/usr/share/benchmark/%,$(shell find benchmark -type f))
BENCHMARK_FOLDERS := $(shell dirname $(BENCHMARK_FILES))

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

test: $(INITRAM) parse_kconfig
	scripts/test.py

run: parse_kconfig write_config $(INITRAM)
	scripts/loop.py

evaluate:
	@ #TODO

clean:
	@$(MAKE) -C scripts/parse_kconfig/ clean
	@$(MAKE) -C scripts/write_config/ clean
	$(RM) -r build
	$(RM) -r scripts/buildroot/system/skeleton/usr/share/benchmark
	$(RM) $(INITRAM)
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

scripts/buildroot/system/skeleton/usr/share/%:
	mkdir -p $@

build:
	mkdir -p $@

$(BUILDROOT_INITRAM): scripts/buildroot/.config scripts/buildroot/system/skeleton/usr/bin/linux-conf-perf $(BENCHMARK_FILES)
	@$(MAKE) -C scripts/buildroot

$(INITRAM): build
$(INITRAM): $(BUILDROOT_INITRAM)
	cp $< $@

scripts/buildroot/.config:
	cp $(BUILDROOT_DEF_CONFIG) $@

scripts/buildroot/system/skeleton/usr/bin/linux-conf-perf:
	cp $(BUILDROOT_INITSCRIPT) $@
	cat $(BUILDROOT_INITTAB_DIRECTIVE) >> scripts/buildroot/system/skeleton/etc/inittab

$(BENCHMARK_FILES): $(BENCHMARK_FOLDERS)
scripts/buildroot/system/skeleton/usr/share/benchmark/%: benchmark/%
	cp $< $@
