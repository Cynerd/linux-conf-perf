#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <libintl.h>
#include <string.h>

#include <kconfig/lkc.h>
#include <macros.h>
#include <build_files.h>
#include "solution.h"

const char defkconfig_file[] = "Kconfig";
const char defoutput_config[] = ".config";

int verbose_level;
char *kconfig_file;
char *input_config;

void print_help();

int exit_status;

int main(int argc, char **argv) {
    exit_status = 0;
    verbose_level = 1;
    kconfig_file = (char*)defkconfig_file;
    int i;
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
            print_help();
            exit(0);
        } else if (!strcmp(argv[i], "-v")) {
            verbose_level++;
        } else if (input_config == NULL) {
            input_config = argv[i];
        } else {
            Eprintf("Unknown parameter: %s\n", argv[i]);
            exit(-1);
        }
    }

    if (input_config == NULL) {
        Eprintf("No input config specified.");
        print_help();
        exit(2);
    }

    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);

    conf_parse(kconfig_file);
    conf_read(input_config);

    // TODO configuration check disabled. It is not compatible after changes.

    conf_write(defoutput_config);

    return exit_status;
}

void print_help() {
    printf("Usage: write_config [-v] [-h] Input\n");
    printf("  This applies configuration to Linux.");
}
