#include "settings.h"

#include "region.h"
#include "ship.h"
#include "parser.h"

#include <stream.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <cJSON.h>

struct settings config;

void read_config_json(cJSON *json) {
    cJSON *item;
    
    item = cJSON_GetObjectItem(json, "startmoney");
    if (item && item->type == cJSON_Number) {
        config.startmoney = item->valueint;
    }
    item = cJSON_GetObjectItem(json, "startmen");
    if (item && item->type == cJSON_Number) {
        config.startmen = item->valueint;
    }

    item = cJSON_GetObjectItem(json, "width");
    if (item && item->type == cJSON_Number) {
        config.width = item->valueint;
    }
    item = cJSON_GetObjectItem(json, "height");
    if (item && item->type == cJSON_Number) {
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
    item = cJSON_GetObjectItem(json, "moves");
    if (item && item->type == cJSON_Number) {
        config.moves = item->valueint;
        if (config.moves>0) {
            config.features |= CFG_MOVES;
        }
    }
    item = cJSON_GetObjectItem(json, "upkeep");
    if (item && item->type == cJSON_Number) {
        config.upkeep = item->valueint;
    }
    item = cJSON_GetObjectItem(json, "ships");
    if (item && item->type == cJSON_Array) {
        cJSON *j, *c;
        for (j=item->child;j;j=j->next) {
            if (j->type==cJSON_Object) {
                c = cJSON_GetObjectItem(j, "name");
                if (c && c->type==cJSON_String) {
                    ship_type *stype = get_shiptype_by_name(c->valuestring);
                    if (!stype) {
                        stype = create_shiptype(c->valuestring);
                    }
                    if ((c = cJSON_GetObjectItem(j, "capacity"))!=0 && c->type==cJSON_Number) {
                        stype->capacity = c->valueint;
                    }
                    if ((c = cJSON_GetObjectItem(j, "cost"))!=0 && c->type==cJSON_Number) {
                        stype->cost = c->valueint;
                    }
                    if ((c = cJSON_GetObjectItem(j, "speed"))!=0 && c->type==cJSON_Number) {
                        stype->speed = c->valueint;
                    }
                }
            }
        }
    }
    item = cJSON_GetObjectItem(json, "terrain");
    if (item && item->type == cJSON_Array) {
        cJSON *j, *c;
        for (j=item->child;j;j=j->next) {
            if (j->type==cJSON_Object) {
                c = cJSON_GetObjectItem(j, "name");
                if (c && c->type==cJSON_String) {
                    terrain *t = get_terrain_by_name(c->valuestring);
                    if (!t) {
                        t = create_terrain(c->valuestring);
                    }
                    if (t) {
                        if ((c = cJSON_GetObjectItem(j, "work"))!=0 && c->type==cJSON_Number) {
                            t->foodproductivity = c->valueint;
                        }
                        if ((c = cJSON_GetObjectItem(j, "food"))!=0 && c->type==cJSON_Number) {
                            t->maxfoodoutput = c->valueint;
                        }
                        if ((c = cJSON_GetObjectItem(j, "output"))!=0 && c->type==cJSON_Array) {
                            int k;
                            for (k=0;k!=4;++k) {
                                cJSON *e = cJSON_GetArrayItem(c, k);
                                if (e && e->type==cJSON_Number) {
                                    t->maxoutput[k] = e->valueint;
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
