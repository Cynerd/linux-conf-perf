.PHONY: all clean
.SUFFIXES:

all: parse

KCONFIG_PREFIX = ../shared/kconfig
include $(KCONFIG_PREFIX)/files.mk

SRC  = parser.c \
	       cnfexpr.c \
	       symlist.c \
	       output.c
CFLAGS = -O0 -w -ggdb

parse: $(SRC) $(KCONFIG_SRC)
	gcc $(CFLAGS) -o $@ $^ -I../shared

clean::
	$(RM) parser
