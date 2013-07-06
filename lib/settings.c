#include "settings.h"
#include <stream.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <cJSON.h>

struct settings config;

static int read_item(stream *strm, char *line, size_t size, char **key_o, char **value_o) {
    char *tok;
    do {
        int err = strm->api->readln(strm->handle, line, size);
        if (err) {
            return err;
        }
        tok = strtok(line, "= ");
        while (tok[0]==0) {
            tok = strtok(0, "= ");
        }
        if (tok[0]=='[') {
            *key_o = tok+1;
            tok = strtok(0, "]");
            *value_o = 0;
            return 0;
        }
    } while (tok[0]=='#'); /* a comment */
    *key_o = tok;
    do {
        tok = strtok(0, "= ");
    } while (tok[0]==0);
    *value_o = tok;
    return 0;
}

void read_config(stream *strm) {
    char line[160];
    for (;;) {
        char *key, *tok;
        int err;
        err = read_item(strm, line, sizeof(line), &key, &tok);
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

void read_config_json(cJSON *json) {
    cJSON *item;
    
    item = cJSON_GetObjectItem(json, "width");
    if (item) {
        config.width = item->valueint;
    }
    item = cJSON_GetObjectItem(json, "height");
    if (item) {
        config.height = item->valueint;
    }
    item = cJSON_GetObjectItem(json, "stacks");
    if (item && item->type==cJSON_True) {
        config.features |= CFG_STACKS;
    }
    item = cJSON_GetObjectItem(json, "teachers");
    if (item && item->type == cJSON_True) {
        config.features |= CFG_TEACHERS;
    }
}
