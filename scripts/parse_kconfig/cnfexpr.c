#include "cnfexpr.h"

struct cnfexpr *cnf_sym(struct symlist *sl, bool not, struct symbol *sym);
struct cnfexpr *cnf_eql(struct symlist *sl, bool not, struct symbol *sym1,
                        struct symbol *sym2);
struct cnfexpr *cnf_or(struct cnfexpr *e1, struct cnfexpr *e2);
struct cnfexpr *cnf_and(struct cnfexpr *e1, struct cnfexpr *e2);
struct cnfexpr *cnf_not(struct cnfexpr *el);
struct cnfexpr *cnf_copy(struct cnfexpr *el);
void free_cnf(struct cnfexpr *e);

struct cnfexpr *kconfig_cnfexpr(struct symlist *sl, bool nt,
                                struct symbol *sym, struct expr *expr) {
    struct stck {
        struct expr *expr;
        struct cnfexpr *cnf;
    };
    struct expr **back;
    int back_size = 2, back_pos = -1;
    back = malloc((unsigned) back_size * sizeof(struct expr *));
    struct stck *stack;
    int stack_size = 2, stack_pos = -1;
    stack = malloc((unsigned) stack_size * sizeof(struct stck));
    struct cnfexpr *rtrn = NULL;

    while (expr != NULL) {
        if ((back_pos >= 0 && back[back_pos] != expr) || back_pos < 0) {
            if (++back_pos == back_size) {
                back_size *= 2;
                back =
                    realloc(back,
                            (unsigned) back_size * sizeof(struct expr *));
            }
            back[back_pos] = expr;
        }
        switch (expr->type) {
        case E_SYMBOL:
            rtrn = cnf_sym(sl, nt, expr->left.sym);
            goto go_up;
        case E_NOT:
            nt = !nt;
            if (rtrn == NULL)
                expr = expr->left.expr;
            else
                goto go_up;
            break;
        case E_OR:
        case E_AND:
            if (stack_pos < 0 || stack[stack_pos].expr != expr) {
                if (rtrn == NULL) {
                    expr = expr->left.expr;
                } else {
                    if (stack_size == ++stack_pos) {
                        stack_size *= 2;
                        stack =
                            realloc(stack,
                                    (unsigned) stack_size *
                                    sizeof(struct stck));
                    }
                    stack[stack_pos].expr = expr;
                    stack[stack_pos].cnf = rtrn;
                    expr = expr->right.expr;
                    rtrn = NULL;
                }
            } else {
                if ((!nt && expr->type == E_OR)
                    || (nt && expr->type == E_AND))
                    rtrn = cnf_or(stack[stack_pos].cnf, rtrn);
                else
                    rtrn = cnf_and(stack[stack_pos].cnf, rtrn);
                stack_pos--;
                goto go_up;
            }
            break;
        case E_EQUAL:
            rtrn = cnf_eql(sl, nt, expr->left.sym, expr->right.sym);
            goto go_up;
        case E_UNEQUAL:
            rtrn = cnf_eql(sl, !nt, expr->left.sym, expr->right.sym);
            goto go_up;
        default:
            fprintf(stderr, "ERROR: Unknown expression type.\n");
        }
        continue;

      go_up:
        if (--back_pos >= 0)
            expr = back[back_pos];
        else
            expr = NULL;
    }

    free(back);
    free(stack);

    struct symlist_el *se = symlist_find(sl, sym->name);
    if (se->prompt || nt == true)
        rtrn = cnf_or(cnf_sym(sl, !nt, sym), rtrn);
    else {
        struct cnfexpr *w12 = cnf_not(cnf_copy(rtrn));
        struct cnfexpr *w11 = cnf_sym(sl, false, sym);
        struct cnfexpr *w1 = cnf_or(w11, w12);
        struct cnfexpr *w22 = rtrn;
        struct cnfexpr *w21 = cnf_sym(sl, true, sym);
        struct cnfexpr *w2 = cnf_or(w21, w22);
        rtrn = cnf_and(w1, w2);
        /*
           rtrn =
           cnf_and(cnf_or
           (cnf_sym(sl, false, sym), cnf_not(cnf_copy(rtrn))),
           cnf_or(cnf_sym(sl, true, sym), rtrn));
         */
    }

    return rtrn;
}

void cnf_printf(struct cnfexpr *wcnf) {
    if (wcnf == NULL) {
        printf("hey NULL\n");
        return;
    }
    switch (wcnf->type) {
    case CT_TRUE:
        printf("True\n");
        return;
    case CT_FALSE:
        printf("False\n");
        return;
    case CT_EXPR:
        break;
    }
    unsigned i, r;
    for (i = 0; i < wcnf->size; i++) {
        for (r = 0; r < wcnf->sizes[i]; r++) {
            printf("%d ", wcnf->exprs[i][r]);
        }
        if (i < wcnf->size - 1)
            printf("x ");
    }
    printf("\n");
}

struct cnfexpr *cnf_sym(struct symlist *sl, bool not, struct symbol *sym) {
    struct cnfexpr *w = malloc(sizeof(struct cnfexpr));
    struct symlist_el *se = symlist_find(sl, sym->name);
    if (se == NULL) {
        if (!not)
            w->type = CT_FALSE;
        else
            w->type = CT_TRUE;
    } else {
        w->type = CT_EXPR;
        w->size = 1;
        w->sizes = malloc(sizeof(int));
        w->sizes[0] = 1;
        w->exprs = malloc(sizeof(int *));
        w->exprs[0] = malloc(sizeof(int));
        w->exprs[0][0] = (int) se->id;
        if (not)
            w->exprs[0][0] *= -1;
    }
    return w;
}

struct cnfexpr *cnf_eql(struct symlist *sl, bool not, struct symbol *sym1,
                        struct symbol *sym2) {
    if (!strcmp(sym2->name, "m")) {
        struct cnfexpr *fls = malloc(sizeof(struct cnfexpr));
        if (!not)
            fls->type = CT_FALSE;
        else
            fls->type = CT_TRUE;
        return fls;
    }
    if (!strcmp(sym2->name, "n")) {
        return cnf_sym(sl, !not, sym1);
    }
    if (!strcmp(sym2->name, "y")) {
        return cnf_sym(sl, not, sym1);
    }

    struct cnfexpr *w1 = cnf_sym(sl, not, sym1);
    struct cnfexpr *w2 = cnf_sym(sl, not, sym2);
    struct cnfexpr *w3 = cnf_sym(sl, !not, sym1);
    struct cnfexpr *w4 = cnf_sym(sl, !not, sym2);
    struct cnfexpr *wa = cnf_or(w1, w2);
    struct cnfexpr *wb = cnf_or(w3, w4);
    struct cnfexpr *w = cnf_and(wa, wb);
    return w;
}

struct cnfexpr *cnf_or(struct cnfexpr *e1, struct cnfexpr *e2) {
    switch (e1->type) {
    case CT_TRUE:
        free_cnf(e2);
        return e1;
    case CT_FALSE:
        free_cnf(e1);
        return e2;
    case CT_EXPR:
        switch (e2->type) {
        case CT_TRUE:
            free_cnf(e1);
            return e2;
        case CT_FALSE:
            free_cnf(e2);
            return e1;
        case CT_EXPR:
            break;
        }
        break;
    }

    unsigned oldsize = e1->size;
    e1->size *= e2->size;
    e1->sizes = realloc(e1->sizes, e1->size * sizeof(int));
    e1->exprs = realloc(e1->exprs, e1->size * sizeof(int *));
    unsigned i1, i2;
    // Duplicate e2->size times e1
    for (i2 = 1; i2 < e2->size; i2++) {
        memcpy(e1->sizes + (i2 * oldsize), e1->sizes,
               oldsize * sizeof(int));
        for (i1 = 0; i1 < oldsize; i1++) {
            e1->exprs[(i2 * oldsize) + i1] =
                malloc(e1->sizes[i1] * sizeof(int));
            memcpy(e1->exprs[(i2 * oldsize) + i1], e1->exprs[i1],
                   e1->sizes[i1] * sizeof(int));
        }
    }
    unsigned oldsizes;
    // Append e2->exprs to e1->exprs
    for (i2 = 0; i2 < e2->size; i2++) {
        for (i1 = 0; i1 < oldsize; i1++) {
            oldsizes = e1->sizes[(i2 * oldsize) + i1];
            e1->sizes[(i2 * oldsize) + i1] += e2->sizes[i2];
            e1->exprs[(i2 * oldsize) + i1] =
                realloc(e1->exprs[(i2 * oldsize) + i1],
                        e1->sizes[(i2 * oldsize) + i1] * sizeof(int));
            memcpy(e1->exprs[(i2 * oldsize) + i1] + oldsizes,
                   e2->exprs[i2], e2->sizes[i2] * sizeof(int));
        }
    }
    free_cnf(e2);
    return e1;
}

struct cnfexpr *cnf_and(struct cnfexpr *e1, struct cnfexpr *e2) {
    switch (e1->type) {
    case CT_FALSE:
        free_cnf(e2);
        return e1;
    case CT_TRUE:
        free_cnf(e1);
        return e2;
    case CT_EXPR:
        switch (e2->type) {
        case CT_FALSE:
            free_cnf(e1);
            return e2;
        case CT_TRUE:
            free_cnf(e2);
            return e1;
        case CT_EXPR:
            break;
        }
        break;
    }

    unsigned oldsize = e1->size;
    e1->size += e2->size;
    e1->sizes = realloc(e1->sizes, e1->size * sizeof(int));
    e1->exprs = realloc(e1->exprs, e1->size * sizeof(int *));
    memcpy(e1->sizes + oldsize, e2->sizes, e2->size * sizeof(int));
    unsigned i;
    for (i = 0; i < e2->size; i++) {
        e1->exprs[oldsize + i] = malloc(e2->sizes[i] * sizeof(int));
        memcpy(e1->exprs[oldsize + i], e2->exprs[i],
               e2->sizes[i] * sizeof(int));
    }
    free_cnf(e2);
    return e1;
}

struct cnfexpr *cnf_not_dissolve(struct cnfexpr *expr, unsigned i) {
    struct cnfexpr *w;
    unsigned y;
    w = malloc(sizeof(struct cnfexpr));
    w->type = CT_EXPR;
    w->size = expr->sizes[i];
    w->exprs = malloc(w->size * sizeof(int *));
    w->sizes = malloc(w->size * sizeof(unsigned));
    for (y = 0; y < w->size; y++) {
        w->sizes[y] = 1;
        w->exprs[y] = malloc(sizeof(int));
        w->exprs[y][0] = -1 * expr->exprs[i][y];
    }
    return w;
}

struct cnfexpr *cnf_not(struct cnfexpr *expr) {
    switch (expr->type) {
    case CT_TRUE:
        expr->type = CT_FALSE;
        return expr;
    case CT_FALSE:
        expr->type = CT_TRUE;
        return expr;
    case CT_EXPR:
        break;
    }
    struct cnfexpr *rtrn, *w;
    unsigned i = 0;
    rtrn = cnf_not_dissolve(expr, 0);
    for (i = 1; i < expr->size; i++) {
        w = cnf_not_dissolve(expr, i);
        rtrn = cnf_or(rtrn, w);
    }
    return rtrn;
}

struct cnfexpr *cnf_copy(struct cnfexpr *el) {
    struct cnfexpr *rtrn;
    rtrn = malloc(sizeof(struct cnfexpr));
    rtrn->type = el->type;
    rtrn->size = el->size;
    if (el->type != CT_EXPR)
        return rtrn;
    rtrn->sizes = malloc(rtrn->size * sizeof(int));
    memcpy(rtrn->sizes, el->sizes, rtrn->size * sizeof(int));
    rtrn->exprs = malloc(rtrn->size * sizeof(int *));
    unsigned i;
    for (i = 0; i < rtrn->size; i++) {
        rtrn->exprs[i] = malloc(rtrn->sizes[i] * sizeof(int));
        memcpy(rtrn->exprs[i], el->exprs[i], rtrn->sizes[i] * sizeof(int));
    }
    return rtrn;
}

void free_cnf(struct cnfexpr *e) {
    if (e->type != CT_EXPR) {
        free(e);
        return;
    }
    unsigned i;
    for (i = 0; i < e->size; i++) {
        free(e->exprs[i]);
    }
    free(e->exprs);
    free(e->sizes);
    free(e);
}


struct boolexp *printf_original(struct symlist *sl, struct expr *expr) {
    switch (expr->type) {
    case E_OR:
        printf("  OR\n");
        printf_original(sl, expr->left.expr);
        printf_original(sl, expr->right.expr);
        break;
    case E_AND:
        printf("  AND\n");
        printf_original(sl, expr->left.expr);
        printf_original(sl, expr->right.expr);
        break;
    case E_NOT:
        printf("  NOT\n");
        printf_original(sl, expr->left.expr);
        break;
    case E_EQUAL:
        printf("  = ");
        printf("%s ", expr->left.sym->name);
        if (!strcmp("y", expr->right.sym->name))
            printf("YES\n");
        else if (!strcmp("n", expr->right.sym->name))
            printf("NO\n");
        else if (!strcmp("m", expr->right.sym->name))
            printf("MODULE\n");
        else
            printf("%s\n", expr->left.sym->name);
        break;
    case E_UNEQUAL:
        printf("  != ");
        printf("%s ", expr->left.sym->name);
        if (!strcmp("y", expr->right.sym->name))
            printf("YES\n");
        else if (!strcmp("n", expr->right.sym->name))
            printf("NO\n");
        else
            printf("OTHER %s\n", expr->right.sym->name);
        break;
    case E_LIST:
        printf("  list\n");
        break;
    case E_SYMBOL:
        printf("  symbol");
        if (expr->left.sym->name != NULL)
            printf(": %s", expr->left.sym->name);
        printf("\n");
        break;
    case E_RANGE:
        printf("  range\n");
        break;
    case E_NONE:
        printf("  none\n");
        break;
    default:
        printf("  ERROR\n");
        break;
    }
}
