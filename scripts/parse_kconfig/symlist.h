#include <stdbool.h>
#include <string.h>
#include "cnfexpr.h"

#ifndef _SYMLIST_H_
#define _SYMLIST_H_

struct symlist_el {
    unsigned int id;
    char *name;
    bool prompt;
    struct cnfexpr *be;
    struct cnfexpr *re_be; // forward dependency
};
struct symlist {
    struct symlist_el *array;
    size_t size, pos;
};

struct symlist *symlist_create();
void symlist_add(struct symlist *sl, char *name);
void symlist_set_prompt(struct symlist *sl, char *name, bool prompt);
struct symlist_el *symlist_find(struct symlist *sl, char *name);
void symlist_print(struct symlist *sl);
void symlist_free(struct symlist *sl);

#endif /* _SYMLIST_H_ */
