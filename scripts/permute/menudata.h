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

bool menudata_cal_subpermute(struct menu *m);

#endif /* _MENUDATA_H_ */
