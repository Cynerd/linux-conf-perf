#ifndef _CNFEXPR_H_
#define _CNFEXPR_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "symlist.h"
#include <kconfig/lkc.h>

enum cnfexpr_type {
    CT_EXPR, CT_FALSE, CT_TRUE
};

struct cnfexpr {
    enum cnfexpr_type type;
    int **exprs;
    unsigned *sizes;
    unsigned size;
};

struct cnfexpr *kconfig_cnfexpr(struct symlist *sl, bool nt, bool def,
                                struct symbol *sym, struct expr *expr);
void cnf_printf(struct cnfexpr *);

struct boolexp *printf_original(struct symlist *sl, struct expr *expr);

#endif /* _CNFEXPR_H_ */
