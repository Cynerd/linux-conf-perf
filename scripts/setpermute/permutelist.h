#include <stdlib.h>
#include <stdbool.h>
#include <kconfig/lkc.h>

#ifndef _PERMUTELIST_H_
#define _PERMUTELIST_H_

struct permutelist {
    struct menu *permute;
    size_t size, pos;
};

void permutelist_add(struct menu *m);
void permutelist_remove(struct menu *m);
bool permutelist_is_permute(struct menu *m);

#endif /* _PERMUTELIST_H_ */
