.SUFFIXES:

all: kconfig_parser


clean:
	rm -f kconfig/zconf.tab.c kconfig/zconf.lex.c kconfig/zconf.hash.c
	rm -f ../../kconfig_parser

INPUT_FILES  = kconfig_parser.c
INPUT_FILES += kconfig/zconf.tab.c \
			   boolexp.c \
			   symlist.c \
			   output.c

kconfig_parser: $(INPUT_FILES) kconfig/zconf.lex.c kconfig/zconf.hash.c
	gcc -O0 -w -ggdb -o ../../$@ $(INPUT_FILES)

%.hash.c: %.gperf
	gperf -t --output-file $@ -a -C -E -g -k '1,3,$$' -p -t $<

%.lex.c: %.l
	flex -o $@  -L -P zconf $<

%.tab.c: %.y kconfig/zconf.lex.c kconfig/zconf.hash.c
	bison -o $@ $< -p zconf -t -l
