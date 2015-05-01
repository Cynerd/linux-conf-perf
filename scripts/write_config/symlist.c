#include "symlist.h"

#define NONAMEGEN "NONAMEGEN"

struct symlist *symlist_read(FILE * f) {
    struct symlist *ret;
    ret = malloc(sizeof(struct symlist));
    ret->size = 1;
    ret->maxid = 0;
    ret->array = malloc(ret->size * sizeof(struct symlist_el));

    unsigned int id;
    char *w;
    size_t w_pos = 0, w_size = 2;
    w = malloc((w_size + 1) * sizeof(char));

    int c;
    do {
        c = fgetc(f);
        if (c == '\n') {
            w[w_pos] = '\0';
            if ((size_t) id > ret->size) {
                ret->size *= 2;
                ret->array =
                    realloc(ret->array,
                            ret->size * sizeof(struct symlist_el));
            }
            if (id > ret->maxid)
                ret->maxid = id;
            ret->array[(size_t) id - 1].id = id;
            if (!strncmp(w, NONAMEGEN, strlen(NONAMEGEN)))
                ret->array[(size_t) id - 1].sym = NULL;
            else
                ret->array[(size_t) id - 1].sym = sym_lookup(w, 0);
            w_pos = 0;
        } else if (c == ':') {
            w[w_pos] = '\0';
            id = atoi(w);
            w_pos = 0;
        } else {
            if (w_pos >= w_size) {
                w_size *= 2;
                w = realloc(w, (w_size + 1) * sizeof(char));
            }
            w[w_pos++] = (char) c;
        }
    } while (c != EOF);

    return ret;
}

struct symbol *symlist_get(struct symlist *sl, unsigned int id) {
    return sl->array[id].sym;
}
