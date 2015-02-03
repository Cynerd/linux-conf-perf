#ifndef _BOOLEXP_H_
#define _BOOLEXP_H_

#include <stdlib.h>
#include <stdbool.h>
#include "symlist.h"
#include "kconfig/lkc.h"

enum boolexp_type {
    BE_OR, BE_AND, BE_NOT, BE_LEAF
};

struct boolexp;
union boolexp_data {
    struct boolexp *be;
    unsigned int id;
};

struct boolexp {
    enum boolexp_type type;
    union boolexp_data left, right;
};

struct boolexp *copy_kconfig_dep(struct symlist *sl, struct expr *expr);
struct boolexp *boolexp_cnf(struct boolexp *be);
void boolexp_print(struct boolexp *be);

#endif /* _BOOLEXP_H_ */
