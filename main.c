/*
 * Atlantis v1.0 13 September 1993
 * Copyright 1993 by Russell Wallace
 *
 * This program may be freely used, modified and distributed. It may not be
 * sold or used commercially without prior written permission from the author.
 */

#include "atlantis.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

int main(int argc, char **argv)
{
    int i;
    char buf[12];
    rnd_seed((unsigned int) time(0));

    puts("Atlantis v1.0 " __DATE__ "\n"
         "Copyright 1993 by Russell Wallace.\n"
         "Type ? for list of commands.");

    turn = -1;
    for (i = 1; i != argc; ++i) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
            case 'p': /* process */
                processturn();
                break;
            case 't': /* turn */
                if (argv[i][2]) {
                    turn = atoi(argv[i] + 3);
                    break;
                } else if (i + 1 < argc) {
                    turn = atoi(argv[++i]);
                    break;
                }
            default:
                fprintf(stderr, "invalid argument %d: '%s'\n", i, argv[i]);
                return -1;
            }
        }
    }

    initgame();

    for (;;) {
        printf("> ");
        fgets(buf, sizeof(buf), stdin);

        switch (tolower(buf[0])) {
        case 'c':
            createcontinent();
            break;

        case 'a':
            addplayers();
            break;

        case 'u':
            addunits();
            break;

        case 'p':
            processturn();
            return 0;

        case 'q':
            return 0;

        default:
            puts("C - Create New Continent.\n"
                 "A - Add New Players.\n"
                 "U - Add New Units.\n"
                 "P - Process Game Turn.\n" "Q - Quit.");
        }
    }
}
