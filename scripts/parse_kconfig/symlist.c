#include "symlist.h"

struct symlist *symlist_create() {
    struct symlist *ret;
    ret = malloc(sizeof(struct symlist));
    ret->size = 1;
    ret->pos = 0;
    ret->lastsym = 0;
    ret->array = malloc(ret->size * sizeof(struct symlist_el));
    return ret;
}

void symlist_add(struct symlist *sl, char *name) {
    if (sl->pos >= sl->size) {
        sl->size *= 2;
        sl->array =
            realloc(sl->array, sl->size * sizeof(struct symlist_el));
    }
    sl->array[sl->pos].name = name;
    sl->array[sl->pos].prompt = false;
    sl->array[sl->pos].def = NULL;
    sl->array[sl->pos].def_size = 0;
    sl->array[sl->pos].dep = NULL;
    sl->array[sl->pos].rev_dep = NULL;
    sl->pos++;
}

void symlist_closesym(struct symlist *sl) {
    sl->lastsym = (unsigned) sl->pos;
}

unsigned symlist_adddummy(struct symlist *sl) {
    if (sl->lastsym == 0)
        fprintf(stderr,
                "W: symlist adddummy, but lastsym is zero. This shouldn't happen.");
    return sl->lastsym++;
}

struct symlist_el *symlist_find(struct symlist *sl, char *name) {
    size_t i = symlist_id(sl, name);
    if (i == 0)
        return NULL;
    else
        return &(sl->array[i - 1]);
}

// TODO faster implementation? Maybe binary search tree?
size_t symlist_id(struct symlist * sl, char *name) {
    size_t i = 0;
    while (i < sl->pos) {
        if (!strcmp(name, sl->array[i].name))
            return i + 1;
        i++;
    }
    return 0;
}

void symlist_print(struct symlist *sl) {
    /*
       size_t i;
       for (i = 0; i < sl->pos; i++) {
       printf("%d:%s\n", sl->array[i].id, sl->array[i].name);
       if (sl->array[i].be != NULL) {
       printf("  ");
       cnf_printf(sl->array[i].be);
       printf("\n");
       }
       }
     */
}

void symlist_free(struct symlist *sl) {
    free(sl->array);
    free(sl);
}
