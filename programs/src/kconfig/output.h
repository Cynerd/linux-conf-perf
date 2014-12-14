#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include <stdlib.h>
#include <stdio.h>
#include "symlist.h"

#define RULES_FILE "rules"
#define LINKER_FILE "linker"

void fprint_rules(struct symlist *sl);
void fprint_linker(struct symlist *sl);

#endif /* _OUTPUT_H_ */
