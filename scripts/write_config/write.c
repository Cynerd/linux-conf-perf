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

int exit_status;

int main(int argc, char **argv) {
    exit_status = 0;
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

    char *rules_file, *symbol_map_file, *def_config_file, *config_map, *config_solved;
    asprintf(&rules_file, "%s/%s", folder, DEFAULT_RULES_FILE);
    asprintf(&symbol_map_file, "%s/%s", folder, DEFAULT_SYMBOL_MAP_FILE);
    asprintf(&def_config_file, "%s/%s", folder, DEFAULT_DEF_CONFIG_FILE);
    asprintf(&config_map, "%s/%s", folder, DEFAULT_CONFIG_MAP_FILE);
    asprintf(&config_solved, "%s/%s", folder, DEFAULT_CONFIG_SOLVED_FILE);

    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);

    conf_parse(file);
    struct symbol *sym;
    //conf_read(def_config_file);
    conf_read(".config");

    FILE *f = fopen(symbol_map_file, "r");
    if (f == NULL) {
        Eprintf("Can't open file: %s\n", symbol_map_file);
        exit(-1);
    }
    struct symlist *sl = symlist_read(f);
    fclose(f);

    FILE *fconfig_map = fopen(config_map, "r");
    if (fconfig_map == NULL) {
        Eprintf("Can't open file: %s\n", config_map);
        exit(-2);
    }
    FILE *fconfig_solved = fopen(config_solved, "r");
    if (fconfig_map == NULL) {
        Eprintf("Can't open file: %s\n", config_solved);
        exit(-3);
    }
    //struct solution *sol = solution_load(fconfig_map, fconfig_solved);
    //solution_check(sl, sol);
    fclose(fconfig_map);
    fclose(fconfig_solved);

    conf_write(".config");

    return exit_status;
}
