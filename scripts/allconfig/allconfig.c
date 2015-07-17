#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <stdbool.h>

#include <kconfig/lkc.h>
#include <build_files.h>
#include <macros.h>

int verbose_level;
bool full_config;

char *kconfig_file;
char *output_config_file;
char *input_config_file;

void print_help();

int main(int argc, char **argv) {
    full_config = false;
    verbose_level = 1;
    int i;
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
            print_help();
            exit(0);
        } else if (!strcmp(argv[i], "-v")) {
            verbose_level++;
        } else if (!strcmp(argv[i], "--all")) {
            full_config = true;
        } else if (kconfig_file == NULL) {
            kconfig_file = argv[i];
        } else if (input_config_file == NULL) {
            input_config_file = argv[i];
        } else if (output_config_file == NULL) {
            output_config_file = argv[i];
        } else {
            Eprintf("Unknown parameter: %s\n", argv[i]);
            exit(-1);
        }
    }

    if (output_config_file == NULL || kconfig_file == NULL
        || input_config_file == NULL) {
        Eprintf("Parameters mismatch.\n");
        print_help();
        exit(-2);
    }

    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);

    conf_parse(kconfig_file);
    conf_read(input_config_file);

    struct symbol *sym;
    sym = sym_find("MODULES");
    if (sym == NULL) {
        Eprintf("Config MODULES not found. Ignoring...\n");
    } else if (sym_get_tristate_value(sym) == yes) {
        Eprintf("Config MODULES set as yes. This is incompatible.\n");
        exit(-4);
    }

    FILE *f;
    f = fopen(output_config_file, "w");
    if (f == NULL) {
        Eprintf("Can't write to file %s\n", output_config_file);
        exit(-3);
    }

    struct property *prop;
    for_all_symbols(i, sym) {
        if ((sym->type == S_BOOLEAN || sym->type == S_TRISTATE)
            && sym->name != NULL) {
            if (!full_config) {
                for_all_prompts(sym, prop) {
                    goto printit;
                }
            } else
                goto printit;
            continue;
          printit:
            fprintf(f, "CONFIG_%s=%s\n", sym->name,
                    sym_get_tristate_value(sym) == no ? "n" : "y");
        }
    }
    fclose(f);

    return 0;
}

void print_help() {
    printf("Usage: allconfig [-v] [-h] Kconfig Input Output\n");
    printf("  This is generating full configuration.\n");
    printf("  Output configuration has all configuration options.\n");
}
