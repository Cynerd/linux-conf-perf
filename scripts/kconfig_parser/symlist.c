#include "symlist.h"

struct symlist *symlist_create() {
    struct symlist *ret;
    ret = malloc(sizeof(struct symlist));
    ret->size = 2;
    ret->pos = 0;
    ret->array = malloc(ret->size * sizeof(struct symlist_el));
    return ret;
}

void symlist_add(struct symlist *sl, char *name) {
    if (sl->pos >= sl->size) {
        sl->size *= 2;
        sl->array =
            realloc(sl->array, sl->size * sizeof(struct symlist_el));
    }
    sl->array[sl->pos].id = sl->pos + 1;
    sl->array[sl->pos].name = name;
    sl->array[sl->pos].be = NULL;
    sl->pos++;
}

struct symlist_el *symlist_find(struct symlist *sl, char *name) {
    int i = 0;
    while (i < sl->pos) {
        if (!strcmp(name, sl->array[i].name))
            return &sl->array[i];
        i++;
    }
    return NULL;
}

void symlist_print(struct symlist *sl) {
    int i;
    for (i = 0; i < sl->pos; i++) {
        printf("%d:%s\n", sl->array[i].id, sl->array[i].name);
        if (sl->array[i].be != NULL) {
            printf("  ");
            cnf_printf(sl->array[i].be);
            printf("\n");
        }
    }
}

void symlist_free(struct symlist *sl) {
    free(sl->array);
    free(sl);
}
