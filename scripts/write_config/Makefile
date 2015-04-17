MAKEFLAGS += --no-builtin-rules
.PHONY: all clean
.SUFFIXES:

all: write

KCONFIG_PREFIX = ../shared/kconfig
include $(KCONFIG_PREFIX)/files.mk

SRC  = write.c \
	   symlist.c \
	   solution.c
OBJ = $(patsubst %.c,%.o,$(SRC))
CFLAGS = -O0 -w -ggdb
INCLUDES = -I../shared

%.o: %.c
	gcc -c $(CFLAGS) -o $@ $^ $(INCLUDES)

write: $(OBJ) $(KCONFIG_OBJ)
	gcc -o $@ $^

clean::
	$(RM) $(OBJ)
	$(RM) write
