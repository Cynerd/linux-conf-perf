#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <stdbool.h>
#include <libintl.h>
#include <kconfig/lkc.h>
#include "symlist.h"
#include "output.h"
#include "macros.h"

int verbose_level;
char *file, *folder;

struct symlist *gsymlist;
int noname_num;

void build_symlist();
void cpy_dep();

int main(int argc, char **argv) {
    verbose_level = 1;
    int i;
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-v"))
            verbose_level++;
        else if (file == NULL)
            file = argv[i];
        else if (folder == NULL)
            folder = argv[i];
        else {
            Eprintf("Unknown parameter: %s\n", argv[i]);
            exit(1);
        }
    }

    if (file == NULL) {
        Eprintf("No Kconfig input file specified\n");
        exit(2);
    }
    if (folder == NULL) {
        Eprintf("No output folder specified\n");
        exit(3);
    }

    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);

    conf_parse(file);
    //sym_clear_all_valid();

    gsymlist = symlist_create();

    build_symlist();
    cpy_dep();

    char *rules_file, *symbol_map_file;
    asprintf(&rules_file, "%s/%s", folder, DEFAULT_RULES_FILE);
    asprintf(&symbol_map_file, "%s/%s", folder, DEFAULT_SYMBOL_MAP_FILE);
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
        struct property *prop;
        for_all_prompts(sym, prop) {
            gsymlist->array[gsymlist->pos - 1].prompt = true;
            break;
        }
    }
}

void cpy_dep() {
    int i;
    struct symbol *sym;
    struct symlist_el *el;
    for_all_symbols(i, sym) {
        if ((sym->type == S_BOOLEAN || sym->type == S_TRISTATE)) {
            el = symlist_find(gsymlist, sym->name);
            Iprintf("working: %s(%d)\n", sym->name, el->id);

            if (sym->dir_dep.expr != NULL) {
                if (verbose_level > 3)
                    printf_original(gsymlist, sym->dir_dep.expr);
                el->be =
                    kconfig_cnfexpr(gsymlist, false, sym,
                                    sym->dir_dep.expr);
                Iprintf("Direct:\n");
                if (verbose_level > 2)
                    cnf_printf(el->be);
            } else
                el->be = NULL;
            if (sym->rev_dep.expr != NULL) {
                if (verbose_level > 3)
                    printf_original(gsymlist, sym->rev_dep.expr);
                el->re_be =
                    kconfig_cnfexpr(gsymlist, true, sym,
                                    sym->rev_dep.expr);
                Iprintf("Revers:\n");
                if (verbose_level > 2)
                    cnf_printf(el->re_be);
            } else
                el->re_be = NULL;
        }
    }
}
