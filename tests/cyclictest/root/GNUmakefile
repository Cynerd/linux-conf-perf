export SRCARCH=$(CONF_KERNEL_ARCH)

lastword = $(word $(words $(1)),$(1))
makedir := $(dir $(call lastword,$(MAKEFILE_LIST)))

MAKEARGS := -C $(makedir)/../../../scripts/buildroot
MAKEARGS += O=$(if $(patsubst /%,,$(makedir)),$(CURDIR)/)$(patsubst %/,%,$(makedir))

MAKEFLAGS += --no-print-directory

.PHONY: all $(MAKECMDGOALS)

all	:= $(filter-out Makefile .config,$(MAKECMDGOALS))

_all:
	$(MAKE) $(MAKEARGS) $(all)

-include ../../../.conf.mk

../../../.conf.mk:
	$(MAKE) -C ../../../ .conf.mk

_all: .config
.config: config_base ../../../$(CONF_TEST_CONFIG_CYCLICTEST)
	cp config_base .config
	cat ../../../$(CONF_TEST_CONFIG_CYCLICTEST) >> .config
	yes "" | make oldconfig

Makefile:;

$(all): _all
	@:

%/: _all
	@:
