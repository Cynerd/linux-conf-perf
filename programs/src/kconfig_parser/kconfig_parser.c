#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <locale.h>
#include <stdbool.h>
#include "kconfig/lkc.h"
#include "symlist.h"
#include "output.h"

struct symlist *gsymlist;
int noname_num;

void build_symlist();
void cpy_dep();

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("No input file specified\n");
        exit(1);
    }
    if (argc < 3) {
        printf("No output folder specified\n");
        exit(2);
    }

    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);

    conf_parse(argv[1]);
    sym_clear_all_valid();

    gsymlist = symlist_create();

    build_symlist();
    cpy_dep();

    char *rules_file, *symbol_map_file;
    asprintf(&rules_file, "%s/%s", argv[2], DEFAULT_RULES_FILE);
    asprintf(&symbol_map_file, "%s/%s", argv[2], DEFAULT_SYMBOL_MAP_FILE);
    fprint_rules(gsymlist, rules_file);
    fprint_symbol_map(gsymlist, symbol_map_file);
    return 0;
}

void build_symlist() {
    int i;
    struct symbol *sym;
    for_all_symbols(i, sym) {
        if (sym->type == S_BOOLEAN || sym->type == S_TRISTATE) {
            if (sym->name != NULL) {
                symlist_add(gsymlist, sym->name);
            } else {
                sym->name = malloc((9 + 7) * sizeof(char));
                sprintf(sym->name, "NONAMEGEN%d", noname_num++);
                symlist_add(gsymlist, sym->name);
            }
        }
    }
}

void cpy_dep() {
    int i;
    struct symbol *sym;
    struct symlist_el *el;
    for_all_symbols(i, sym) {
        if ((sym->type == S_BOOLEAN || sym->type == S_TRISTATE)
            && strstr(sym->name, "NONAMEGEN") == NULL) {
            printf("working: %s\n", sym->name);

            el = symlist_find(gsymlist, sym->name);
            if (sym->dir_dep.expr != NULL) {
                printf_original(gsymlist, sym->dir_dep.expr);
                printf("Direct:\n");
                el->be = kconfig_cnfexpr(gsymlist, sym->dir_dep.expr);
                cnf_printf(el->be);
            }
            if (sym->rev_dep.expr != NULL) {
                if (!strcmp(sym->name, "CRC32"))
                        continue;
                printf_original(gsymlist, sym->rev_dep.expr);
                printf("Revers:\n");
                el->re_be = kconfig_cnfexpr(gsymlist, sym->rev_dep.expr);
                cnf_printf(el->re_be);
                el->re_be = kconfig_cnfexpr(gsymlist, sym->rev_dep.expr);
            }
        }
    }
}
