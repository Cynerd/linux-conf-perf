#include "cnfbuild.h"

struct cnfexpr *kconfig_expr(struct symlist *sl, struct expr *expr) {
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
    struct cnfexpr *rtn = NULL;

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
            rtn = cnf_sym(sl, expr->left.sym);
            goto go_up;
        case E_NOT:
            if (rtn == NULL)
                expr = expr->left.expr;
            else {
                rtn = cnf_not(sl, rtn);
                goto go_up;
            }
            break;
        case E_OR:
        case E_AND:
            if (stack_pos < 0 || stack[stack_pos].expr != expr) {
                if (rtn == NULL)
                    expr = expr->left.expr;
                else {
                    if (stack_size >= ++stack_pos) {
                        stack_size *= 2;
                        stack =
                            realloc(stack,
                                    (unsigned) stack_size *
                                    sizeof(struct stck *));
                    }
                    stack[stack_pos].expr = expr;
                    stack[stack_pos].cnf = rtn;
                    expr = expr->right.expr;
                    rtn = NULL;
                }
            } else {
                if (expr->type == E_OR)
                    rtn = cnf_or(sl, stack[stack_pos].cnf, rtn);
                else
                    rtn = cnf_and(sl, stack[stack_pos].cnf, rtn);
                stack_pos--;
                goto go_up;
            }
            break;
        case E_EQUAL:
            rtn = cnf_eql(sl, expr->left.sym, expr->right.sym);
            goto go_up;
        case E_UNEQUAL:
            rtn =
                cnf_not(sl, cnf_eql(sl, expr->left.sym, expr->right.sym));
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

struct cnfexpr *cnfexpr_true(struct symlist *sl) {
    struct cnfexpr *rtn;
    rtn = malloc(sizeof(struct cnfexpr));
    rtn->type = CT_TRUE;
    rtn->id = 0;
    rtn->out = NULL;
    return rtn;
}

struct cnfexpr *cnfexpr_false(struct symlist *sl) {
    struct cnfexpr *rtn;
    rtn = malloc(sizeof(struct cnfexpr));
    rtn->type = CT_FALSE;
    rtn->id = 0;
    rtn->out = NULL;
    return rtn;
}

struct cnfexpr *cnf_sym(struct symlist *sl, struct symbol *sym) {
    struct cnfexpr *rtn;
    rtn = malloc(sizeof(struct cnfexpr));
    rtn->type = CT_EXPR;
    rtn->id = symlist_id(sym->name);
    rtn->out = NULL;
    if (rtn->id == 0)
        rtn->type = CT_FALSE;
    return rtn;
}

struct cnfexpr *cnf_eql(struct symlist *sl, struct symbol *sym1,
                        struct symbol *sym2) {
    if (!strcmp(sym2->name, "m")) {
        struct cnfexpr *fls = malloc(sizeof(struct cnfexpr));
        fls->type = CT_FALSE;
        return fls;
    }
    if (!strcmp(sym2->name, "n"))
        return cnf_not(sl, cnf_sym(sl, sym1));
    if (!strcmp(sym2->name, "y"))
        return cnf_sym(sl, sym1);

    // sym1 <-> sym2
    // (!sym1 || sym2) && (sym1 || !sym2)
    return cnf_and(sl,
                   cnf_or(sl, cnf_not(sl, cnf_sym(sl, sym1)),
                          cnf_sym(sl, sym2)), cnf_or(sl, cnf_sym(sl,
                                                                 sym1),
                                                     cnf_not(sl,
                                                             cnf_sym
                                                             (sl, sym2))));
}

struct cnfexpr *cnf_or(struct symlist *sl, struct cnfexpr *e1,
                       struct cnfexpr *e2) {
    switch (e1->type) {
    case CT_TRUE:
        cnfexpr_free(e2);
        return e1;
    case CT_FALSE:
        cnfexpr_free(e1);
        return e2;
    case CT_EXPR:
        switch (e2->type) {
        case CT_TRUE:
            cnfexpr_free(e1);
            return e2;
        case CT_FALSE:
            cnfexpr_free(e2);
            return e1;
        case CT_EXPR:
            break;
        }
        break;
    }

    unsigned newid = (unsigned) symlist_adddummy(sl);
    e1->out = output_rules_joinexpr(e1->out, e2->out);
    if (e1->out == NULL)
        e1->out = output_rules_newexpr();

    // rtn <-> (e1 || e2)
    // (!rtn || e1 || e2) && (rtn || !e1) && (rtn || !e2)
    output_rules_symbol(e1->out, -(int) newid);
    output_rules_symbol(e1->out, (int) e1->id);
    output_rules_symbol(e1->out, (int) e2->id);
    output_rules_endterm(e1->out);
    output_rules_symbol(e1->out, (int) rtn->id);
    output_rules_symbol(e1->out, -(int) e1->id);
    output_rules_endterm(e1->out);
    output_rules_symbol(e1->out, (int) rtn->id);
    output_rules_symbol(e1->out, -(int) e2->id);
    output_rules_endterm(e1->out);

    cnfexpr_free(e2);
    e1->id = newid;
    return e1;
}

struct cnfexpr *cnf_and(struct symlist *sl, struct cnfexpr *e1,
                        struct cnfexpr *e2) {
    switch (e1->type) {
    case CT_FALSE:
        cnfexpr_free(e2);
        return e1;
    case CT_TRUE:
        cnfexpr_free(e1);
        return e2;
    case CT_EXPR:
        switch (e2->type) {
        case CT_FALSE:
            cnfexpr_free(e1);
            return e2;
        case CT_TRUE:
            cnfexpr_free(e2);
            return e1;
        case CT_EXPR:
            break;
        }
        break;
    }

    unsigned newid = (unsigned) symlist_adddummy(sl);
    e1->out = output_rules_joinexpr(e1->out, e2->out);
    if (e1->out == NULL)
        e1->out = output_rules_newexpr();

    // rtn <-> (e1 && e2)
    // (rtn || !e1 || !e2) && (!rtn || e1) && (!rtn || e2)
    output_rules_symbol(e1->out, (int) rtn->id);
    output_rules_symbol(e1->out, -(int) e1->id);
    output_rules_symbol(e1->out, -(int) e2->id);
    output_rules_endterm(e1->out);
    output_rules_symbol(e1->out, -(int) rtn->id);
    output_rules_symbol(e1->out, (int) e1->id);
    output_rules_endterm(e1->out);
    output_rules_symbol(e1->out, -(int) rtn->id);
    output_rules_symbol(e1->out, (int) e2->id);
    output_rules_endterm(e1->out);

    cnfexpr_free(e2);
    e1->id = newid;
    return e1;
}

struct cnfexpr *cnf_not(struct symlist *sl, struct cnfexpr *e) {
    switch (e->type) {
    case CT_FALSE:
        e->type = CT_TRUE;
        return e;
    case CT_TRUE:
        e->type = CT_FALSE;
        return e;
    case CT_EXPR:
        break;
    }
    unsigned newid = (unsigned) symlist_adddummy(sl);

    // rtn <-> !e
    // (rtn || e) && (!rtn || !e)
    output_rules_symbol(e->out, (int) newid);
    output_rules_symbol(e->out, (int) e->id);
    output_rules_endterm(e->out);
    output_rules_symbol(e->out, -(int) newid);
    output_rules_symbol(e->out, -(int) e->id);
    output_rules_endterm(e->out);

    e->id = newid;
    return e;
}

struct cnfexpr *cnfexpr_copy(struct cnfexpr *e) {
    struct cnfexpr *rtn;
    rtn = malloc(sizeof(struct cnfexpr));
    rtn->type = e->type;
    rtn->id = e->id;
    rtn->out = output_rules_copycnf(e->out);
    return rtn;
}

void cnfexpr_free(struct cnfexpr *e) {
    output_rules_freexpr(e->out);
    free(e);
}
