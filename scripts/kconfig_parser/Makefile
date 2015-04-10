.SUFFIXES:

SRC  = parser.c \
	       cnfexpr.c \
	       symlist.c \
	       output.c
SRC += kconfig/zconf.tab.c
CFLAGS = -O0 -w -ggdb

parser: $(SRC) kconfig/zconf.lex.c kconfig/zconf.hash.c
	gcc $(CFLAGS) -o $@ $(SRC)

%.hash.c: %.gperf
	gperf -t --output-file $@ -a -C -E -g -k '1,3,$$' -p -t $<

%.lex.c: %.l
	flex -o $@  -L -P zconf $<

%.tab.c: %.y kconfig/zconf.lex.c kconfig/zconf.hash.c
	bison -o $@ $< -p zconf -t -l

clean:
	$(RM) kconfig/zconf.tab.c kconfig/zconf.lex.c kconfig/zconf.hash.c
	$(RM) parser
