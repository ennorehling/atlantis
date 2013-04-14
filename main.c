/*
 * Atlantis v1.0 13 September 1993
 * Copyright 1993 by Russell Wallace
 *
 * This program may be freely used, modified and distributed. It may not be
 * sold or used commercially without prior written permission from the author.
 */

#include "atlantis.h"
#include "region.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

static void fixme() {
    region * r;
    if (turn==0) { /* forgot to initialize regions with money */
        for (r=regions;r;r=r->next) {
            r->money = r->peasants * 3 / 2;
        }
    }
}

int main(int argc, char **argv)
{
    int i;
    char buf[12];
    const char * arg, * orders = 0;

    rnd_seed((unsigned int) time(0));

    puts("Atlantis v1.0 " __DATE__ "\n"
         "Copyright 1993 by Russell Wallace.\n"
         "Type ? for list of commands.");

    turn = -1;
    for (i = 1; i != argc; ++i) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
            case 'p': /* process */
                orders = (argv[i][2]) ? (argv[i] + 2) : argv[++i];
                break;
            case 't': /* turn */
                arg = (argv[i][2]) ? (argv[i] + 2) : argv[++i];
                turn = atoi(arg);
                break;
            default:
                fprintf(stderr, "invalid argument %d: '%s'\n", i, argv[i]);
                return -1;
            }
        }
    }

    initgame();
    if (orders) {
        return processturn(orders);
    }

    for (;;) {
        printf("> ");
        fgets(buf, sizeof(buf), stdin);

        switch (tolower(buf[0])) {
        case 'c':
            createcontinent();
            break;

        case 'm':
            writemap(stdout);
            break;
            
        case 'a':
            turn = 0;
            cleargame();
            autoworld("players");
            writemap(stdout);
            break;

        case 'r':
            reports();
            break;

        case 'f':
            fixme();
            break;

        case 'w':
            makeworld();
            writesummary();
            writegame();
            break;

        case 'p':
            printf("Name of orders file? ");
            fgets(buf, sizeof(buf), stdin);
            if (!buf[0]) return -1;
            return processturn(buf);

        case 'q':
            return 0;

        default:
            puts("C - Create New Continent.\n"
                 "A - Add New Players.\n"
                 "M - Draw Map.\n"
                 "P - Process Game Turn.\n"
                 "R - Write Reports.\n"
                 "Q - Quit.\n"
                 "W - Write Game.\n");
        }
    }
}

