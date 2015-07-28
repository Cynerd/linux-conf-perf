#include "inv.h"

void inv_prepare(char *input_file) {
    FILE *f;
    f = fopen(input_file, "r");
    if (f == NULL) {
        Eprintf("Can't open input file: %s\n", input_file);
        exit(-2);
    }

    struct property *fixed_prop;
    fixed_prop = malloc(sizeof(struct property));
    fixed_prop->type = P_UNKNOWN;
    fixed_prop->lineno = LINENUM_IDENTIFICATOR;
    fixed_prop->next = NULL;
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
            if (sym->prop == NULL) {
                sym->prop = fixed_prop;
                continue;
            }
            struct property *prop;
            prop = sym->prop;
            while (prop->next != NULL)
                prop = prop->next;
            prop->next = fixed_prop;
        }
    }

    fclose(f);
}

bool inv_fixed(struct symbol *sym) {
    if (sym->prop == NULL)
        return false;
    struct property *prop;
    prop = sym->prop;
    while (prop->next != NULL)
        prop = prop->next;
    if (prop->type == P_UNKNOWN && prop->lineno == LINENUM_IDENTIFICATOR)
        return true;
    else
        return false;
}
