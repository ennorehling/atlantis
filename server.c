/*
 * Atlantis v1.0 13 September 1993
 * Copyright 1993 by Russell Wallace
 *
 * This program may be freely used, modified and distributed. It may not be
 * sold or used commercially without prior written permission from the author.
 */

#include <atlantis.h>
#include <region.h>
#include <faction.h>
#include <game.h>
#include <settings.h>

#include <rtl.h>

#include "json.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>

#include <quicklist.h>
#include <stream.h>
#include <filestream.h>
#include <cJSON.h>

static void fixme() {
    if (turn==0) { /* forgot to initialize regions with money */
        ql_iter rli;
        for (rli = qli_init(&regions); qli_more(rli);) {
            region * r = (region *)qli_next(&rli);
            r->money = r->peasants * 3 / 2;
        }
    }
}

static void reports(void)
{
    FILE * F;
    ql_iter fli;

    _mkdir("reports");

    for (fli = qli_init(&factions); qli_more(fli);) {
        faction * f = (faction *)qli_next(&fli);
        write_reports(f);
    }
    F = fopen("send", "w");
    puts("Writing send file...");

    for (fli = qli_init(&factions); qli_more(fli);) {
        faction * f = (faction *)qli_next(&fli);
        const char * addr = faction_getaddr(f);
        if (addr) {
            fprintf(F, "mail %d-%d.r\n", turn, f->no);
            fprintf(F, "in%%\"%s\"\n", addr);
            fprintf(F, "Atlantis Report for %s\n", gamedate());
        }
    }

    fclose(F);

    F = fopen("maillist", "w");
    puts("Writing maillist file...");

    for (fli = qli_init(&factions); qli_more(fli);) {
        faction * f = (faction *)qli_next(&fli);
        const char * addr = faction_getaddr(f);
        if (addr) {
            fprintf(F, "%s\n", addr);
        }
    }

    fclose(F);
}

static int processturn(const char *orders)
{
    turn++;
    readorders(orders);
    processorders();
    reports();
    writesummary();
    writegame();
    return 0;
}

int main(int argc, char **argv)
{
    int i;
    char buf[12];
    const char *arg, *orders = 0, *cfgfile = 0;

    rnd_seed((unsigned int) time(0));

    puts("Atlantis v1.0 " __DATE__ "\n"
         "Copyright 1993 by Russell Wallace.\n"
         "Type ? for list of commands.");

    turn = -1;
    for (i = 1; i != argc; ++i) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
            case 'c':
                cfgfile = (argv[i][2]) ? (argv[i] + 2) : argv[++i];
                break;
            case 'i':
                ignore_password = 1;
                break;
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

    if (cfgfile) {
        int err = read_config(cfgfile);
        if (err) return err;
    }
    initgame();
    if (orders) {
        return processturn(orders);
    }

    for (;;) {
        printf("> ");
        fgets(buf, sizeof(buf), stdin);

        switch (tolower(buf[0])) {
        case 'm':
            writemap(stdout);
            break;

        case 'g':
            turn = 0;
            cleargame(false);
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
                 "G - Generate New World.\n"
                 "Q - Quit.\n"
                 "W - Write Game.\n");
        }
    }
}

