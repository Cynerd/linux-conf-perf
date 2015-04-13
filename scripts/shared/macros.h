extern int verbose_level; // Must be defined in project

#define Eprintf(...) fprintf(stderr, __VA_ARGS__)
#define Wprintf(...) if (verbose_level > 1) printf(__VA_ARGS__)
#define Iprintf(...) if (verbose_level > 2) printf(__VA_ARGS__)

#ifndef DEBUG
#define Dprintf(...)
#else
#define Dprintf(...) if (verbose_level > 3) printf(a, __VA_ARGS__)
#endif /* DEBUG */
