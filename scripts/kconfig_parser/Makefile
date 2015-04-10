.PHONY: all clean
.SUFFIXES:

all: parser

KCONFIG_PREFIX = ../shared/kconfig
include $(KCONFIG_PREFIX)/files.mk

SRC  = parser.c \
	       cnfexpr.c \
	       symlist.c \
	       output.c
CFLAGS = -O0 -w -ggdb

parser: $(SRC) $(KCONFIG_SRC)
	gcc $(CFLAGS) -o $@ $^ -I$(KCONFIG_PREFIX)

clean::
	$(RM) parser
