#include "boolexpr.h"

struct boolexpr *boolexpr_eql(struct symlist *sl, struct symbol *sym1,
                              struct symbol *sym2);

struct boolexpr *boolexpr_kconfig(struct symlist *sl, struct expr *expr) {
    struct stck {
        struct expr *expr;
        struct boolexpr *bl;
    };
    struct expr **back;
    int back_size = 2, back_pos = -1;
    back = malloc((unsigned) back_size * sizeof(struct expr *));
    struct stck *stack;
    unsigned stack_size = 2;
    int stack_pos = -1;
    stack = malloc((unsigned) stack_size * sizeof(struct stck));
    struct boolexpr *rtn = NULL;

    while (expr != NULL) {
        if ((back_pos >= 0 && back[back_pos] != expr) || back_pos < 0) {
            if (++back_pos >= back_size) {
                back_size *= 2;
                back =
                    realloc(back,
                            (unsigned) back_size * sizeof(struct expr *));
            }
            back[back_pos] = expr;
        }
        switch (expr->type) {
        case E_SYMBOL:
            rtn = boolexpr_sym(sl, expr->left.sym);
            goto go_up;
        case E_NOT:
            if (rtn == NULL)
                expr = expr->left.expr;
            else {
                rtn = boolexpr_not(rtn);
                goto go_up;
            }
            break;
        case E_OR:
        case E_AND:
            if (stack_pos < 0 || stack[stack_pos].expr != expr) {
                if (rtn == NULL)
                    expr = expr->left.expr;
                else {
                    if (++stack_pos >= stack_size) {
                        stack_size *= 2;
                        stack =
                            realloc(stack,
                                    stack_size * sizeof(struct stck));
                    }
                    stack[stack_pos].expr = expr;
                    stack[stack_pos].bl = rtn;
                    expr = expr->right.expr;
                    rtn = NULL;
                }
            } else {
                if (expr->type == E_OR)
                    rtn = boolexpr_or(stack[stack_pos].bl, rtn);
                else
                    rtn = boolexpr_and(stack[stack_pos].bl, rtn);
                stack_pos--;
                goto go_up;
            }
            break;
        case E_EQUAL:
            rtn = boolexpr_eql(sl, expr->left.sym, expr->right.sym);
            goto go_up;
        case E_UNEQUAL:
            rtn =
                boolexpr_not(boolexpr_eql
                             (sl, expr->left.sym, expr->right.sym));
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

    return rtn;
}

struct boolexpr *boolexpr_true() {
    struct boolexpr *rtn;
    rtn = malloc(sizeof(struct boolexpr));
    rtn->type = BT_TRUE;
    rtn->overusage = 0;
    rtn->id = 0;
    return rtn;
}

struct boolexpr *boolexpr_false() {
    struct boolexpr *rtn;
    rtn = malloc(sizeof(struct boolexpr));
    rtn->type = BT_FALSE;
    rtn->overusage = 0;
    rtn->id = 0;
    return rtn;
}

struct boolexpr *boolexpr_sym(struct symlist *sl, struct symbol *sym) {
    struct boolexpr *rtn;
    rtn = malloc(sizeof(struct boolexpr));
    rtn->overusage = 0;
    rtn->id = symlist_id(sl, sym->name);
    if (rtn->id != 0)
        rtn->type = BT_SYM;
    else
        rtn->type = BT_FALSE;
    return rtn;
}

struct boolexpr *boolexpr_eql(struct symlist *sl, struct symbol *sym1,
                              struct symbol *sym2) {
    if (!strcmp(sym2->name, "m")) {
        struct boolexpr *rtn = malloc(sizeof(struct boolexpr));
        rtn->overusage = 0;
        rtn->id = 0;
        rtn->type = BT_FALSE;
        return rtn;
    }
    if (!strcmp(sym2->name, "n"))
        return boolexpr_not(boolexpr_sym(sl, sym1));
    if (!strcmp(sym2->name, "y"))
        return boolexpr_sym(sl, sym1);

    // sym1 <-> sym2
    // (!sym1 || sym2) && (sym1 || !sym2)
    return
        boolexpr_and(boolexpr_or
                     (boolexpr_not(boolexpr_sym(sl, sym1)),
                      boolexpr_sym(sl, sym2)), boolexpr_or(boolexpr_sym(sl,
                                                                        sym1),
                                                           boolexpr_not
                                                           (boolexpr_sym
                                                            (sl, sym2))));
}

struct boolexpr *boolexpr_or(struct boolexpr *e1, struct boolexpr *e2) {
    switch (e1->type) {
    case BT_TRUE:
        boolexpr_free(e2);
        return e1;
    case BT_FALSE:
        boolexpr_free(e1);
        return e2;
    default:
        switch (e2->type) {
        case BT_TRUE:
            boolexpr_free(e1);
            return e2;
        case BT_FALSE:
            boolexpr_free(e2);
            return e1;
        default:
            break;
        }
        break;
    }

    struct boolexpr *rtn;
    rtn = malloc(sizeof(struct boolexpr));
    rtn->type = BT_OR;
    rtn->overusage = 0;
    rtn->id = 0;
    rtn->left = e1;
    rtn->right = e2;
    return rtn;
}

struct boolexpr *boolexpr_and(struct boolexpr *e1, struct boolexpr *e2) {
    switch (e1->type) {
    case BT_FALSE:
        boolexpr_free(e2);
        return e1;
    case BT_TRUE:
        boolexpr_free(e1);
        return e2;
    default:
        switch (e2->type) {
        case BT_FALSE:
            boolexpr_free(e1);
            return e2;
        case BT_TRUE:
            boolexpr_free(e2);
            return e1;
        default:
            break;
        }
        break;
    }

    struct boolexpr *rtn;
    rtn = malloc(sizeof(struct boolexpr));
    rtn->type = BT_AND;
    rtn->overusage = 0;
    rtn->id = 0;
    rtn->left = e1;
    rtn->right = e2;
    return rtn;
}

struct boolexpr *boolexpr_not(struct boolexpr *e) {
    struct boolexpr *rtn;
    rtn = malloc(sizeof(struct boolexpr));
    rtn->overusage = 0;
    rtn->id = 0;

    switch (e->type) {
    case BT_FALSE:
        rtn->type = BT_TRUE;
        boolexpr_free(e);
        break;
    case BT_TRUE:
        rtn->type = BT_FALSE;
        boolexpr_free(e);
        break;
    default:
        rtn->type = BT_NOT;
        rtn->left = e;
        break;
    }
    return rtn;
}

void boolexpr_free(struct boolexpr *e) {
    struct boolexpr **stack;
    size_t stack_size = 2, stack_pos = 0;
    stack = malloc(stack_size * sizeof(struct boolexpr *));

    struct boolexpr *m;
    while (e != NULL) {
        m = e;
        switch (e->type) {
        case BT_OR:
        case BT_AND:
            if (++stack_pos >= stack_size) {
                stack_size *= 2;
                stack =
                    realloc(stack, stack_size * sizeof(struct boolexpr *));
            }
            stack[stack_pos - 1] = e->right;
        case BT_NOT:
            e = e->left;
            break;
        default:
            if (stack_pos > 0)
                e = stack[--stack_pos];
            else
                e = NULL;
        }
        if (m->overusage > 0)
            m->overusage--;
        else
            free(m);
    }
    free(stack);
}

struct boolexpr *boolexpr_copy(struct boolexpr *e) {
    struct boolexpr **stack;
    size_t stack_size = 2, stack_pos = 0;
    stack = malloc(stack_size * sizeof(struct boolexpr *));

    while (e != NULL) {
        e->overusage++;
        switch (e->type) {
        case BT_OR:
        case BT_AND:
            if (++stack_pos >= stack_size) {
                stack_size *= 2;
                stack =
                    realloc(stack, stack_size * sizeof(struct boolexpr *));
            }
            stack[stack_pos - 1] = e->right;
        case BT_NOT:
            e = e->left;
            break;
        default:
            if (stack_pos > 0)
                e = stack[--stack_pos];
            else
                e = NULL;
        }
    }
    free(stack);
    return e;
}
