#include "boolexp.h"

struct boolexp *copy_kconfig_dep(struct symlist *sl, struct expr *expr) {
    struct boolexp *w;
    switch (expr->type) {
    case E_SYMBOL:
        w = malloc(sizeof(struct boolexp));
        w->type = BE_LEAF;
        struct symlist_el *sel;
        sel = symlist_find(sl, expr->left.sym->name);
        if (sel == NULL)
            return NULL;
        w->left.id = sel->id;
        return w;
    case E_AND:
    case E_OR:
        w = malloc(sizeof(struct boolexp));
        switch (expr->type) {
        case E_AND:
            w->type = BE_AND;
            break;
        case E_OR:
            w->type = BE_OR;
            break;
        }
        w->left.be = copy_kconfig_dep(sl, expr->left.expr);
        if (w->left.be == NULL) {
            free(w);
            return copy_kconfig_dep(sl, expr->right.expr);
        }
        w->right.be = copy_kconfig_dep(sl, expr->right.expr);
        if (w->right.be == NULL) {
            struct boolexp *ret = w->left.be;
            free(w);
            return ret;
        }
        return w;
    case E_EQUAL:
    case E_UNEQUAL:
        return NULL;
    case E_NOT:
        w = malloc(sizeof(struct boolexp));
        w->type = BE_NOT;
        w->left.be = copy_kconfig_dep(sl, expr->left.expr);
        if (w->left.be == NULL) {
            free(w);
            return NULL;
        }
        return w;
    default:
        fprintf(stderr, "Error (%d): %s\n", expr->type,
                sym_type_name(expr->type));
        return NULL;
    }
}

struct boolexp *copy_kconfig_f_dep(struct symlist *sl, struct expr *expr) {
    switch (expr->type) {
    case E_OR:
        printf("OR\n");
        copy_kconfig_f_dep(sl, expr->left.expr);
        copy_kconfig_f_dep(sl, expr->right.expr);
        break;
    case E_AND:
        printf("AND\n");
        copy_kconfig_f_dep(sl, expr->left.expr);
        copy_kconfig_f_dep(sl, expr->right.expr);
        break;
    case E_NOT:
        printf("NOT\n");
        copy_kconfig_f_dep(sl, expr->left.expr);
        break;
    case E_EQUAL:
        printf("= ");
        printf("%s ", expr->left.sym->name);
        if (!strcmp("y", expr->right.sym->name))
            printf("YES\n");
        else if (!strcmp("n", expr->right.sym->name))
            printf("NO\n");
        else
            printf("PROBLEM\n");
        break;
    case E_UNEQUAL:
        printf("!= ");
        printf("%s ", expr->left.sym->name);
        if (!strcmp("y", expr->right.sym->name))
            printf("YES\n");
        else if (!strcmp("n", expr->right.sym->name))
            printf("NO\n");
        else
            printf("OTHER %s\n", expr->right.sym->name);
        break;
    case E_LIST:
        printf("list\n");
        break;
    case E_SYMBOL:
        printf("symbol\n");
        break;
    case E_RANGE:
        printf("range\n");
        break;
    case E_NONE:
        printf("none\n");
        break;
    default:
        printf("ERROR\n");
        break;
    }

}

/*struct cnfexpr *kconfig_dep(struct symlist *sl, struct expr *expr) {
    struct nd {
        struct cnfexpr *cnfe;
        struct expr *expr;
    };
    struct cnfexpr *ret = NULL;
    struct nd *wlist;
    int wlist_size = 2, wlist_pos = -1;
    struct expr **backstack;
    int backstack_size = 2, backstack_pos = -1;
    bool neg = false;
    int ng;

    wlist = malloc((unsigned) wlist_size * sizeof(struct nd));
    backstack = malloc((unsigned) backstack_size * sizeof(struct expr *));

    while (expr != NULL) {
        if (neg)
            ng = -1;
        else
            ng = 1;
        if ((backstack_pos >= 0 && backstack[backstack_pos] != expr)
            || backstack_pos < 0) { // left
            if (++backstack_pos >= backstack_size) {
                backstack_size *= 2;
                backstack =
                    realloc(backstack,
                            (unsigned) backstack_size *
                            sizeof(struct expr *));
            }
            backstack[backstack_pos] = expr;
            switch (expr->type) {
            case E_SYMBOL:
                ret = malloc(sizeof(struct cnfexpr));
                ret->size = 1;
                ret->sizes = malloc(sizeof(int));
                ret->sizes[0] = 1;
                ret->exprs = malloc(sizeof(int *));
                ret->exprs[0] = malloc(sizeof(int));
                ret->exprs[0][0] = ng * (int)
                    symlist_find(sl, expr->left.sym->name)->id;
                goto back;
            case E_OR:
            case E_AND:
                if (expr->left.expr->type != E_SYMBOL)
                    expr = expr->left.expr;
                break;
            case E_NOT:
                neg = !neg;     // Is it working?
                expr = expr->left.expr;
                break;
            case E_EQUAL:
            case E_UNEQUAL:
                ng = 1;
                if (expr->type == E_UNEQUAL)
                    ng *= -1;
                if (!strcmp("y", expr->right.sym->name)) {
                    // pass
                } else if (!strcmp("n", expr->right.sym->name)) {
                    ng *= -1;
                } else if (!strcmp("m", expr->right.sym->name)) {
                    ret = NULL;
                    goto back;
                } else {
                    fprintf(stderr, "ERROR: kconfig_dep unknown equal\n");
                    exit(1);
                }
                ret = malloc(sizeof(struct cnfexpr));
                ret->size = 1;
                ret->sizes = malloc(sizeof(unsigned));
                ret->sizes[0] = 1;
                ret->exprs = malloc(sizeof(int *));
                ret->exprs[0] = malloc(sizeof(int));
                ret->exprs[0][0] = ng *
                    (int) symlist_find(sl, expr->left.sym->name)->id;
                goto back;
            default:
                fprintf(stderr, "ERROR: kconfig_dep unknown left state\n");
                exit(1);
            }
        } else {                // right
            switch (expr->type) {
            case E_OR:
            case E_AND:
                if (wlist_pos >= 0 && wlist[wlist_pos].expr == expr) {
                    // navrat z prave strany v ret prava strana
                    if (ret == NULL) {
                        if ((!neg && expr->type == E_OR)
                            || (neg && expr->type == E_AND)) {
                            ret = wlist[wlist_pos--].cnfe;
                        } else {
                            struct cnfexpr *wcnf = wlist[wlist_pos--].cnfe;
                            unsigned i;
                            for (i = 0; i < wcnf->size; i++) {
                                free(wcnf->exprs[i]);
                            }
                            free(wcnf->sizes);
                            free(wcnf->exprs);
                            free(wcnf);
                        }
                        goto back;
                    }
                    if ((!neg && expr->type == E_OR)
                        || (neg && expr->type == E_AND)) {
                        struct cnfexpr *wcnf = wlist[wlist_pos--].cnfe;
                        unsigned oldsize = ret->size;
                        ret->size *= wcnf->size;
                        ret->sizes =
                            realloc(ret->sizes, ret->size * sizeof(int));
                        ret->exprs =
                            realloc(ret->exprs, ret->size * sizeof(int *));
                        unsigned i1, i2;
                        for (i1 = 1; i1 < wcnf->size; i1++) {
                            for (i2 = 0; i2 < oldsize; i2++) {
                                ret->sizes[(i1 * oldsize) + i2] =
                                    ret->sizes[i2];
                                ret->exprs[(i1 * oldsize) + i2] =
                                    malloc(ret->sizes[i2] * sizeof(int));
                                memcpy(ret->exprs[(i1 * oldsize) + i2],
                                       ret->exprs[i2],
                                       ret->sizes[i2] * sizeof(int));
                            }
                        }
                        for (i1 = 0; i1 < wcnf->size; i1++) {
                            for (i2 = 0; i2 < oldsize; i2++) {
                                unsigned offset =
                                    ret->sizes[(i1 * oldsize) + i2];
                                ret->sizes[(i1 * oldsize) + i2] +=
                                    wcnf->sizes[i1];
                                ret->exprs[(i1 * oldsize) + i2] =
                                    realloc(ret->exprs
                                            [(i1 * oldsize) + i2],
                                            ret->sizes[(i1 * oldsize) +
                                                       i2] * sizeof(int));
                                memcpy(ret->exprs[(i1 * oldsize) + i2] +
                                       offset, wcnf->exprs[i1],
                                       wcnf->sizes[i1] * sizeof(int));
                            }
                        }
                        for (i1 = 0; i1 < wcnf->size; i1++)
                            free(wcnf->exprs[i1]);
                        free(wcnf->sizes);
                        free(wcnf->exprs);
                        free(wcnf);
                    } else {
                        struct cnfexpr *wcnf = wlist[wlist_pos--].cnfe;
                        unsigned oldsize = ret->size;
                        ret->size += wcnf->size;
                        ret->sizes =
                            realloc(ret->sizes, ret->size * sizeof(int));
                        ret->exprs =
                            realloc(ret->exprs, ret->size * sizeof(int *));
                        memcpy(ret->sizes + oldsize, wcnf->sizes,
                               wcnf->size * sizeof(int));
                        memcpy(ret->exprs + oldsize, wcnf->exprs,
                               wcnf->size * sizeof(int *));
                        free(wcnf->sizes);
                        free(wcnf->exprs);
                        free(wcnf);
                    }
                    goto back;
                } else if (expr->left.expr->type == E_SYMBOL) {
                    if (expr->right.expr->type != E_SYMBOL) {
                        if (++wlist_pos >= wlist_size) {
                            wlist_size *= 2;
                            wlist =
                                realloc(wlist,
                                        (unsigned) wlist_size *
                                        sizeof(struct nd));
                        }
                        wlist[wlist_pos].expr = expr;
                        struct cnfexpr *w = malloc(sizeof(struct cnfexpr));
                        w->size = 1;
                        w->sizes = malloc(sizeof(int));
                        w->sizes[0] = 1;
                        w->exprs = malloc(sizeof(int *));
                        w->exprs[0] = malloc(sizeof(int));
                        w->exprs[0][0] = ng * (int)
                            symlist_find(sl,
                                         expr->left.expr->left.sym->
                                         name)->id;
                        wlist[wlist_pos].cnfe = w;
                        expr = expr->right.expr;
                    } else {
                        ret = malloc(sizeof(struct cnfexpr));
                        if ((!neg && expr->type == E_OR)
                            || (neg && expr->type == E_AND)) {
                            ret->size = 1;
                            ret->sizes = malloc(sizeof(int));
                            ret->sizes[0] = 2;
                            ret->exprs = malloc(sizeof(int *));
                            ret->exprs[0] = malloc(2 * sizeof(int));
                            ret->exprs[0][0] = ng * (int)
                                symlist_find(sl,
                                             expr->left.expr->left.
                                             sym->name)->id;
                            ret->exprs[0][1] = ng * (int)
                                symlist_find(sl,
                                             expr->right.expr->left.
                                             sym->name)->id;
                        } else {
                            ret->size = 2;
                            ret->sizes = malloc(2 * sizeof(int));
                            ret->sizes[0] = 1;
                            ret->sizes[1] = 1;
                            ret->exprs = malloc(2 * sizeof(int *));
                            ret->exprs[0] = malloc(sizeof(int));
                            ret->exprs[1] = malloc(sizeof(int));
                            ret->exprs[0][0] =
                                ng * (int) symlist_find(sl,
                                                        expr->left.expr->
                                                        left.sym->name)->
                                id;
                            ret->exprs[1][0] =
                                ng * (int) symlist_find(sl,
                                                        expr->right.expr->
                                                        left.sym->name)->
                                id;
                        }
                        goto back;
                    }
                } else if (ret != NULL) {
                    if (expr->right.expr->type == E_SYMBOL) {
                        if ((!neg && expr->type == E_OR)
                            || (neg && expr->type == E_AND)) {
                            unsigned i;
                            for (i = 0; i < ret->size; i++) {
                                ret->sizes[i]++;
                                ret->exprs[i] =
                                    realloc(ret->exprs[i],
                                            ret->sizes[i] * sizeof(int));
                                ret->exprs[i][ret->sizes[i] - 1] =
                                    ng * (int)
                                    symlist_find(sl,
                                                 expr->right.expr->
                                                 left.sym->name)->id;
                            }
                        } else {
                            ret->size++;
                            ret->exprs =
                                realloc(ret->exprs,
                                        ret->size * sizeof(int *));
                            ret->exprs[ret->size - 1] =
                                malloc(sizeof(int));
                            ret->sizes =
                                realloc(ret->sizes,
                                        ret->size * sizeof(int));
                            ret->sizes[ret->size - 1] = 1;
                            ret->exprs[ret->size - 1][0] =
                                ng * (int) symlist_find(sl,
                                                        expr->right.
                                                        expr->left.sym->
                                                        name)->id;
                        }
                        goto back;
                    } else {
                        if (++wlist_pos >= wlist_size) {
                            wlist_size *= 2;
                            wlist =
                                realloc(wlist,
                                        (unsigned) wlist_size *
                                        sizeof(struct nd));
                        }
                        wlist[wlist_pos].cnfe = ret;
                        wlist[wlist_pos].expr = expr;
                        expr = expr->right.expr;
                    }
                } else {
                    // navrat z leva, v lefo je false
                    if (expr->right.expr->type == E_SYMBOL) {
                        if ((!neg && expr->type == E_OR)
                            || (neg && expr->type == E_AND)) {
                        } else
                            goto back;
                    } else {
                        if ((!neg && expr->type == E_OR)
                            || (neg && expr->type == E_AND)) {
                        } else {
                        }
                    }
                }
                break;
            case E_NOT:
                neg = !neg;
                goto back;
            default:
                fprintf(stderr,
                        "ERROR: kconfig_dep unknown right state\n");
            }
        }
        continue;
      back:
        if (backstack_pos > 0) {
            expr = backstack[--backstack_pos];
        } else
            expr = NULL;
    }

    return ret;
}

void cnfprint(struct cnfexpr *wcnf) {
    unsigned i, r;
    for (i = 0; i < wcnf->size; i++) {
        for (r = 0; r < wcnf->sizes[i]; r++) {
            printf("%d ", wcnf->exprs[i][r]);
        }
        printf("x ");
    }
    printf("\n");
}*/

// This function is leaking memory! TODO
struct boolexp *boolexp_cnf(struct boolexp *be) {
    if (be->type == BE_NOT) {
        if (be->left.be->type == BE_OR || be->left.be->type == BE_AND) {
            struct boolexp *root, *nleft, *nright;
            root = malloc(sizeof(struct boolexp));
            nleft = malloc(sizeof(struct boolexp));
            nright = malloc(sizeof(struct boolexp));
            if (be->left.be->type == BE_OR)
                root->type = BE_AND;
            else
                root->type = BE_OR;
            nleft->type = BE_NOT;
            nright->type = BE_NOT;
            root->left.be = nleft;
            root->right.be = nright;
            nleft->left.be = be->left.be;
            nright->left.be = be->right.be;
            be = root;
        }
    } else if (be->type == BE_OR) {
        if (be->left.be->type == BE_AND) {
            struct boolexp *root, *nleft, *nright;
            root = malloc(sizeof(struct boolexp));
            nleft = malloc(sizeof(struct boolexp));
            nright = malloc(sizeof(struct boolexp));
            root->type = BE_AND;
            nleft->type = BE_OR;
            nright->type = BE_OR;
            root->left.be = nleft;
            root->right.be = nright;
            nleft->left.be = be->left.be->left.be;
            nleft->right.be = be->right.be;
            nright->left.be = be->left.be->right.be;
            nright->right.be = be->right.be;
            be = root;
        } else if (be->right.be->type == BE_AND) {
            struct boolexp *root, *nleft, *nright;
            root = malloc(sizeof(struct boolexp));
            nleft = malloc(sizeof(struct boolexp));
            nright = malloc(sizeof(struct boolexp));
            root->type = BE_AND;
            nleft->type = BE_OR;
            nright->type = BE_OR;
            root->left.be = nleft;
            root->right.be = nright;
            nleft->left.be = be->left.be;
            nleft->right.be = be->right.be->left.be;
            nright->left.be = be->left.be;
            nright->right.be = be->right.be->right.be;
            be = root;
        }
    }
    if (be->type == BE_OR || be->type == BE_AND || be->type == BE_NOT)
        be->left.be = boolexp_cnf(be->left.be);
    if (be->type == BE_OR || be->type == BE_AND)
        be->right.be = boolexp_cnf(be->right.be);
    return be;
}

void boolexp_print(struct boolexp *be) {
    if (be != NULL)
        switch (be->type) {
        case BE_LEAF:
            printf("%d", be->left.id);
            break;
        case BE_AND:
            //printf("(");
            boolexp_print(be->left.be);
            printf(" and ");
            boolexp_print(be->right.be);
            //printf(")");
            break;
        case BE_OR:
            printf("(");
            boolexp_print(be->left.be);
            printf(" or ");
            boolexp_print(be->right.be);
            printf(")");
            break;
        case BE_NOT:
            printf("-");
            boolexp_print(be->left.be);
            break;
        }
}
