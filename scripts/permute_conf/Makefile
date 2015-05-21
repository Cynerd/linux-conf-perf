MAKEFLAGS += --no-builtin-rules
.PHONY: all clean
.SUFFIXES:

all: permute_conf

KCONFIG_PREFIX = ../shared/kconfig
include $(KCONFIG_PREFIX)/files.mk

SRC  = permute_conf.c \
	   menudata.c \
	   dotconf.c
OBJ = $(patsubst %.c,%.o,$(SRC))
CFLAGS = -O0 -Wall -ggdb -DDEBUG
INCLUDES = -I../shared

%.o: %.c
	gcc -c $(CFLAGS) -o $@ $^ $(INCLUDES)

permute_conf: $(OBJ) $(KCONFIG_OBJ)
	gcc -o $@ $^

clean::
	$(RM) $(OBJ)
	$(RM) permute_conf
