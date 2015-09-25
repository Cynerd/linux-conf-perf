-include .conf.mk

.PHONY: all
all: parse_kconfig write_config allconfig picosat initram_cyclictest

.PHONY: help
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

.PHONY: psql
psql:
	PGPASSWORD="$(CONF_DB_PASSWORD)" psql -d "$(CONF_DB_DATABASE)" -h "$(CONF_DB_HOST)" -p "$(CONF_DB_PORT)"

.PHONY: mbuildroot
mbuildroot:
	$(MAKE) -C tests/cyclictest/root/ menuconfig

.PHONY: mlinux
mlinux:
	ARCH=$(CONF_KERNEL_ARCH) $(MAKE) -C $(CONF_LINUX_SOURCES) menuconfig

.PHONY: deflinux
deflinux:
	ARCH=$(CONF_KERNEL_ARCH) $(MAKE) -C $(CONF_LINUX_SOURCES) defconfig

dot_config: allconfig
	cd $(CONF_LINUX_SOURCES) && \
		SRCARCH=$(CONF_KERNEL_ARCH) \
		ARCH=$(CONF_KERNEL_ARCH) \
		KERNELVERSION=$(CONF_KERNEL_ARCH) \
		$(CONF_ABSROOT)/scripts/allconfig/allconfig \
		Kconfig .config $(CONF_ABSROOT)/$(CONF_DOT_CONFIG)

.PHONY: initialize init
init: initialize
initialize: all
	scripts/initialize.py

.PHONY: initdb initialize_database
initdb: initialize_database
initialize_database:
	PGPASSWORD="$(CONF_DB_PASSWORD)" psql -d "$(CONF_DB_DATABASE)" -h "$(CONF_DB_HOST)" -p "$(CONF_DB_PORT)" -f scripts/databaseinit.sql

.PHONY: test
test: parse_kconfig initram_cyclictest
	scripts/test.py

.PHONY: run
run: all
	scripts/loop.py

.PHONY: evaluate
evaluate:
	scripts/evaluate.py

.PHONY: clean
clean:
	@$(MAKE) -C scripts/parse_kconfig clean
	@$(MAKE) -C scripts/write_config clean
	@$(MAKE) -C scripts/allconfig clean
	@if [ -e scripts/picosat-959/makefile ]; then $(MAKE) -C scripts/picosat-959 clean; fi
	$(RM) .conf.mk
	$(RM) -r jobfiles

.PHONY: clean_measure
clean_measure:
	$(RM) -r configurations
	$(RM) -r output
	$(RM) -r result
	$(RM) $(CONF_DOT_CONFIG)

.PHONY: cleandb clean_database
cleandb: clean_database
clean_database:
	PGPASSWORD="$(CONF_DB_PASSWORD)" psql -d "$(CONF_DB_DATABASE)" -h "$(CONF_DB_HOST)" -p "$(CONF_DB_PORT)" -f scripts/databaseclean.sql

.PHONY: distclean
distclean: clean distclean_linux distclean_buildroot clean_measure

.PHONY: clean_linux
clean_linux:
	@$(MAKE) -C $(CONF_LINUX_SOURCES) clean

.PHONY: distclean_linux
distclean_linux:
	@$(MAKE) -C $(CONF_LINUX_SOURCES) distclean

.PHONY: clean_buildroot
clean_buildroot:
	@$(MAKE) -C tests/cyclictest/root/ clean

.PHONY: distclean_buildroot
distclean_buildroot:
	@$(MAKE) -C tests/cyclictest/root/ distclean

#######################################

.conf.mk: conf.py .target
	scripts/confmk.py

.target:
	$(error Please select target by writing it to .target file)

.PHONY: parse_kconfig
parse_kconfig:
	@if [ `$(MAKE) -C scripts/parse_kconfig/ -q; echo $$?` != "0" ]; then \
	$(MAKE) -C scripts/parse_kconfig/; fi

.PHONY: write_config
write_config:
	@if [ `$(MAKE) -C scripts/write_config/ -q; echo $$?` != "0" ]; then \
	$(MAKE) -C scripts/write_config/; fi

.PHONY:allconfig
allconfig:
	@if [ `$(MAKE) -C scripts/allconfig/ -q; echo $$?` != "0" ]; then \
	$(MAKE) -C scripts/allconfig/; fi

.PHONY: picosat
picosat:
	@if [ ! -e scripts/picosat-959/makefile ]; then \
	cd scripts/picosat-959 && ./configure; fi
	@if [ `$(MAKE) -C scripts/picosat-959 -q; echo $$?` != "0" ]; then \
	$(MAKE) -C scripts/picosat-959; fi

.PHONY: initram_cyclictest
initram_cyclictest:
	@if [ `$(MAKE) -C tests/cyclictest/root/ -q; echo $$?` != "0" ]; then \
	$(MAKE) -C scripts/cyclictest/root/; fi
