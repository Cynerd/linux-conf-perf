HELP=""
-include .conf.mk

HELP+="all         - Builds basic programs and prints message about next steps.\n"
.PHONY: all
all: parse_kconfig write_config allconfig picosat initram_cyclictest

.PHONY: help
help:
	@echo " help        - Prints this text"
	@echo -e $(HELP)

HELP+="psql        - Launch PostgreSQL interactive terminal.\n"
.PHONY: psql
psql:
	PGPASSWORD="$(CONF_DB_PASSWORD)" psql -d "$(CONF_DB_DATABASE)" -h "$(CONF_DB_HOST)" -p "$(CONF_DB_PORT)"

.PHONY: mbuildroot
mbuildroot:
	$(MAKE) -C tests/cyclictest/root/ menuconfig

HELP+="mlinux      - Calls 'make menuconfig' in Linux folder. Use this for Linux\n"
HELP+="              configuration.\n"
.PHONY: mlinux
mlinux:
	ARCH=$(CONF_KERNEL_ARCH) $(MAKE) -C $(CONF_LINUX_SOURCES) menuconfig

HELP+="deflinux    - Executes 'make menuconfig' in linux folder. This generates default\n"
HELP+="              linux configuration.\n"
.PHONY: deflinux
deflinux:
	ARCH=$(CONF_KERNEL_ARCH) $(MAKE) -C $(CONF_LINUX_SOURCES) defconfig

HELP+="dot_config  - Generate dot_config file. This file is based on default Linux\n"
HELP+="              configuration.\n"
dot_config: allconfig
	cd $(CONF_LINUX_SOURCES) && \
		SRCARCH=$(CONF_KERNEL_ARCH) \
		ARCH=$(CONF_KERNEL_ARCH) \
		KERNELVERSION=$(CONF_KERNEL_ARCH) \
		$(CONF_ABSROOT)/scripts/allconfig/allconfig \
		Kconfig .config $(CONF_ABSROOT)/$(CONF_DOT_CONFIG)

HELP+="initialize  - Executes only initialization.\n"
.PHONY: initialize init
init: initialize
initialize: all
	scripts/initialize.py

HELP+="initdb      - Initialize database.\n"
.PHONY: initdb initialize_database
initdb: initialize_database
initialize_database:
	PGPASSWORD="$(CONF_DB_PASSWORD)" psql -d "$(CONF_DB_DATABASE)" -h "$(CONF_DB_HOST)" -p "$(CONF_DB_PORT)" -f scripts/databaseinit.sql

HELP+="test        - Executes boot and benchmark test. You should use this before\n"
HELP+="              target run. This target is for testing if initial kernel\n"
HELP+="              configuration, buildroot configuration and benchmark are\n"
HELP+="              configured right.\n"
.PHONY: test
test: parse_kconfig initram_cyclictest
	scripts/test.py

HELP+="run         - Executes loop of kernel building, booting and benchmark execution.\n"
.PHONY: run
run: all
	scripts/loop.py

.PHONY: evaluate
evaluate:
	scripts/evaluate.py

HELP+="\n"

HELP+="clean           - Cleans all generated files. Except those in\n"
HELP+="                  linux and buildroot.\n"
.PHONY: clean
clean:
	@$(MAKE) -C scripts/parse_kconfig clean
	@$(MAKE) -C scripts/write_config clean
	@$(MAKE) -C scripts/allconfig clean
	@if [ -e scripts/picosat-959/makefile ]; then $(MAKE) -C scripts/picosat-959 clean; fi
	$(RM) .conf.mk
	$(RM) -r jobfiles

HELP+="clean_measure   - Removes working files used while measuring. After this,\n"
HELP+="                  initializations has to be executed once again.\n"
.PHONY: clean_measure
clean_measure:
	$(RM) -r configurations
	$(RM) -r output
	$(RM) -r result
	$(RM) $(CONF_DOT_CONFIG)

HELP+="clean_database  - Drop all tables in database.\n"
.PHONY: cleandb clean_database
cleandb: clean_database
clean_database:
	PGPASSWORD="$(CONF_DB_PASSWORD)" psql -d "$(CONF_DB_DATABASE)" -h "$(CONF_DB_HOST)" -p "$(CONF_DB_PORT)" -f scripts/databaseclean.sql

HELP+="distclean       - Cleans all configurations and generated files.\n"
HELP+="                  Including linux and buildroot.\n"
.PHONY: distclean
distclean: clean distclean_linux distclean_buildroot clean_measure

HELP+="clean_linux     - Executes 'make clean' in linux folder.\n"
.PHONY: clean_linux
clean_linux:
	@$(MAKE) -C $(CONF_LINUX_SOURCES) clean

HELP+="distclean_linux - Executes 'make distclean' in linux folder.\n"
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
