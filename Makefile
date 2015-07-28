.PHONY: all help parse_kconfig write_config build run test clean clean_db clean_database clean_measure clean_linux clean_buildroot mlinux mbuildroot deflinux distclean_linux distclean_buildroot distclean picosat init initialize initialize_database initdb

-include .conf.mk

all: parse_kconfig write_config allconfig picosat

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

mbuildroot: buildroot/.config buildroot/system/skeleton/usr/bin/linux-conf-perf
	$(MAKE) -C buildroot menuconfig

mlinux:
	ARCH=$(CONF_KERNEL_ARCH) $(MAKE) -C linux menuconfig

deflinux:
	ARCH=$(CONF_KERNEL_ARCH) $(MAKE) -C linux defconfig

dot_config: allconfig
	cd linux && SRCARCH=$(CONF_KERNEL_ARCH) ARCH=$(CONF_KERNEL_ARCH) KERNELVERSION=$(CONF_KERNEL_ARCH) \
		../scripts/allconfig/allconfig Kconfig .config ../$(CONF_DOT_CONFIG)

init: initialize
initialize: all
	scripts/initialize.py

initdb: initialize_database
initialize_database:
	echo "$(CONF_DB_HOST):$(CONF_DB_PORT):$(CONF_DB_DATABASE):$(CONF_DB_USER):$(CONF_DB_PASSWORD)" > .pgpass
	psql -d "$(CONF_DB_DATABASE)" -h "$(CONF_DB_HOST)" -p "$(CONF_DB_PORT)" -f scripts/databaseinit.sql
	$(RM) .pgpass

test: parse_kconfig
	scripts/test.py

run: all
	scripts/loop.py

evaluate:
	scripts/evaluate.py

clean:
	@$(MAKE) -C scripts/parse_kconfig clean
	@$(MAKE) -C scripts/write_config clean
	@$(MAKE) -C scripts/allconfig clean
	@if [ -e scripts/picosat-959/makefile ]; then $(MAKE) -C scripts/picosat-959 clean; fi
	$(RM) .conf.mk
	$(RM) -r jobfiles

clean_measure: cleandb
	$(RM) -r configurations
	$(RM) -r output
	$(RM) -r result
	$(RM) $(CONF_DOT_CONFIG)

cleandb: clean_database
clean_database:
	echo "$(CONF_DB_HOST):$(CONF_DB_PORT):$(CONF_DB_DATABASE):$(CONF_DB_USER):$(CONF_DB_PASSWORD)" > .pgpass
	psql -d "$(CONF_DB_DATABASE)" -h "$(CONF_DB_HOST)" -p "$(CONF_DB_PORT)" -f scripts/databaseclean.sql
	rm -f .pgpass

distclean: clean distclean_linux distclean_buildroot clean_measure

clean_linux:
	@$(MAKE) -C linux clean

distclean_linux:
	@$(MAKE) -C linux distclean

clean_buildroot:
	@$(MAKE) -C buildroot clean

distclean_buildroot:
	@$(MAKE) -C buildroot distclean

#######################################

.conf.mk: conf.py
	scripts/confmk.py

parse_kconfig:
	@if [[ `$(MAKE) -C scripts/parse_kconfig/ -q; echo $$?` != "0" ]]; then \
	$(MAKE) -C scripts/parse_kconfig/; fi

write_config:
	@if [[ `$(MAKE) -C scripts/write_config/ -q; echo $$?` != "0" ]]; then \
	$(MAKE) -C scripts/write_config/; fi

allconfig:
	@if [[ `$(MAKE) -C scripts/allconfig/ -q; echo $$?` != "0" ]]; then \
	$(MAKE) -C scripts/allconfig/; fi

picosat:
	@if [ ! -e scripts/picosat-959/makefile ]; then \
	cd scripts/picosat-959 && ./configure; fi
	@if [[ `$(MAKE) -C scripts/picosat-959 -q; echo $$?` != "0" ]]; then \
	$(MAKE) -C scripts/picosat-959; fi

buildroot/.config:
	cp $(CONF_BUILDROOT_DEF_CONFIG) $@

buildroot/system/skeleton/usr/bin/linux-conf-perf:
	cp $(CONF_BUILDROOT_INITSCRIPT) $@
	cat $(CONF_BUILDROOT_INITTAB_DIRECTIVE) >> buildroot/system/skeleton/etc/inittab
