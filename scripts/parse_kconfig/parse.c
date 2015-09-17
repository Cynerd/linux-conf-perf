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
                                gsymlist->lastsym - 1, gsymlist->pos);

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
    }
    symlist_closesym(gsymlist);
}

void cpy_dep() {
    int i;
    struct symbol *sym;
    struct property *prop;
    struct symlist_el *el;
    unsigned el_id;
    struct boolexpr *boolsym;
    for_all_symbols(i, sym) {
        if (sym->type == S_BOOLEAN || sym->type == S_TRISTATE) {
            el_id = symlist_id(gsymlist, sym->name);
            el = &(gsymlist->array[el_id - 1]);
            boolsym = boolexpr_sym(gsymlist, sym, false, NULL);
            Iprintf("Processing: %s\n", sym->name);
            // Visibility
            for_all_prompts(sym, prop) {
                Dprintf(" Prompt: %s\n", prop->text);
                if (prop->visible.expr != NULL) {
                    doutput_expr(prop->visible.expr);
                    struct boolexpr *vis =
                        boolexpr_kconfig(gsymlist, prop->visible.expr,
                                         false, NULL);
                    if (el->vis == NULL) {
                        el->vis = vis;
                    } else {
                        el->vis = boolexpr_or(el->vis, vis);
                    }
                } else if (el->vis == NULL) {
                    el->vis = boolexpr_true();
                }
            }
            if (el->vis == NULL)
                el->vis = boolexpr_false();
            // Symbol is choice.. special treatment required
            if (sym_is_choice(sym)) {
                Dprintf(" Is Choice\n");
                goto choice_exception;
            }
            // Default value
            struct boolexpr **defexpr = NULL;
            size_t defexpr_size = 0;
            int z;
            bool exitdef = false;
            for_all_defaults(sym, prop) {
                Dprintf(" Default value:\n");
                doutput_expr(prop->expr);
                struct boolexpr *def =
                    boolexpr_kconfig(gsymlist, prop->expr, true, NULL);
                struct boolexpr *vis;
                if (prop->visible.expr != NULL)
                    vis = boolexpr_kconfig(gsymlist, prop->visible.expr,
                            false, NULL);
                else
                    vis = boolexpr_true();
                if (vis->type != BT_TRUE) {
                    defexpr = realloc(defexpr,
                            ++defexpr_size * sizeof(struct boolexpr *));
                    defexpr[defexpr_size - 1] = boolexpr_copy(vis);
                } else {
                    ++defexpr_size;
                    exitdef = true;
                }
                def = boolexpr_and(def, vis);
                for (z = 0; z < ((int)defexpr_size - 1); z++) {
                    def = boolexpr_and(def, boolexpr_not(
                                boolexpr_copy(defexpr[z])));
                }
                if (el->def == NULL)
                    el->def = def;
                else
                    el->def = boolexpr_or(el->def, def);
                if (exitdef)
                    break;
            }
            if (defexpr != NULL) {
                for (z = 0; z < defexpr_size - 1; z++) {
                    boolexpr_free(defexpr[z]);
                }
                free(defexpr);
            }
            if (el->def == NULL)
                el->def = boolexpr_false();
            // Dependency expression
            if (sym->dir_dep.expr != NULL) {
                Dprintf(" Dependency:\n");
                doutput_expr(sym->dir_dep.expr);
                el->dep =
                    boolexpr_kconfig(gsymlist, sym->dir_dep.expr, false, NULL);
            } else
                el->dep = boolexpr_true();
            // Reverse dependency expression
            if (sym->rev_dep.expr != NULL) {
                Dprintf(" Reverse dependency:\n");
                doutput_expr(sym->rev_dep.expr);
                el->rev_dep =
                    boolexpr_kconfig(gsymlist, sym->rev_dep.expr, false, NULL);
            } else
                el->rev_dep = boolexpr_false();

            if (el->dep->type != BT_FALSE && el->dep->type != BT_TRUE)
                cnf_boolexpr(gsymlist, el->dep);
            if (el->rev_dep->type != BT_FALSE
                && el->rev_dep->type != BT_TRUE)
                cnf_boolexpr(gsymlist, el->rev_dep);
            if (el->def->type != BT_FALSE && el->def->type != BT_TRUE)
                cnf_boolexpr(gsymlist, el->def);
            if (el->vis->type != BT_FALSE && el->vis->type != BT_TRUE)
                cnf_boolexpr(gsymlist, el->vis);
            // (!sym || dep) && (sym || !rev_dep) &&
            // && (sym || !dep || !def || vis) &&
            // (!sym || rev_dep || def || vis)
            if (el->dep->type != BT_TRUE) {
                output_rules_symbol(-1 * boolsym->id);
                if (el->dep->type != BT_FALSE) {
                    output_rules_symbol(el->dep->id);
                }
                output_rules_endterm();
            }
            if (el->rev_dep->type != BT_FALSE) {
                output_rules_symbol(boolsym->id);
                if (el->rev_dep->type != BT_TRUE) {
                    output_rules_symbol(-1 * el->rev_dep->id);
                }
                output_rules_endterm();
            }
            if (el->dep->type != BT_FALSE && el->def->type != BT_FALSE
                && el->vis->type != BT_TRUE) {
                output_rules_symbol(boolsym->id);
                if (el->dep->type != BT_TRUE) {
                    output_rules_symbol(-1 * el->dep->id);
                }
                if (el->def->type != BT_TRUE) {
                    output_rules_symbol(-1 * el->def->id);
                }
                if (el->vis->type != BT_FALSE) {
                    output_rules_symbol(el->vis->id);
                }
                output_rules_endterm();
            }
            if (el->rev_dep->type != BT_TRUE && el->def->type != BT_TRUE
                && el->vis->type != BT_TRUE) {
                output_rules_symbol(-1 * boolsym->id);
                if (el->rev_dep->type != BT_FALSE) {
                    output_rules_symbol(el->rev_dep->id);
                }
                if (el->def->type != BT_FALSE) {
                    output_rules_symbol(el->def->id);
                }
                if (el->vis->type != BT_FALSE) {
                    output_rules_symbol(el->vis->id);
                }
                output_rules_endterm();
            }

            boolexpr_free(el->def);
            boolexpr_free(el->vis);
            boolexpr_free(el->dep);
            boolexpr_free(el->rev_dep);

            continue;

          choice_exception:
            // Add exclusive rules for choice symbol
            if (sym->rev_dep.expr != NULL) {
                Dprintf(" Dependency:\n");
                doutput_expr(sym->rev_dep.expr);
                el->rev_dep =
                    boolexpr_kconfig(gsymlist, sym->rev_dep.expr, true, NULL);
            } else
                el->rev_dep = boolexpr_true();
            for_all_choices(sym, prop) {
                struct symbol *symw;
                struct expr *exprw;
                unsigned *symx = NULL;
                size_t symx_size = 0;
                int x, y;
                expr_list_for_each_sym(prop->expr, exprw, symw) {
                    symx_size++;
                    symx = realloc(symx, symx_size * sizeof(unsigned));
                    symx[symx_size - 1] = symlist_id(gsymlist, symw->name);
                    output_rules_symbol(symx[symx_size - 1]);
                }
                output_rules_symbol(-(int)
                                    el_id);
                output_rules_endterm();
                for (x = 0; x < symx_size - 1; x++) {
                    for (y = x + 1; y < symx_size; y++) {
                        output_rules_symbol(-(int)
                                            symx[x]);
                        output_rules_symbol(-(int)
                                            symx[y]);
                        output_rules_endterm();
                    }
                }
                free(symx);
                symx = NULL;
            }
            if (el->rev_dep->type != BT_FALSE && el->rev_dep->type != BT_TRUE)
                cnf_boolexpr(gsymlist, el->rev_dep);
            if (el->vis->type != BT_FALSE && el->vis->type != BT_TRUE)
                cnf_boolexpr(gsymlist, el->vis);
            // (!sym || rev_dep) && (!sym || !rev_dep || vis)
            // For nonoptional per list symbol add:
            // (sym || !rev_dep || !vis || !dir_dep_of_list))
            if (el->rev_dep->type != BT_TRUE) {
                output_rules_symbol(-1 * boolsym->id);
                if (el->rev_dep->type != BT_FALSE) {
                    output_rules_symbol(el->rev_dep->id);
                }
                output_rules_endterm();
            }
            if (el->rev_dep->type != BT_FALSE && el->vis->type != BT_TRUE) {
                output_rules_symbol(-1 * boolsym->id);
                if (el->rev_dep->type != BT_TRUE) {
                    output_rules_symbol(-1 * el->rev_dep->id);
                }
                if (el->vis != BT_FALSE) {
                    output_rules_symbol(el->vis->id);
                }
                output_rules_endterm();
            }
            if (!sym_is_optional(sym)) {
                for_all_choices(sym, prop) {
                    struct symbol *symw;
                    struct expr *exprw;
                    expr_list_for_each_sym(prop->expr, exprw, symw) {
                        struct boolexpr *wdep;
                        if (symw->dir_dep.expr != NULL) {
                            struct symbol *settrue[] = {sym, NULL};
                            wdep =
                                boolexpr_kconfig(gsymlist, symw->dir_dep.expr,
                                        false, settrue);
                        } else
                            wdep = boolexpr_true();
                        cnf_boolexpr(gsymlist, wdep);
                        if (el->rev_dep->type != BT_FALSE
                            && el->vis->type != BT_FALSE
                            && wdep->type != BT_FALSE) {
                            output_rules_symbol(boolsym->id);
                            if (el->rev_dep->type != BT_TRUE) {
                                output_rules_symbol(-1 * el->rev_dep->id);
                            }
                            if (el->vis->type != BT_TRUE) {
                                output_rules_symbol(-1 * el->vis->id);
                            }
                            if (wdep->type != BT_TRUE
                                && wdep->id != boolsym->id) {
                                output_rules_symbol(-1 * wdep->id);
                            }
                            output_rules_endterm();
                        }
                    }
                }
            }
        }
    }
}
