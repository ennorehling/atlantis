/*
 * Atlantis v1.0 13 September 1993
 * Copyright 1993 by Russell Wallace
 *
 * This program may be freely used, modified and distributed. It may not be
 * sold or used commercially without prior written permission from the author.
 */

#include <atlantis.h>
#include <game.h>
#include <rtl.h>

#include <cJSON.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>

int main(int argc, char **argv)
{
    int i;
    char buf[64];
    const char *cfgfile = 0;
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
            default:
                fprintf(stderr, "invalid argument %d: '%s'\n", i, argv[i]);
                return -1;
            }
        }
    }

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
    initgame();

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

        case 'w':
            writesummary();
            writegame();
            break;

        case 'q':
            return 0;

        default:
            puts("M - Draw Map.\n"
                 "G - Generate New World.\n"
                 "Q - Quit.\n"
                 "W - Write Game.\n");
        }
    }
    return 0;
}
