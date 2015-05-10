#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <kconfig/lkc.h>
#include "menudata.h"

#ifndef _DOTCONF_H_
#define _DOTCONF_H_

#define DOTCONFIG_FILE "../dot_config"
#define READBUFFER_SIZE 128

bool dotconfig_read(void);
void dotconfig_write(void);

#endif /* _DOTCONF_H_ */
