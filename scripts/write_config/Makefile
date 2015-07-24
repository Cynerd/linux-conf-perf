MAKEFLAGS += --no-builtin-rules
.PHONY: all clean
.SUFFIXES:

all: write_config

KCONFIG_PREFIX = ../shared/kconfig
include $(KCONFIG_PREFIX)/files.mk

SRC  = write_config.c
OBJ = $(patsubst %.c,%.o,$(SRC))
CFLAGS = -O0 -Wall -ggdb
INCLUDES = -I../shared

%.o: %.c
	gcc -c $(CFLAGS) -o $@ $^ $(INCLUDES)

write_config: $(OBJ) $(KCONFIG_OBJ)
	gcc -o $@ $^

clean::
	$(RM) $(OBJ)
	$(RM) write_config
