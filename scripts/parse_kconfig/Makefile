MAKEFLAGS += --no-builtin-rules
.PHONY: all clean
.SUFFIXES:

all: parse

KCONFIG_PREFIX = ../shared/kconfig
include $(KCONFIG_PREFIX)/files.mk

SRC  = parser.c \
	       cnfexpr.c \
	       symlist.c \
	       output.c
OBJ = $(patsubst %.c,%.o,$(SRC))
CFLAGS = -O0 -w -ggdb
INCLUDES = -I../shared

%.o: %.c
	gcc -c $(CFLAGS) -o $@ $^ $(INCLUDES)

parse: $(OBJ) $(KCONFIG_OBJ)
	gcc -o $@ $^

clean::
	$(RM) $(OBJ)
	$(RM) parser
