#include <stdlib.h>
#include <stdbool.h>

#include "symlist.h"
#include <kconfig/lkc.h>
#include "output.h"

#ifndef _CNFBUILD_H_
#define _CNFBUILD_H_

enum cnfexpr_type {
    CT_EXPR, CT_FALSE, CT_TRUE
};

struct cnfexpr {
    enum cnfexpr_type type;
    unsigned id;
    struct output_expr *out;
};

struct cnfexpr *kconfig_expr(struct symlist *sl, struct expr *expr);

struct cnfexpr *cnfexpr_true(struct symlist *sl);
struct cnfexpr *cnfexpr_false(struct symlist *sl);
struct cnfexpr *cnfexpr_sym(struct symlist *sl, struct symbol *sym);
struct cnfexpr *cnfexpr_eql(struct symlist *sl, struct symbol *sym1,
                        struct symbol *sym2);
struct cnfexpr *cnfexpr_or(struct symlist *sl, struct cnfexpr *e1,
                       struct cnfexpr *e2);
struct cnfexpr *cnfexpr_and(struct symlist *sl, struct cnfexpr *e1,
                        struct cnfexpr *e2);
struct cnfexpr *cnfexpr_not(struct symlist *sl, struct cnfexpr *e);
struct cnfexpr *cnfexpr_copy(struct cnfexpr *e);
void cnfexpr_free(struct cnfexpr *e);

#endif /* _CNFBUILD_H_ */
