#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include <stdlib.h>
#include <stdio.h>
#include "symlist.h"

#define DEFAULT_RULES_FILE "rules"
#define DEFAULT_SYMBOL_MAP_FILE "symbol_map"

void fprint_rules(struct symlist *sl, char* output);
void fprint_symbol_map(struct symlist *sl, char* output);

#endif /* _OUTPUT_H_ */
