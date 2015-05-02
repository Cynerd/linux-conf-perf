#include "output.h"

FILE *frules, *fsymmap;

int output_init(char *rules_file, char *symbolmap_file) {
    if ((frules = fopen(rules_file, "w")) == NULL)
        return 1;
    if ((fsymmap = fopen(symbolmap_file, "w")) == NULL)
        return 2;
    return 0;
}

void output_finish(void) {
    fclose(frules);
    fclose(fsymmap);
}


// Functions for symbol_map
void output_push_symbol(unsigned id, char *name) {
    fprintf(fsymmap, "%d:%s\n", id, name);
}

// Functions for rules
void output_rules_symbol(int id) {
    fprintf(frules, "%d ", id);
}

void output_rules_endterm(void) {
    fprintf(frules, "\n");
}

// Functions for variable_count
void output_write_variable_count(char *var_file, int count) {
    FILE *f;
    f = fopen(var_file, "w");
    fprintf(f, "%d", count);
    fclose(f);
}
