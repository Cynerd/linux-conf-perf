#include "output.h"

FILE *frules, *fsymmap;

int output_init(char *rules_file, char *symbolmap_file) {
    if ((frules = fopen(rules_file, "w")) == NULL)
        return 1;
    if ((fsymmap = fopen(symbolmap_file, "w")) == NULL)
        return 2;
    return 0;
}

void output_finish(void) {
    fclose(frules);
    fclose(fsymmap);
}


// Functions for symbol_map
void output_push_symbol(int id, char *name) {
    fprintf(fsymmap, "%d:%s\n", id, name);
}

// Functions for rules
struct output_expr *output_rules_newexpr(void) {
    struct output_expr *rtn;
    rtn = malloc(sizeof(struct output_expr));
    rtn->terms_size = 1;
    rtn->terms_pos = 0;
    rtn->terms = malloc(rtn->terms_size * sizeof(int *));
    rtn->terms_sizes = malloc(rtn->terms_size * sizeof(size_t));

    rtn->w_term_size = 1;
    rtn->w_term_pos = 0;
    rtn->w_term = malloc(rtn->w_term_size * sizeof(int));

    return rtn;
}

void output_rules_symbol(struct output_expr *ex, int id) {
    if (++(ex->w_term_pos) >= ex->w_term_size) {
        ex->w_term_size *= 2;
        ex->w_term = realloc(ex->w_term, ex->w_term_size * sizeof(int));
    }
    ex->w_term[ex->w_term_pos - 1] = id;
}

void output_rules_endterm(struct output_expr *ex) {
    if (ex->w_term_pos <= 0)
        return;
    if (++(ex->terms_pos) >= ex->terms_size) {
        ex->terms_size *= 2;
        ex->terms = realloc(ex->terms, ex->terms_size * sizeof(int *));
        ex->terms_sizes =
            realloc(ex->terms_sizes, ex->terms_size * sizeof(size_t));
    }
    ex->terms_sizes[ex->terms_pos - 1] = ex->w_term_pos;
    ex->terms[ex->terms_pos - 1] = malloc(ex->w_term_pos * sizeof(int));
    memcpy(ex->terms[ex->terms_pos - 1], ex->w_term,
           ex->w_term_pos * sizeof(int));
    ex->w_term_pos = 0;
}

struct output_expr *output_rules_joinexpr(struct output_expr *ex1,
                                          struct output_expr *ex2) {
    if (ex1 == NULL)
        if (ex2 == NULL)
            return NULL;
        else
            return ex2;
    if (ex2 == NULL)
        return ex1;

    if ((ex1->terms_pos + ex2->terms_pos) >= ex1->terms_size) {
        ex1->terms_size += ex2->terms_pos;
        ex1->terms = realloc(ex1->terms, ex1->terms_size * sizeof(int *));
        ex1->terms_sizes =
            realloc(ex1->terms_sizes, ex1->terms_size * sizeof(size_t));
    }
    memcpy(ex1->terms + ex1->terms_pos - 1, ex2->terms,
           ex2->terms_pos * sizeof(int *));
    memcpy(ex1->terms_sizes + ex1->terms_size - 1, ex2->terms_sizes,
           ex2->terms_pos * sizeof(size_t));
    ex1->terms_pos += ex2->terms_pos;

    ex1->w_term_pos = 0;
    free(ex2->terms);
    free(ex2->terms_sizes);
    free(ex2);
    return ex1;
}

struct output_expr *output_rules_copycnf(struct output_expr *ex) {
    struct output_expr *rtn;
    rtn = malloc(sizeof(struct output_expr));
    rtn->terms_size = ex->terms_size;
    rtn->terms_pos = ex->terms_pos;
    rtn->terms_sizes = malloc(rtn->terms_size * sizeof(size_t));
    memcpy(rtn->terms_sizes, ex->terms_sizes,
           rtn->terms_size * sizeof(size_t));
    rtn->terms = malloc(rtn->terms_size * sizeof(int *));
    size_t i;
    for (i = 0; i < rtn->terms_pos; i++) {
        rtn->terms[i] = malloc(ex->terms_sizes[i] * sizeof(int));
        memcpy(rtn->terms[i], ex->terms[i],
               ex->terms_sizes[i] * sizeof(int));
    }

    ex->w_term_size = 1;
    ex->w_term_pos = 0;
    ex->w_term = malloc(ex->w_term_size * sizeof(int));
    return rtn;
}

void output_rules_freexpr(struct output_expr *ex) {
    size_t i;
    for (i = 0; i < ex->terms_pos; i++) {
        free(ex->terms[i]);
    }
    free(ex->terms);
    free(ex->terms_sizes);
    free(ex->w_term);
    free(ex);
}

void output_rules_writexpr(struct output_expr *ex) {
    size_t i, y;
    for (i = 0; i < ex->terms_pos; i++) {
        for (y = 0; y < ex->terms_sizes[i]; y++) {
            fprintf(frules, "%d ", ex->terms[i][y]);
        }
        fprintf(frules, "\n");
    }
}
