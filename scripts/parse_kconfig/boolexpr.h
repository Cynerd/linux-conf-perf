#include <stdlib.h>
#include <stdbool.h>

#include <kconfig/lkc.h>

#ifndef _BOOLEXPR_H_
#define _BOOLEXPR_H_

#include "symlist.h"

enum boolexpr_type {
    BT_SYM, BT_TRUE, BT_FALSE, BT_OR, BT_AND, BT_NOT
};

struct boolexpr {
    enum boolexpr_type type;
    unsigned id;
    struct boolexpr *left, *right;

    unsigned overusage;
};

struct boolexpr *boolexpr_kconfig(struct symlist *sl, struct expr *expr,
                                  bool modulesym);

struct boolexpr *boolexpr_true();
struct boolexpr *boolexpr_false();
struct boolexpr *boolexpr_sym(struct symlist *sl, struct symbol *sym,
                              bool modulesym);
struct boolexpr *boolexpr_or(struct boolexpr *e1, struct boolexpr *e2);
struct boolexpr *boolexpr_and(struct boolexpr *e1, struct boolexpr *e2);
struct boolexpr *boolexpr_not(struct boolexpr *e);
struct boolexpr *boolexpr_copy(struct boolexpr *e);
void boolexpr_free(struct boolexpr *e);

#endif /* _BOOLEXPR_H_ */
