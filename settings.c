#include "settings.h"

#include "region.h"
#include "ship.h"
#include "parser.h"

#include "rtl.h"

#include <stream.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <cJSON.h>

static struct settings config_;

struct settings * get_config(void) {
    return &config_;
}

void read_config_json(cJSON *json) {
    cJSON *item;
    
    item = cJSON_GetObjectItem(json, "directions");
    if (item && item->type == cJSON_Array) {
      int k;
      for (k = 0; k != MAXDIRECTIONS; ++k) {
        cJSON *e = cJSON_GetArrayItem(item, k);
        if (e) {
          if (e->type == cJSON_String) {
            config_.directions[k] = (char **)calloc(2, sizeof(char *));
            config_.directions[k][0] = _strdup(e->valuestring);
          }
          else if (e->type == cJSON_Array) {
            cJSON *c;
            int j = 0;
            config_.directions[k] = (char **)calloc(1 + cJSON_GetArraySize(e), sizeof(char *));
            for (c=e->child; c; c=c->next) {
              if (c->type == cJSON_String) {
                config_.directions[k][j++] = _strdup(c->valuestring);
              }
            }
          }
        }
      }
    }
    item = cJSON_GetObjectItem(json, "coordinates");
    if (item && item->type == cJSON_String) {
        if (strcmp(item->valuestring, "torus")==0) {
            config_.transform = COOR_TORUS;
        }
        else if (strcmp(item->valuestring, "alh")==0) {
            config_.transform = COOR_ALH;
        }
        else if (strcmp(item->valuestring, "eressea")==0) {
            config_.transform = COOR_ERESSEA;
        } else {
            config_.transform = COOR_NONE;
        }
    }

    item = cJSON_GetObjectItem(json, "startmoney");
    if (item && item->type == cJSON_Number) {
        config_.startmoney = item->valueint;
    }
    item = cJSON_GetObjectItem(json, "startmen");
    if (item && item->type == cJSON_Number) {
        config_.startmen = item->valueint;
    }

    item = cJSON_GetObjectItem(json, "width");
    if (item && item->type == cJSON_Number) {
        config_.width = item->valueint;
    }
    item = cJSON_GetObjectItem(json, "height");
    if (item && item->type == cJSON_Number) {
        config_.height = item->valueint;
    }
    item = cJSON_GetObjectItem(json, "stacks");
    if (item && item->type==cJSON_True) {
        config_.features |= CFG_STACKS;
    }
    item = cJSON_GetObjectItem(json, "moves");
    if (item && item->type == cJSON_Number) {
        config_.moves = item->valueint;
        if (config_.moves>0) {
            config_.features |= CFG_MOVES;
        }
    }
    item = cJSON_GetObjectItem(json, "upkeep");
    if (item && item->type == cJSON_Number) {
        config_.upkeep = item->valueint;
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
                        fprintf(stderr, "config_ | invalid terrain '%s'\n", c->valuestring);
                        continue;
                    }
                }
            }
        }
    }
}

int read_config(const char *cfgfile) {
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
    return 0;
}
