#include "solution.h"

extern int exit_status;

struct solution *solution_load(FILE * fmap, FILE * fsolved) {
    char buffer[BUFFER_SIZE];
    size_t buff_pos;

    int c;
    buff_pos = 0;
    size_t buff_pos_old;
    while (1) {
        c = fgetc(fsolved);
        if (c == '\n') {
            buff_pos_old = buff_pos;
            buff_pos = 0;
        } else if (c == EOF) {
            break;
        } else {
            buffer[buff_pos++] = (char) c;
        }
    }
    buffer[buff_pos_old++] = '\0';
    char *hash;
    hash = malloc(buff_pos_old * sizeof(char));
    memcpy(hash, buffer, buff_pos_old * sizeof(char));

    while (1) {
        do {
            c = fgetc(fmap);
            buffer[buff_pos++] = (char) c;
        } while (c != ':');
        buffer[buff_pos] = '\0';
        if (!strcmp(buffer, hash))
            break;
        do {
            c = fgetc(fmap);
        } while (c != '\n');
    }

    size_t sz = 2;
    struct solution *sol;
    sol = malloc(sizeof(struct solution));
    sol->sol = malloc(sz * sizeof(int));
    sol->size = 0;
    while (1) {
        c = fgetc(fmap);
        if (c == ' ' || c == '\n') {
            buffer[buff_pos] = '\0';
            if (sol->size >= sz) {
                sz *= 2;
                sol->sol = realloc(sol->sol, sz * sizeof(int));
            }
            sol->sol[sol->size++] = atoi(buffer);
        } else {
            buffer[buff_pos++] = (char) c;
        }
        if (c == '\n')
            break;
    }

    return sol;
}

void solution_check(struct symlist *sl, struct solution *s) {
    unsigned i;
    for (i = 0; i < s->size; s++) {
        bool neg = false;
        if (s->sol[i] < 0) {
            neg = true;
            s->sol[i] *= -1;
        }
        if ((unsigned) s->sol[i] > sl->maxid)
            break;
        if (s->sol[i] == 0)
            continue;
        if (sl->array[s->sol[i] - 1].sym == NULL)
            continue;
        if (neg ==
            (sym_get_tristate_value(sl->array[s->sol[i] - 1].sym) ==
             no ? true : false)) {
        } else {
            printf("Problem %s=%d/%d\n",
                   sl->array[s->sol[i] - 1].sym->name, !neg,
                   sym_get_tristate_value(sl->array[s->sol[i] - 1].sym));
            exit_status++;
        }
    }
}
