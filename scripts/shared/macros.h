extern int verbose_level; // Defined in kconfig_parser.c

#define Eprintf(...) fprintf(stderr, __VA_ARGS__)
#define Wprintf(...) if (verbose_level > 1) printf(__VA_ARGS__)
#define Iprintf(...) if (verbose_level > 2) printf(__VA_ARGS__)

#ifndef DEBUG
#define Dprintf(...)
#else
#define Dprintf(...) if (verbose_level > 3) printf(a, __VA_ARGS__)
#endif /* DEBUG */
