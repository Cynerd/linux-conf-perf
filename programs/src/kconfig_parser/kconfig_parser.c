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
#include "boolexp.h"
#include "output.h"

void kconfig_menu_walker(void (*solve)
                          (struct symbol * sym));

void solve_names(struct symbol *sym);
void solve_dep(struct symbol *sym);

struct symlist *gsymlist;
int noname_num;

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

    kconfig_menu_walker(solve_names);
    kconfig_menu_walker(solve_dep);

    char *rules_file, *symbol_map_file;
    asprintf(&rules_file, "%s/%s", argv[2], DEFAULT_RULES_FILE);
    asprintf(&symbol_map_file, "%s/%s", argv[2], DEFAULT_SYMBOL_MAP_FILE);
    fprint_rules(gsymlist, rules_file);
    fprint_symbol_map(gsymlist, symbol_map_file);
    return 0;
}

void kconfig_menu_walker(void (*solve) (struct symbol * sym)) {
    struct menu *menu;
    struct symbol *sym;
    menu = rootmenu.list;

    while (menu != NULL) {
        sym = menu->sym;
        if (sym != NULL) {
            do {
                if (sym->type == S_BOOLEAN || sym->type == S_TRISTATE) {
                    solve(sym);
                }
            } while ((sym = sym->next) != NULL);
        }
        // switch to menu
        if (menu->list != NULL) {
            menu = menu->list;
        } else if (menu->next != NULL) {
            menu = menu->next;
        } else {
            while ((menu = menu->parent) != NULL) {
                if (menu->next != NULL) {
                    menu = menu->next;
                    break;
                }
            }
        }
    }
}

void solve_names(struct symbol *sym) {
    if (sym->name != NULL) {
        if (symlist_find(gsymlist, sym->name) == NULL)
            symlist_add(gsymlist, sym->name);
    } else {
        sym->name = malloc((9 + 7) * sizeof(char));
        sprintf(sym->name, "NONAMEGEN%d", noname_num++);
        symlist_add(gsymlist, sym->name);
    }
}

void solve_dep(struct symbol *sym) {
    if (sym->dir_dep.expr != NULL) {
        struct symlist_el *el;
        el = symlist_find(gsymlist, sym->name);
        el->be = copy_kconfig_dep(gsymlist, sym->dir_dep.expr);
        if (el->be != NULL)
            el->be = boolexp_cnf(el->be);
    }
}
