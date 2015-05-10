#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <locale.h>
#include <libintl.h>
#include <kconfig/lkc.h>
#include <macros.h>
#include <build_files.h>
#include "menudata.h"
#include "dotconf.h"
#define INPUT_SIZE 1024
int verbose_level;
char *file;

bool reqsave;

void printf_help(void);
void exit_save(void);

int main(int argc, char **argv) {
    verbose_level = 1;
    int i;
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-v"))
            verbose_level++;
        else if (file == NULL)
            file = argv[i];
        else {
            Eprintf("Unknown parameter: %s\n", argv[i]);
            exit(1);
        }
    }

    if (file == NULL) {
        Eprintf("No Kconfig input file specified\n");
        exit(2);
    }

    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);

    conf_parse(file);
    conf_read(".config");

    if (!dotconfig_read())
        reqsave = true;
    else
        reqsave = false;

    struct menu *wroot, *wmenu, *wwmenu;
    wroot = &rootmenu;
    int menucount;
    char *input;
    int inputi;
    input = malloc(INPUT_SIZE * sizeof(char));

    printf_help();

    rootmenu.data = menudata_new();
    while (1) {
        printf("\n%s\n", wroot->prompt->text);
        wmenu = wroot->list;
        menucount = 0;
        while (wmenu != NULL) {
            if (wmenu->prompt != NULL
                && (wmenu->sym == NULL || wmenu->sym->type == S_BOOLEAN
                    || wmenu->sym->type == S_TRISTATE
                    || wmenu->sym->type == S_OTHER)) {
                if (wmenu->data == NULL)
                    wmenu->data = menudata_new();
                printf("%3d", ++menucount);
                if (((struct menudata *) wmenu->data)->permute) {
                    printf("<O>");
                } else if (((struct menudata *) wmenu->data)->subpermute) {
                    printf("<->");
                } else {
                    printf("<X>");
                }
                if (wmenu->sym == NULL || sym_is_choice(wmenu->sym))
                    printf(" %s -->\n", wmenu->prompt->text);
                else
                    printf(" %s\n", wmenu->prompt->text);
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
                if (wwmenu->prompt != NULL
                    && (wwmenu->sym == NULL
                        || wwmenu->sym->type == S_BOOLEAN
                        || wwmenu->sym->type == S_TRISTATE
                        || wwmenu->sym->type == S_OTHER))
                    y++;
                if (y >= inputi)
                    break;
                wwmenu = wwmenu->next;
            }
            break;
        case 'u':
            if (wroot->parent == NULL)
                goto input;
            wroot = wroot->parent;
            break;
        case 's':
            reqsave = false;
            dotconfig_write();
            printf("Configuration saved...\n");
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
            if (wwmenu->list != NULL)
                wroot = wwmenu;
            else
                goto input;
            break;
        case 'v':
            menudata_set_permute(wwmenu, true);
            reqsave = true;
            break;
        case 'f':
            menudata_set_permute(wwmenu, false);
            reqsave = true;
            break;
        }
    }

  quit:
    exit_save();

    return 0;
}

void printf_help(void) {
    printf("As input are accepted these commands:\n");
    printf("  e <NUM> Enter menu according to number.\n");
    printf("  u       Go to previous upper menu.\n");
    printf("  v <NUM> Set menu and all its submenus as variable.\n");
    printf("  f <NUM> Set menu and all its submenus as fixed.\n");
    printf("  s       Save settings.\n");
    printf("  r       Reprint menu.\n");
    printf("  h       Prints this text.\n");
    printf("  q       Quit this program\n");
}

void exit_save(void) {
    if (!reqsave)
        return;
    printf("Unsaved chages. Save (y/N): ");
    int ch = fgetc(stdin);
    if (ch == 'y' || ch == 'Y') {
        dotconfig_write();
        printf("Configuration saved.\n");
    }
}
