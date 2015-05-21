#include "dotconf.h"

void dotconfig_read(bool * reqsave) {
    FILE *f;
    f = fopen(DOTCONFIG_FILE, "r");
    if (f == NULL) {
        *reqsave = true;
        return;
    }

    char buffer[READBUFFER_SIZE];
    while (fgets(buffer, READBUFFER_SIZE, f) != NULL) {
        if (buffer[0] == '\0' || buffer[1] == '\0')
            continue;
        if (buffer[0] != '#') {
            char *wstr = buffer + 7;
            char *end = strchr(wstr, '=');
            *end = '\0';
            struct symbol *sym = sym_find(wstr);
            if (sym->type != S_BOOLEAN && sym->type != S_TRISTATE)
                continue;
            if ((sym_get_tristate_value(sym) == yes && *(end + 1) != 'y')
                || (sym_get_tristate_value(sym) == no
                    && *(end + 1) != 'n'))
                *reqsave = true;
            struct property *prop;
            for_all_prompts(sym, prop) {
                if (prop->menu->data == NULL)
                    prop->menu->data = menudata_new();
            }
        }
    }

    fclose(f);

    struct menu *wmenu;
    struct menu **stack;
    size_t stack_size = 2, stack_pos = 0;
    stack = malloc(stack_size * sizeof(struct menu *));
    wmenu = rootmenu.list;
    while (wmenu != NULL) {
        if (wmenu->list != NULL) {
            if (stack_pos >= stack_size) {
                stack_size *= 2;
                stack = realloc(stack, stack_size * sizeof(struct menu *));
            }
            stack[stack_pos++] = wmenu->list;
        }
        if (wmenu->data == NULL) {
            if (wmenu->sym == NULL || wmenu->sym->name == NULL) {
                wmenu->data = menudata_new();
            } else {
                wmenu->data = menudata_new();
                menudata_set_permute(wmenu, true);
            }
        }
        wmenu = wmenu->next;
        if (wmenu == NULL && stack_pos > 0)
            wmenu = stack[--stack_pos];
    }
    while (wmenu != NULL) {
        if (wmenu->list != NULL)
            stack[stack_pos++] = wmenu->list;
        if (wmenu->data == NULL) {
            if (wmenu->sym == NULL || wmenu->sym->name == NULL) {
                wmenu->data = menudata_new();
            } else {
                wmenu->data = menudata_new();
                menudata_set_permute(wmenu, true);
            }
        }
        wmenu = wmenu->next;
        if (wmenu == NULL && stack_pos > 0)
            wmenu = stack[--stack_pos];
    }
}

void dotconfig_write(void) {
    FILE *f;
    f = fopen(DOTCONFIG_FILE, "w");

    struct symbol *sym;
    struct property *prop;
    int i;
    unsigned variable = 0, fixed = 0;
    for_all_symbols(i, sym)
        if ((sym->type == S_BOOLEAN || sym->type == S_TRISTATE)
            && sym->name != NULL) {
        for_all_prompts(sym, prop) {
            if (prop->menu->data == NULL
                || !((struct menudata *) prop->menu->data)->permute) {
                fprintf(f, "CONFIG_%s=%s\n", sym->name,
                        sym_get_tristate_value(sym) == no ? "n" : "y");
                fixed++;
                break;
            } else {
                variable++;
                if (verbose_level > 1)
                    printf("%s=%s\n", sym->name,
                           sym_get_tristate_value(sym) == no ? "n" : "y");
            }
        }
    }

    printf("Variable: %d, Fixed: %d\n", variable, fixed);

    fclose(f);
}
