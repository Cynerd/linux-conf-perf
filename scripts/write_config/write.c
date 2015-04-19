#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <libintl.h>
#include <kconfig/lkc.h>
#include <macros.h>
#include <build_files.h>
#include "symlist.h"
#include "solution.h"

int verbose_level;
char *file, *folder;

int main(int argc, char **argv) {
    verbose_level = 1;
    int i;
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-v"))
            verbose_level++;
        else if (file == NULL)
            file = argv[i];
        else if (folder == NULL)
            folder = argv[i];
        else {
            Eprintf("Unknown parameter: %s\n", argv[i]);
            exit(1);
        }
    }

    if (file == NULL) {
        Eprintf("No Kconfig input file specified\n");
        exit(2);
    }
    if (folder == NULL) {
        Eprintf("No output folder specified\n");
        exit(3);
    }

    char *rules_file, *symbol_map_file, *def_config_file, *solution_file;
    asprintf(&rules_file, "%s/%s", folder, DEFAULT_RULES_FILE);
    asprintf(&symbol_map_file, "%s/%s", folder, DEFAULT_SYMBOL_MAP_FILE);
    asprintf(&def_config_file, "%s/%s", folder, DEFAULT_DEF_CONFIG_FILE);
    asprintf(&solution_file, "%s/%s", folder, DEFAULT_SOLUTION_FILE);

    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);

    conf_parse(file);
    conf_read(".config");
    conf_write(".config");

    FILE *f = fopen(symbol_map_file, "r");
    if (f == NULL) {
        Eprintf("Can't open file: %s\n", symbol_map_file);
        exit(1);
    }
    struct symlist *sl = symlist_read(f);
    fclose(f);

    f = fopen(solution_file, "r");
    if (f == NULL) {
        Eprintf("Can't open file: %s\n", solution_file);
        exit(2);
    }
    solution_set(sl, f);
    fclose(f);

    return 0;
}
