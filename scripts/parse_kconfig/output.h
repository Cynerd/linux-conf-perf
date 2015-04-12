#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include <stdlib.h>
#include <stdio.h>
#include "symlist.h"
#include <build_files.h>

void fprint_rules(struct symlist *sl, char *output);
void fprint_symbol_map(struct symlist *sl, char *output);

#endif /* _OUTPUT_H_ */
