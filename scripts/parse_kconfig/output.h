#include <stdlib.h>
#include <stdio.h>

#include <build_files.h>

#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include "symlist.h"

int output_init(char *rules_file, char *symbolmap_file);
void output_finish(void);

// Functions for symbol_map
void output_push_symbol(unsigned id, char *name);

// Functions for rules
void output_rules_symbol(int id);
void output_rules_endterm(void);

// Functions for variable_count
void output_write_variable_count(char *var_file, int count);
#endif /* _OUTPUT_H_ */
