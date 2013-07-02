#include "settings.h"
#include <stream.h>
#include <string.h>
#include <stdlib.h>

struct settings config;

void read_config(stream *strm) {
    char line[160];
    for (;;) {
        char *tok, *key;
        if (strm->api->readln(strm->handle, line, sizeof(line))!=0) {
            break;
        }
        tok = strtok(line, "= ");
        while (tok[0]==0) {
            tok = strtok(0, "= ");
        }
        if (tok[0]=='#') continue; /* a comment */
        key = tok;
        do {
            tok = strtok(0, "= ");
        } while (tok[0]==0);
        if (strcmp("width", key)==0) {
            config.width = atoi(tok);
        }
        else if (strcmp("height", key)==0) {
            config.height = atoi(tok);
        }
        else if (strcmp("stacks", key)==0 && strcmp(tok, "yes")==0) {
            config.features |= CFG_STACKS;
        }
        else if (strcmp("teachers", key)==0 && strcmp(tok, "no")!=0) {
            config.features |= CFG_TEACHERS;
        }
    }
}
