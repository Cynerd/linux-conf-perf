#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <libintl.h>

#include <kconfig/lkc.h>
#include <macros.h>
#include <build_files.h>

#define INPUT_SIZE 1024

int verbose_level;
char *file, *folder;

void printf_help();

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

    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);

    conf_parse(file);
    conf_read(".config");

    struct menu *wroot, *wmenu, *wwmenu;
    wroot = &rootmenu;
    unsigned menucount;
    char *input;
    int inputi;
    input = malloc(INPUT_SIZE * sizeof(char));

    printf_help();

    while (1) {
        printf("\n%s\n", wroot->prompt->text);
        wmenu = wroot->list;
        menucount = 0;
        while (wmenu != NULL) {
            if (wmenu->prompt != NULL) {
                if (wmenu->list == NULL)
                    printf("%3d<X>: %s\n", ++menucount,
                           wmenu->prompt->text);
                else
                    printf("%3d<X>: %s -->\n", ++menucount,
                           wmenu->prompt->text);
            }
            wmenu = wmenu->next;
        }

      input:
        printf("Input: ");
        fgets(input, INPUT_SIZE, stdin);
        switch (input[0]) {
        case 'e':
        case 'v':
        case 'f':
            inputi = atoi(input + 1);
            if (inputi <= 0 && inputi > menucount)
                goto input;
            int y = 0;
            wwmenu = wroot->list;
            while (1) {
                if (wwmenu->prompt != NULL)
                    y++;
                if (y >= inputi)
                    break;
                wwmenu = wwmenu->next;
            }
            break;
        case 'u':
            wmenu = wmenu->parent;
            break;
        case 'r':
            break;
        case 'q':
            goto quit;
        case 'h':
            printf_help();
        default:
            goto input;
        }
        switch (input[0]) {
        case 'e':
            wroot = wwmenu;
            break;
        case 'v':
            break;
        case 'f':
            break;
        }
    }

  quit:

    return 0;
}

void printf_help() {
    printf("As input are accepted these commands:\n");
    printf("  e <NUM> Enter menu according to number.\n");
    printf("  u       Go to previous upper menu.\n");
    printf("  v <NUM> Set menu and all its submenus as variable.\n");
    printf("  f <NUM> Set menu and all its submenus as fixed.\n");
    printf("  r       Reprint menu.\n");
    printf("  h       Prints this text.\n");
    printf("  q       Quit this program\n");
}
