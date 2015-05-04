MAKEFLAGS += --no-builtin-rules
.PHONY: all clean
.SUFFIXES:

all: parse

KCONFIG_PREFIX = ../shared/kconfig
include $(KCONFIG_PREFIX)/files.mk

SRC  = parse.c \
	       boolexpr.c \
		   cnfbuild.c \
	       symlist.c \
		   doutput.c \
	       output.c
OBJ = $(patsubst %.c,%.o,$(SRC))
CFLAGS = -O0 -w -ggdb -DDEBUG
INCLUDES = -I../shared

%.o: %.c
	gcc -c $(CFLAGS) -o $@ $^ $(INCLUDES)

parse: $(OBJ) $(KCONFIG_OBJ)
	gcc -o $@ $^

clean::
	$(RM) $(OBJ)
	$(RM) parse
