# Files in this folder are exported from kernel source tree (except this file)
# 	commit: e5e02de0665ef2477e7a018193051387c6fe0fbc
.PHONY: clean

ifndef KCONFIG_PREFIX
	$(error "Please specify variable KCONFIG_SRC in your makefile before importing kconfig files.mk")
endif

KCONFIG_SRC = $(KCONFIG_PREFIX)/zconf.tab.c

%.hash.c: %.gperf
	gperf -t --output-file $@ -a -C -E -g -k '1,3,$$' -p -t $<

%.lex.c: %.l
	flex -o $@  -L -P zconf $<

$(KCONFIG_PREFIX)/zconf.tab.c: $(KCONFIG_PREFIX)/zconf.lex.c
$(KCONFIG_PREFIX)/zconf.tab.c: $(KCONFIG_PREFIX)/zconf.hash.c
%.tab.c: %.y
	bison -o $@ $< -p zconf -t -l


clean::
	$(RM) $(KCONFIG_PREFIX)/zconf.tab.c
	$(RM) $(KCONFIG_PREFIX)/zconf.lex.c
	$(RM) $(KCONFIG_PREFIX)/zconf.hash.c
