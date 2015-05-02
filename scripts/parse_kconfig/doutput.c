#include "doutput.h"

void doutput_expr(struct expr *expr) {
#ifdef DEBUG
    if (verbose_level <= 3)
        return;
    switch (expr->type) {
    case E_OR:
        printf("  OR\n");
        doutput_expr(expr->left.expr);
        doutput_expr(expr->right.expr);
        break;
    case E_AND:
        printf("  AND\n");
        doutput_expr(expr->left.expr);
        doutput_expr(expr->right.expr);
        break;
    case E_NOT:
        printf("  NOT\n");
        doutput_expr(expr->left.expr);
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
#endif /* DEBUG */
}

void doutput_boolexpr(struct boolexpr *bl, struct symlist *sl) {
#ifdef DEBUG
    if (verbose_level <= 3)
        return;
    switch (bl->type) {
    case BT_TRUE:
        printf("  true\n");
        break;
    case BT_FALSE:
        printf("  false\n");
        break;
    case BT_SYM:
        printf("  CONFIG_%s\n", sl->array[bl->id - 1].name);
        break;
    case BT_OR:
        printf("  OR\n");
        doutput_boolexpr(bl->left, sl);
        doutput_boolexpr(bl->right, sl);
        break;
    case BT_AND:
        printf("  AND\n");
        doutput_boolexpr(bl->left, sl);
        doutput_boolexpr(bl->right, sl);
        break;
    case BT_NOT:
        if (bl->left->type == BT_SYM) {
            printf("  NOT CONFIG_%s\n", sl->array[bl->left->id - 1].name);
        } else {
            printf("  NOT\n");
            doutput_boolexpr(bl->left, sl);
        }
        break;
    }
#endif /* DEBUG */
}
