#include "solution.h"

void solution_check(struct symlist *sl, FILE * f) {
    int c;
    // skip first line
    do
        c = fgetc(f);
    while (c != EOF && c != '\n');

    char *w;
    size_t w_size = 1, w_pos = 0;
    w = malloc((w_size + 1) * sizeof(char));
    do {
        c = fgetc(f);
        if (c == ' ' || c == '\n') {
            w[w_pos] = '\0';
            w_pos = 0;
            char *ww = w;
            bool neg = false;
            if (w[0] == '-') {
                neg = true;
                ww = w + 1;
            }
            int id = atoi(ww);
            if ((unsigned) id > sl->maxid)
                break;
            if (id == 0)
                continue;
            if (sl->array[id - 1].sym == NULL)
                continue;
            //tristate val = sym_get_tristate_value(sl->array[id - 1].sym);
            //sym_set_tristate_value(sl->array[id - 1].sym, neg ? no : yes);
            //sym_calc_value(sl->array[id - 1].sym);
            if (neg ==
                (sym_get_tristate_value(sl->array[id - 1].sym) ==
                 no ? true : false))
                //printf("Ok\n");
            {
            } else
                printf("Problem %s=%d/%d\n", sl->array[id - 1].sym->name,
                       !neg,
                       sym_get_tristate_value(sl->array[id - 1].sym));
            //if (sym_get_tristate_value(sl->array[id - 1].sym) != val)
            //printf("Change: %s\n", sl->array[id - 1].sym->name);
        } else {
            if (w_pos >= w_size) {
                w_size *= 2;
                w = realloc(w, (w_size + 1) * sizeof(char));
            }
            w[w_pos++] = (char) c;
        }
    } while (c != EOF && c != '\n');
}
