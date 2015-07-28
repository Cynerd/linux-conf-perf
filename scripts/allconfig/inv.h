#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <macros.h>
#include <kconfig/lkc.h>

#ifndef _INV_H_
#define _INV_H_

#define READBUFFER_SIZE 127
#define LINENUM_IDENTIFICATOR -10

void inv_prepare(char *input_file);
bool inv_fixed(struct symbol *sym);

#endif /* _INV_H_ */
