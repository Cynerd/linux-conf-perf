#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include <stdlib.h>
#include <stdio.h>

#include "symlist.h"
#include <build_files.h>

int output_init(char *rules_file, char *symbolmap_file);
void output_finish(void);

// Functions for symbol_map
void output_push_symbol(int id, char *name);

// Functions for rules
struct output_expr {
    int **terms;
    size_t *terms_sizes;
    size_t terms_size, terms_pos;

    int *w_term;
    size_t w_term_size, w_term_pos;
};

struct output_expr *output_rules_newexpr(void);
void output_rules_symbol(struct output_expr *ex, int id);
void output_rules_endterm(struct output_expr *ex);
struct output_expr *output_rules_joinexpr(struct output_expr *ex1,
                                          struct output_expr *ex2);
struct output_expr *output_rules_copycnf(struct output_expr *ex);
void output_rules_freexpr(struct output_expr *ex);
void output_rules_writexpr(struct output_expr *ex);

#endif /* _OUTPUT_H_ */
