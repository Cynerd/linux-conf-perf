#include "output.h"

void fprint_rules(struct symlist *sl, char* output) {
    FILE *f;
    f = fopen(output, "w");
    if (f == NULL) {
        fprintf(stderr, "Can't create file: %s\n", output);
        return;
    }
    int i;
    struct symlist_el *el;
    struct boolexp *be;
    struct boolexp **stack;
    size_t stack_size = 2, stack_pos = 0;
    int count_or, count_and;
    stack = malloc(stack_size * sizeof(struct boolexp *));
    for (i = 0; i < sl->pos; i++) {
        if (sl->array[i].be != NULL) {
            el = sl->array + i;
            be = el->be;
            stack_pos = 0;
            count_or = 0;
            count_and = 0;
            fprintf(f, "-%d ", el->id);
            while (be != NULL) {
                if (stack_pos >= stack_size) {
                    stack_size *= 2;
                    stack =
                        realloc(stack,
                                stack_size * sizeof(struct boolexp *));
                }
                switch (be->type) {
                case BE_NOT:
                    fprintf(f, "-");
                    be = be->left.be;
                    break;
                case BE_AND:
                    count_and++;
                    stack[stack_pos++] = be->right.be;
                    be = be->left.be;
                    break;
                case BE_OR:
                    count_or++;
                    stack[stack_pos++] = be->right.be;
                    be = be->left.be;
                    break;
                case BE_LEAF:
                    fprintf(f, "%d", be->left.id);
                    if (count_or > 0) {
                        fprintf(f, " ");
                        count_or--;
                    } else if (count_and > 0) {
                        fprintf(f, "\n-%d ", el->id);
                        count_and--;
                    }
                    if (stack_pos > 0)
                        be = stack[--stack_pos];
                    else
                        be = NULL;
                    break;
                }
            }
            fprintf(f, "\n");
        }
    }
    free(stack);
    fclose(f);
}

void fprint_symbol_map(struct symlist *sl, char* output) {
    FILE *f;
    f = fopen(output, "w");
    if (f == NULL) {
        fprintf(stderr, "Can't create file: %s\n", output);
        return;
    }
    int i;
    for (i = 0; i < sl->pos; i++) {
        fprintf(f, "%d %s\n", sl->array[i].id, sl->array[i].name);
    }
    fclose(f);
}
