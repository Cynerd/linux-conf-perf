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
            else if (be->left.be->type == BE_AND)
                root->type = BE_OR;
            nleft->type = BE_NOT;
            nright->type = BE_NOT;
            root->left.be = nleft;
            root->right.be = nright;
            nleft->left.be = be->left.be;
            nright->left.be = be->right.be;
            be = root;
        }
    } else if (be->type == BE_OR)
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
