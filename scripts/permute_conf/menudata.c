#include "menudata.h"

struct menudata *menudata_new(void) {
    struct menudata *rtn;
    rtn = calloc(1, sizeof(struct menudata));
    return rtn;
}

void menudata_set_permute(struct menu *m, bool perm) {
    ((struct menudata *) m->data)->permute = perm;
    struct menu *prnt;
    for (prnt = m; prnt != NULL; prnt = prnt->parent) {
        menudata_cal(prnt);
    }
}

void menudata_set_all_permute(struct menu *m, bool perm) {
    menudata_set_permute(m, perm);

    struct menu **stack;
    size_t stack_size = 2, stack_pos = 0;
    stack = malloc(stack_size * sizeof(struct menu *));

    m = m->list;
    while (m != NULL) {
        if (m->data == NULL)
            m->data = menudata_new();
        ((struct menudata *) m->data)->permute = perm;
        ((struct menudata *) m->data)->subpermute = perm;

        if (m->list != NULL) {
            if (stack_pos >= stack_size) {
                stack_size *= 2;
                stack = realloc(stack, stack_size * sizeof(struct menu *));
            }
            stack[stack_pos++] = m->list;
        }

        m = m->next;
        if (m == NULL && stack_pos > 0)
            m = stack[--stack_pos];
    }

}

void menudata_cal(struct menu *m) {
    bool subperm = false;
    struct menu *w;
    for (w = m->list; w != NULL; w = w->next) {
        if (w->data != NULL && (((struct menudata *) w->data)->permute
                                || ((struct menudata *) w->data)->
                                subpermute)) {
            if (m->data == NULL)
                m->data = menudata_new();
            ((struct menudata *) m->data)->subpermute = subperm;
        }
    }
}
