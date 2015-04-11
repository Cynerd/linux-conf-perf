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
#include <argp.h>
#include "kconfig/lkc.h"
#include "symlist.h"
#include "output.h"
#include "macros.h"

int verbose_level;
char *file;

struct symlist *gsymlist;
int noname_num;

void build_symlist();
void cpy_dep();

int main(int argc, char **argv) {
    // TODO argp
    verbose_level = 1;
    int i;
    for (i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-v"))
            verbose_level++;
        else if (file == NULL)
            file = argv[i];
    }

    if (argc < 2) {
        Eprintf("No input file specified\n");
        exit(1);
    }
    if (argc < 3) {
        Eprintf("No output folder specified\n");
        exit(2);
    }

    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);

    conf_parse(argv[1]);
    //sym_clear_all_valid();

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
            el = symlist_find(gsymlist, sym->name);
            Iprintf("working: %s(%d)\n", sym->name, el->id);

            if (sym->dir_dep.expr != NULL) {
                if (verbose_level > 3)
                    printf_original(gsymlist, sym->dir_dep.expr);
                el->be = kconfig_cnfexpr(gsymlist, false, sym->dir_dep.expr);
                Iprintf("Direct:\n");
                if (verbose_level > 2)
                    cnf_printf(el->be);
            }
            if (sym->rev_dep.expr != NULL) {
                if (verbose_level > 3)
                    printf_original(gsymlist, sym->rev_dep.expr);
                el->re_be = kconfig_cnfexpr(gsymlist, true, sym->rev_dep.expr);
                Iprintf("Revers:\n");
                if (verbose_level > 2)
                    cnf_printf(el->re_be);
            }
        }
    }
}
