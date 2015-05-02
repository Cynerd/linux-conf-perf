#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef _DOUTPUT_H_
#define _DOUTPUT_H_

#include <kconfig/lkc.h>
#include <macros.h>
#include "boolexpr.h"

void doutput_expr(struct expr *expr);
void doutput_boolexpr(struct boolexpr *bl, struct symlist *sl);

#endif /* _DOUTPUT_H_ */
