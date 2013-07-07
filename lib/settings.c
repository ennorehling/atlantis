#include "settings.h"

#include "region.h"
#include "parser.h"

#include <stream.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <cJSON.h>

struct settings config;

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
    item = cJSON_GetObjectItem(json, "terrain");
    if (item && item->type == cJSON_Array) {
        cJSON *t, *c;
        for (t=item->child;t;t=t->next) {
            if (t->type==cJSON_Object) {
                c = cJSON_GetObjectItem(t, "name");
                if (c && c->type==cJSON_String) {
                    int i;
                    for (i=0;i!=NUMTERRAINS;++i) {
                        if (strcmp(terrainnames[i], c->valuestring)==0) {
                            break;
                        }
                    }
                    if (i!=NUMTERRAINS) {
                        if ((c = cJSON_GetObjectItem(t, "work"))!=0 && c->type==cJSON_Number) {
                            foodproductivity[i] = c->valueint;
                        }
                        if ((c = cJSON_GetObjectItem(t, "food"))!=0 && c->type==cJSON_Number) {
                            maxfoodoutput[i] = c->valueint;
                        }
                        if ((c = cJSON_GetObjectItem(t, "output"))!=0 && c->type==cJSON_Array) {
                            int j;
                            for (j=0;j!=4;++j) {
                                cJSON *e = cJSON_GetArrayItem(c, j);
                                if (e && e->type==cJSON_Number) {
                                    maxoutput[i][j] = e->valueint;
                                }
                            }
                        }
                    } else {
                        fprintf(stderr, "config | invalid terrain '%s'\n", c->valuestring);
                        continue;
                    }
                }
            }
        }
    }
}
