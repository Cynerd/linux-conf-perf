#include "cnfbuild.h"

struct stck {
    struct boolexpr **stack;
    size_t size, pos;
};

struct stck *stack_create();
void stack_push(struct stck *s, struct boolexpr *b);
struct boolexpr *stack_pop(struct stck *s);

void cnf_and(struct symlist *sl, struct boolexpr *bl);
void cnf_or(struct symlist *sl, struct boolexpr *bl);
void cnf_not(struct symlist *sl, struct boolexpr *bl);

void cnf_boolexpr(struct symlist *sl, struct boolexpr *bl) {
    if (bl->type == BT_TRUE) {
        // Term is always true. No write required.
        return;
    } else if (bl->type == BT_FALSE) {
        fprintf(stderr,
                "E: Trying to write false term. This shouldn't happen.\n");
        exit(6);
    }

    struct stck *stack = stack_create();
    while (bl != NULL) {
        switch (bl->type) {
        case BT_NOT:
            if (bl->left->id != 0) {
                cnf_not(sl, bl);
                bl = stack_pop(stack);
            } else {
                stack_push(stack, bl);
                bl = bl->left;
            }
            break;
        case BT_AND:
        case BT_OR:
            if (bl->left->id != 0) {
                if (bl->right->id != 0) {
                    if (bl->type == BT_OR) {
                        cnf_or(sl, bl);
                        bl = stack_pop(stack);
                    } else {    /* BT_AND */
                        cnf_and(sl, bl);
                        bl = stack_pop(stack);
                    }
                } else {
                    stack_push(stack, bl);
                    bl = bl->right;
                }
            } else {
                stack_push(stack, bl);
                bl = bl->left;
            }
            break;
        default:
            bl = stack_pop(stack);
            break;
        }
    }
}

struct stck *stack_create() {
    struct stck *rtn;
    rtn = malloc(sizeof(struct stck));
    rtn->size = 2;
    rtn->pos = 0;
    rtn->stack = malloc(rtn->size * sizeof(struct boolexpr *));
    return rtn;
}

void stack_push(struct stck *s, struct boolexpr *b) {
    if (++(s->pos) >= s->size) {
        s->size *= 2;
        s->stack = realloc(s->stack, s->size * sizeof(struct boolexpr *));
    }
    s->stack[s->pos - 1] = b;
}

inline struct boolexpr *stack_pop(struct stck *s) {
    if (s->pos > 0)
        return s->stack[--(s->pos)];
    else
        return NULL;
}

void cnf_and(struct symlist *sl, struct boolexpr *bl) {
    if (bl->id != 0)
        return;
    bl->id = symlist_adddummy(sl);
    // bl->id <-> (bl->left->id && bl->right->id)
    //
    // (bl->id || !bl->left->id || !bl->right->id) &&
    // (!bl->id || bl->left->id) &&
    // (!bl->id || bl->right->id)
    output_rules_symbol((int) bl->id);
    output_rules_symbol(-(int) bl->left->id);
    output_rules_symbol(-(int) bl->right->id);
    output_rules_endterm();
    output_rules_symbol(-(int) bl->id);
    output_rules_symbol((int) bl->left->id);
    output_rules_endterm();
    output_rules_symbol(-(int) bl->id);
    output_rules_symbol((int) bl->right->id);
    output_rules_endterm();
}

void cnf_or(struct symlist *sl, struct boolexpr *bl) {
    if (bl->id != 0)
        return;
    bl->id = symlist_adddummy(sl);
    // bl->id <-> (bl->left->id || bl->right->id)
    //
    // (!bl->id || bl->left->id || bl->right->id) &&
    // (bl->id || !bl->left->id) &&
    // (bl->id || !bl->right->id)
    output_rules_symbol(-(int) bl->id);
    output_rules_symbol((int) bl->left->id);
    output_rules_symbol((int) bl->right->id);
    output_rules_endterm();
    output_rules_symbol((int) bl->id);
    output_rules_symbol(-(int) bl->left->id);
    output_rules_endterm();
    output_rules_symbol((int) bl->id);
    output_rules_symbol(-(int) bl->right->id);
    output_rules_endterm();
}

void cnf_not(struct symlist *sl, struct boolexpr *bl) {
    if (bl->id != 0)
        return;
    bl->id = symlist_adddummy(sl);
    // bl->id <-> !bl->left->id
    // (bl->id || bl->left->id) && (!bl->id || !bl->left->id)
    output_rules_symbol((int) bl->id);
    output_rules_symbol((int) bl->left->id);
    output_rules_endterm();
    output_rules_symbol(-(int) bl->id);
    output_rules_symbol(-(int) bl->left->id);
    output_rules_endterm();
}
