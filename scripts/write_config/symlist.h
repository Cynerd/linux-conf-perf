#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <kconfig/lkc.h>

#ifndef _SYMLIST_H_
#define _SYMLIST_H_

struct symlist_el {
    unsigned int id;
    struct symbol *sym;
};

struct symlist {
    struct symlist_el *array;
    size_t size;
};

struct symlist *symlist_read(FILE *f);
struct symbol *symlist_get(struct symlist *, unsigned int id);

#endif /* _SYMLIST_H_ */
