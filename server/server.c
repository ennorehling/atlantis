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
        cJSON * json;
        char buf[256];
        stream strm;

        sprintf(buf, "reports/%d-%d.json", turn, f->no);
        fstream_init(&strm, fopen(buf, "w"));
        json = json_report(f);
        json_write(json, &strm);
        cJSON_Delete(json);
        fstream_done(&strm);

        report(f);
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

region *inputregion(void)
{
    int x, y;
    region *r = 0;
    char buf[256];

    while (!r) {
        printf("X? ");
        fgets(buf, sizeof(buf), stdin);
        if (buf[0] == 0)
            return 0;
        x = atoi(buf);

        printf("Y? ");
        fgets(buf, sizeof(buf), stdin);
        if (buf[0] == 0)
            return 0;
        y = atoi(buf);

        r = findregion(x, y);

        if (!r) {
            puts("No such region.");
        }
    }
    return r;
}

void addplayers_inter(void) {
    region *r;
    FILE * F;
    char buf[512];
    stream strm;

    r = inputregion();

    if (!r) {
        return;
    }

    printf("Name of players file? ");
    fgets(buf, sizeof(buf), stdin);

    if (!buf[0]) {
        return;
    }
    F = fopen(buf, "r");
    fstream_init(&strm, F);
    addplayers(r, &strm);
    fclose(F);
}

static void readorders(const char * filename)
{
    FILE * F;
    stream strm;

    F = fopen(filename, "r");
    fstream_init(&strm, F);
    read_orders(&strm);
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

    initgame();
    if (cfgfile) {
        FILE * F = fopen(cfgfile, "r");
        if (F) {
            cJSON *json;
            char *data;
            size_t len;
            fseek(F, 0,SEEK_END);
            len = ftell(F);
            fseek(F,0,SEEK_SET);
            data = (char *)malloc(len+1);
            if (data) {
                fread(data,1,len,F);
            }
            json = cJSON_Parse(data);
            if (json) {
                read_config_json(json);
                cJSON_Delete(json);
            } else {
                fprintf(stderr, "could not parse configuration file '%s'\n", cfgfile);
            }
            free(data);
        } else {
            fprintf(stderr, "could not open configuration file '%s'\n", cfgfile);
            return errno ? errno : -1;
        }
    }
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
        case 'a':
            addplayers_inter();
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

