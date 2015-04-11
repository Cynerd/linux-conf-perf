#include "output.h"

void fprint_rules_cnf(FILE * f, unsigned id, struct cnfexpr *cnf, bool nt) {
    unsigned i, y;
    switch (cnf->type) {
    case CT_FALSE:
        // Never satisfiable
        if (!nt)
            fprintf(f, "-");
        fprintf(f, "%d\n", id);
        break;
    case CT_TRUE:
        // Always satisfiable
        break;
    case CT_EXPR:
        for (i = 0; i < cnf->size; i++) {
            if (!nt)
                fprintf(f, "-");
            fprintf(f, "%d ", id);
            for (y = 0; y < cnf->sizes[i] - 1; y++) {
                fprintf(f, "%d ", cnf->exprs[i][y]);
            }
            fprintf(f, "%d ", cnf->exprs[i][cnf->sizes[i] - 1]);
            fprintf(f, "\n");
        }
        break;
    }
}

void fprint_rules(struct symlist *sl, char *output) {
    FILE *f;
    f = fopen(output, "w");
    if (f == NULL) {
        fprintf(stderr, "Can't create file: %s\n", output);
        return;
    }
    size_t i;
    struct symlist_el *el;
    for (i = 0; i < sl->pos; i++) {
        if (sl->array[i].be != NULL) {
            el = sl->array + i;
            if (el->be != NULL) {
                fprint_rules_cnf(f, el->id, el->be, false);
            }
            if (el->re_be != NULL) {
                fprint_rules_cnf(f, el->id, el->re_be, true);
            }
        }
    }
    fclose(f);
}

void fprint_symbol_map(struct symlist *sl, char *output) {
    FILE *f;
    f = fopen(output, "w");
    if (f == NULL) {
        fprintf(stderr, "Can't create file: %s\n", output);
        return;
    }
    size_t i;
    for (i = 0; i < sl->pos; i++) {
        fprintf(f, "%d:%s\n", sl->array[i].id, sl->array[i].name);
    }
    fclose(f);
}
