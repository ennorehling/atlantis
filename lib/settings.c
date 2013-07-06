#include "settings.h"
#include <stream.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct settings config;

static int read_entry(stream *strm, char *line, size_t size, char **key_o, char **value_o) {
    char *tok, *key;
    do {
        int err = strm->api->readln(strm->handle, line, size);
        if (err) {
            return err;
        }
        tok = strtok(line, "= ");
        while (tok[0]==0) {
            tok = strtok(0, "= ");
        }
    } while (tok[0]=='#'); /* a comment */
    key = tok;
    do {
        tok = strtok(0, "= ");
    } while (tok[0]==0);
    *key_o = key;
    *value_o = tok;
    return 0;
}

void read_config(stream *strm) {
    char line[160];
    for (;;) {
        char *key, *tok;
        int err;
        err = read_entry(strm, line, sizeof(line), &key, &tok);
        if (err==EOF) break;
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
