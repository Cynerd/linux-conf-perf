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

    char *rules_file, *symbol_map_file;
    asprintf(&rules_file, "%s/%s", folder, DEFAULT_RULES_FILE);
    asprintf(&symbol_map_file, "%s/%s", folder, DEFAULT_SYMBOL_MAP_FILE);
    output_init(rules_file, symbol_map_file);

    build_symlist();

    return 0;
}

void build_symlist() {
    int i;
    struct symbol *sym;
    for_all_symbols(i, sym) {
        if (sym->type == S_BOOLEAN || sym->type == S_TRISTATE) {
            if (sym->name == NULL) {
                sym->name = malloc((9 + 7) * sizeof(char));
                sprintf(sym->name, "NONAMEGEN%d", noname_num++);
            }
            symlist_add(gsymlist, sym->name);
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
    unsigned el_id;
    for_all_symbols(i, sym) {
        if ((sym->type == S_BOOLEAN || sym->type == S_TRISTATE)) {
            el_id = symlist_id(gsymlist, sym->name);
            el = &(gsymlist->array[el_id - 1]);

            for_all_defaults(sym, prop) {
                gsymlist->array[gsymlist->pos - 1].def = true;
                struct cnfexpr *def =
                    kconfig_cnfexpr(gsymlist, prop->expr);
                if (el->def == NULL) {
                    gsymlist->array[gsymlist->pos - 1].def = def;
                } else {
                    gsymlist->array[gsymlist->pos - 1].def =
                        cnfexpr_or(gsymlist,
                                   gsymlist->array[gsymlist->pos - 1].def,
                                   def);
                }
            }
            if (el->def == NULL)
                el->cnfexpr_false(gsymlist);
            if (sym->dir_dep.expr != NULL)
                el->dep = kconfig_cnfexpr(gsymlist, sym->dir_dep.expr);
            else
                el->dep = cnfexpr_true(gsymlist);
            if (sym->rev_dep.expr != NULL)
                el->re_be = kconfig_cnfexpr(gsymlist, sym->rev_dep.expr);
            else
                el->rev_dep = cnfexpr_false(gsymlist);

            if (el->prompt) {
                struct cnfexpr *pw =
                    cnfexpr_and(gsymlist,
                                cnfexpr_or(gsymlist,
                                           cnfexpr_not(gsymlist,
                                                       cnfexpr_sym
                                                       (gsymlist,
                                                        sym->name)),
                                           el->dep), cnfexpr_or(gsymlist,
                                                                cnfexpr_not
                                                                (gsymlist,
                                                                 el->
                                                                 rev_dep),
                                                                cnfexpr_sym
                                                                (gsymlist,
                                                                 sym->
                                                                 name)));
                switch (pw->type) {
                    case CT_EXPR:
                        break;
                    case CT_TRUE:
                        break;
                    case CT_FALSE:
                        break;
                }
            } else {
            }
        }
    }
}
