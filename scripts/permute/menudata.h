#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include <kconfig/lkc.h>

#ifndef _MENUDATA_H_
#define _MENUDATA_H_

struct menudata {
    bool permute;
    bool subpermute;
};

struct menudata *menudata_new(void);
void menudata_set_permute(struct menu *m, bool perm);
void menudata_cal(struct menu *m);

#endif /* _MENUDATA_H_ */
