#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <search.h>

#include "cnfbuild.h"
#include "output.h"

#ifndef _SYMLIST_H_
#define _SYMLIST_H_

struct symlist_el {
    char *name;
    bool prompt;
    struct cnfexpr *def;
    size_t def_size;

    struct cnfexpr *dep, *rev_dep;
};

struct symlist {
    struct symlist_el *array;
    size_t size, pos;
    unsigned lastsym;
};

struct symlist *symlist_create();
void symlist_add(struct symlist *sl, char *name);
void symlist_closesym(struct symlist *sl);
unsigned symlist_adddummy(struct symlist *sl);
struct symlist_el *symlist_find(struct symlist *sl, char *name);
size_t symlist_id(struct symlist *sl, char *name);
void symlist_print(struct symlist *sl);
void symlist_free(struct symlist *sl);

#endif /* _SYMLIST_H_ */
