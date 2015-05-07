MAKEFLAGS += --no-builtin-rules
.PHONY: all clean
.SUFFIXES:

all: setpermute

KCONFIG_PREFIX = ../shared/kconfig
include $(KCONFIG_PREFIX)/files.mk

SRC  = permute.c
OBJ = $(patsubst %.c,%.o,$(SRC))
CFLAGS = -O0 -Wall -ggdb -DDEBUG
INCLUDES = -I../shared

%.o: %.c
	gcc -c $(CFLAGS) -o $@ $^ $(INCLUDES)

permute: $(OBJ) $(KCONFIG_OBJ)
	gcc -o $@ $^

clean::
	$(RM) $(OBJ)
	$(RM) setpermute
