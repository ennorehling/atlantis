#include "rtl.h"
#include "atlantis.h"
#include "faction.h"
#include "region.h"
#include "building.h"
#include "ship.h"
#include "unit.h"
#include "storage/stream.h"
#include <cJSON.h>
#include <string.h>
#include <stdlib.h>


static cJSON * show_strlist(const strlist *slist) {
    cJSON *json, *chld;
    json = cJSON_CreateArray();
    while (slist) {
        cJSON_AddItemToArray(json, cJSON_CreateString(slist->s));
        slist = slist->next;
    }
    return json;
}

static cJSON * show_ship(const faction *f, const region * r, const ship * s) {
    cJSON *json;
    const char * str;
    
    json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "id", s->no);
    cJSON_AddStringToObject(json, "type", shiptypenames[s->type]);
    if ((str = ship_getname(s))!=0) {
        cJSON_AddStringToObject(json, "name", str);
    }
    if ((str = ship_getdisplay(s))!=0) {
        cJSON_AddStringToObject(json, "display", str);
    }
    return json;
}

static cJSON * show_unit(const faction *f, const region * r, const unit * u) {
    cJSON *json;
    const char * str;

    json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "id", u->no);
    if (cansee(f, r, u) == 2) {
        cJSON_AddNumberToObject(json, "faction", f->no);
    }
    if ((str = unit_getname(u))!=0) {
        cJSON_AddStringToObject(json, "name", str);
    }
    if ((str = unit_getdisplay(u))!=0) {
        cJSON_AddStringToObject(json, "display", str);
    }
    cJSON_AddNumberToObject(json, "number", u->number);
    if (u->ship) {
        cJSON_AddNumberToObject(json, "ship", u->ship->no);
    }
    if (u->building) {
        cJSON_AddNumberToObject(json, "building", u->building->no);
    }
    if (f==u->faction) {
        cJSON_AddStringToObject(json, "default", u->lastorder);
        cJSON_AddNumberToObject(json, "money", u->money);
        if (u->behind) {
            cJSON_AddTrueToObject(json, "behind");
        }
        if (u->guard) {
            cJSON_AddTrueToObject(json, "guard");
        }
    }
    return json;
}

static cJSON * show_exit(const faction *f, const region * r, int d) {
    char buf[16];
    cJSON *json;
    region * rn = r->connect[d];

    json = cJSON_CreateObject();
    strncpy(buf, keywords[directions[d]], sizeof(buf));
    cJSON_AddStringToObject(json, "direction", rtl_strlwr(buf));
    cJSON_AddStringToObject(json, "terrain", terrainnames[rn->terrain]);
    return json;
}

static cJSON * show_region(const faction *f, const region * r) {
    cJSON *json, *chld;
    int x, y, d;
    unit * u;
    ship *s;
    const char * str;
    
    x = (r->x - f->origin_x + world.width) % world.width;
    y = (r->y - f->origin_y + world.height) % world.height;
    json = cJSON_CreateObject();
    if (r->terrain != T_OCEAN) {
        if ((str = region_getname(r))!=0) {
            cJSON_AddStringToObject(json, "name", str);
        }
    }
    cJSON_AddStringToObject(json, "terrain", terrainnames[r->terrain]);
    cJSON_AddNumberToObject(json, "x", x);
    cJSON_AddNumberToObject(json, "y", y);
    if (r->peasants) {
        cJSON_AddNumberToObject(json, "peasants", r->peasants);
        cJSON_AddNumberToObject(json, "money", r->money);
    }
    cJSON_AddItemToObject(json, "exits", chld = cJSON_CreateArray());
    for (d=0;d!=MAXDIRECTIONS;++d) {
        if (r->connect[d]) {
            cJSON_AddItemToArray(chld, show_exit(f, r, d));
        }
    }
    if (r->ships) {
        cJSON_AddItemToObject(json, "ships", chld = cJSON_CreateArray());
        for (s=r->ships;s;s=s->next) {
            cJSON_AddItemToArray(chld, show_ship(f, r, s));
        }
    }
    cJSON_AddItemToObject(json, "units", chld = cJSON_CreateArray());
    for (u=r->units;u;u=u->next) {
        cJSON_AddItemToArray(chld, show_unit(f, r, u));
    }
    return json;
}

cJSON * json_report(const faction * f) {
    cJSON *json, *chld;
    region *r;
    
    json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "turn", turn);
    cJSON_AddItemToObject(json, "faction", chld = cJSON_CreateObject());
    cJSON_AddNumberToObject(chld, "id", f->no);
    cJSON_AddStringToObject(chld, "name", faction_getname(f));

    if (f->mistakes) {
        cJSON_AddItemToObject(json, "mistakes", show_strlist(f->mistakes));
    }
    if (f->messages) {
        cJSON_AddItemToObject(json, "messages", show_strlist(f->messages));
    }
    if (f->battles) {
        cJSON_AddItemToObject(json, "battles", show_strlist(f->battles));
    }
    if (f->events) {
        cJSON_AddItemToObject(json, "events", show_strlist(f->events));
    }
    /* spells */
    if (f->allies) {
        rfaction * rf;
        cJSON_AddItemToObject(json, "allies", chld = cJSON_CreateArray());
        for (rf = f->allies; rf; rf = rf->next) {
            cJSON_AddItemToArray(json, cJSON_CreateNumber(rf->faction->no));
        }
    }
    cJSON_AddItemToObject(json, "regions", chld = cJSON_CreateArray());
    
    for (r = regions; r; r = r->next) {
        unit * u;

        for (u = r->units; u; u = u->next) {
            if (u->faction == f) {
                cJSON_AddItemToArray(chld, show_region(f, r));
                break;
            }
        }
    }
    return json;
}

void json_write(cJSON * json, stream * out) {
    char *tok, *rendered = cJSON_Print(json);
    
    tok = strtok(rendered, "\n\r");
    while (tok) {
        if (tok[0]) {
            out->api->writeln(out->handle, tok);
        }
        tok = strtok(NULL, "\n\r");
    }
    free(rendered);
}
