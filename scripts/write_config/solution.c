#include "solution.h"

extern int exit_status;
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
            if (neg ==
                (sym_get_tristate_value(sl->array[id - 1].sym) ==
                 no ? true : false))
            {
            } else {
                printf("Problem %s=%d/%d\n", sl->array[id - 1].sym->name,
                       !neg,
                       sym_get_tristate_value(sl->array[id - 1].sym));
                exit_status++;
            }
        } else {
            if (w_pos >= w_size) {
                w_size *= 2;
                w = realloc(w, (w_size + 1) * sizeof(char));
            }
            w[w_pos++] = (char) c;
        }
    } while (c != EOF && c != '\n');
}
