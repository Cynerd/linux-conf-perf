#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <kconfig/lkc.h>
#include <build_files.h>

#ifndef _SOLUTION_H_
#define _SOLUTION_H_

#define BUFFER_SIZE 32

struct solution {
    int *sol;
    size_t size;
};

struct solution *solution_load(char *source_config);
int solution_check(struct solution *s);

#endif /* _SOLUTION_H_ */
