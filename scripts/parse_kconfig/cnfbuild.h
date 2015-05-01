#include <stdlib.h>
#include <stdbool.h>

#ifndef _CNFBUILD_H_
#define _CNFBUILD_H_

#include "symlist.h"
#include "boolexpr.h"
#include "output.h"

void cnf_boolexpr(struct symlist *sl, struct boolexpr *bl);

#endif /* _CNFBUILD_H_ */
