#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <stdbool.h>
#include <libintl.h>

#include <kconfig/lkc.h>
#include "boolexpr.h"
#include "symlist.h"
#include "output.h"
#include "macros.h"
#include "doutput.h"

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

    char *rules_file, *symbol_map_file, *variable_count_file;
    asprintf(&rules_file, "%s/%s", folder, DEFAULT_RULES_FILE);
    asprintf(&symbol_map_file, "%s/%s", folder, DEFAULT_SYMBOL_MAP_FILE);
    asprintf(&variable_count_file, "%s/%s", folder,
             DEFAULT_VARIABLE_COUNT_FILE);
    output_init(rules_file, symbol_map_file);

    build_symlist();
    cpy_dep();

    output_write_variable_count(variable_count_file,
                                gsymlist->lastsym - 1);

    output_finish();
    return 0;
}

void build_symlist() {
    int i;
    struct symbol *sym;
    struct property *prop;
    for_all_symbols(i, sym) {
        if (sym->type == S_BOOLEAN || sym->type == S_TRISTATE) {
            if (sym->name == NULL) {
                sym->name = malloc((9 + 7) * sizeof(char));
                sprintf(sym->name, "NONAMEGEN%d", noname_num++);
            }
            symlist_add(gsymlist, sym->name);
        }
        for_all_prompts(sym, prop) {
            gsymlist->array[gsymlist->pos - 1].prompt = true;
            break;
        }
    }
    symlist_closesym(gsymlist);
}

void cpy_dep() {
    int i;
    struct symbol *sym;
    struct property *prop;
    struct symlist_el *el;
    unsigned el_id;
    for_all_symbols(i, sym) {
        if ((sym->type == S_BOOLEAN || sym->type == S_TRISTATE)) {
            el_id = symlist_id(gsymlist, sym->name);
            el = &(gsymlist->array[el_id - 1]);
            Iprintf("Processing: %s\n", sym->name);
            if (el->prompt)
                Dprintf("Is prompt\n");

            for_all_defaults(sym, prop) {
                Dprintf(" Default value:\n");
                doutput_expr(prop->expr);
                struct boolexpr *def =
                    boolexpr_kconfig(gsymlist, prop->expr);
                if (el->def == NULL) {
                    el->def = def;
                } else {
                    el->def = boolexpr_or(el->def, def);
                }
            }
            if (el->def == NULL)
                el->def = boolexpr_false();
            if (sym->dir_dep.expr != NULL) {
                Dprintf(" Dependency:\n");
                doutput_expr(sym->dir_dep.expr);
                el->dep = boolexpr_kconfig(gsymlist, sym->dir_dep.expr);
            } else
                el->dep = boolexpr_true();
            if (sym->rev_dep.expr != NULL) {
                Dprintf(" Reverse dependency:\n");
                doutput_expr(sym->rev_dep.expr);
                el->rev_dep =
                    boolexpr_kconfig(gsymlist, sym->rev_dep.expr);
            } else
                el->rev_dep = boolexpr_false();

            struct boolexpr *pw;
            struct boolexpr *boolsym = boolexpr_sym(gsymlist, sym);
            boolexpr_copy(boolsym);
            struct boolexpr *boolsym_not = boolexpr_not(boolsym);
            if (!el->prompt) {
                boolexpr_copy(boolsym);
                boolexpr_copy(boolsym_not);
                boolexpr_copy(el->def);
                boolexpr_copy(el->dep);
                boolexpr_copy(el->rev_dep);
            }
            // (!sym || dep) && (sym || !rev_dep)
            struct boolexpr *w1 = boolexpr_or(boolsym_not, el->dep);
            struct boolexpr *w2 =
                boolexpr_or(boolsym, boolexpr_not(el->rev_dep));
            pw = boolexpr_and(w1, w2);

            if (!el->prompt) {
                // && (sym || !dep || !def) &&
                // (!sym || rev_dep || def)
                struct boolexpr *w31 =
                    boolexpr_or(boolsym, boolexpr_not(el->dep));
                struct boolexpr *w3 =
                    boolexpr_or(w31, boolexpr_not(el->def));
                struct boolexpr *w41 =
                    boolexpr_or(boolsym_not, el->rev_dep);
                struct boolexpr *w4 = boolexpr_or(w41, el->def);
                pw = boolexpr_and(pw, w3);
                pw = boolexpr_and(pw, w4);
            }
            Dprintf(" CNF:\n");
            doutput_boolexpr(pw, gsymlist);
            cnf_boolexpr(gsymlist, pw);
            switch (pw->type) {
            case BT_TRUE:
                break;
            case BT_FALSE:
                Eprintf
                    ("E: Root terms is false. This shouldn't happen.\n");
                break;
            default:
                output_rules_symbol((int) pw->id);
                output_rules_endterm();
            }
            boolexpr_free(pw);
        }
    }
}
