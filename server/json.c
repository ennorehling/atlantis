#include "rtl.h"
#include "atlantis.h"
#include "faction.h"
#include "game.h"
#include "battle.h"
#include "region.h"
#include "building.h"
#include "ship.h"
#include "unit.h"
#include "items.h"
#include "keywords.h"
#include "spells.h"
#include "parser.h"

#include <stream.h>
#include <quicklist.h>

#include <cJSON.h>
#include <string.h>
#include <stdlib.h>


static cJSON * show_qstrlist(quicklist *ql) {
    ql_iter qli;
    cJSON *json;
    json = cJSON_CreateArray();
    for (qli=qli_init(&ql); qli_more(qli);) {
        char * s = (char *)qli_next(&qli);
        cJSON_AddItemToArray(json, cJSON_CreateString(s));
    }
    return json;
}

static cJSON * show_ship(const faction *f, const ship * s) {
    cJSON *json;
    const char * str;
    
    DBG_UNREFERENCED_PARAMETER(f);
    json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "id", s->no);
    cJSON_AddStringToObject(json, "type", shiptypenames[s->type]);
    if ((str = ship_getname(s))!=0) {
        cJSON_AddStringToObject(json, "name", str);
    }
    if ((str = ship_getdisplay(s))!=0) {
        cJSON_AddStringToObject(json, "display", str);
    }
    if (s->left) {
        cJSON_AddFalseToObject(json, "finished");
    } else {
        cJSON_AddTrueToObject(json, "finished");
    }
    return json;
}

static cJSON * show_building(const faction *f, const building * b) {
    cJSON *json;
    const char * str;
    
    DBG_UNREFERENCED_PARAMETER(f);
    json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "id", b->no);
    cJSON_AddNumberToObject(json, "size", b->size);
    if ((str = building_getname(b))!=0) {
        cJSON_AddStringToObject(json, "name", str);
    }
    if ((str = building_getdisplay(b))!=0) {
        cJSON_AddStringToObject(json, "display", str);
    }
    return json;
}

static cJSON * show_skill(const unit * u, skill_t sk) {
    cJSON *json;
    
    json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "skill", skillnames[sk]);
    cJSON_AddNumberToObject(json, "level", effskill(u, sk));
    cJSON_AddNumberToObject(json, "total", u->skills[sk]);
    return json;
}

static cJSON * show_item(const unit * u, int i) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "name", itemnames[i][0]);
    cJSON_AddNumberToObject(json, "count", u->items[i]);
    return json;
}

static cJSON * show_unit(const faction *f, region * r, const unit * u) {
    cJSON *json, *chld;
    const char * str;
    int i;

    json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "id", u->no);
    if (cansee(f, r, u) == 2) {
        cJSON_AddNumberToObject(json, "faction", u->faction->no);
    }
    if (u->stack) {
        cJSON_AddNumberToObject(json, "stack", u->stack->no);
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
        chld = 0;
        for (i = 0; i != MAXSKILLS; i++) {
            if (u->skills[i]) {
                if (!chld) {
                    cJSON_AddItemToObject(json, "skills", chld = cJSON_CreateArray());
                }
                cJSON_AddItemToArray(chld, show_skill(u, (skill_t)i));
            }
        }
        chld = 0;
        for (i = 0; i != MAXSPELLS; i++) {
            if (u->spells[i]) {
                if (!chld) {
                    cJSON_AddItemToObject(json, "spells", chld = cJSON_CreateArray());
                }
                cJSON_AddItemToArray(chld, cJSON_CreateString(spellnames[i]));
            }
        }
        if (u->combatspell >= 0) {
            cJSON_AddItemToObject(json, "combatspell", cJSON_CreateString(spellnames[u->combatspell]));
        }
    }
    chld = 0;
    for (i = 0; i != MAXITEMS; i++) {
        if (u->items[i]) {
            if (!chld) {
                cJSON_AddItemToObject(json, "items", chld = cJSON_CreateArray());
            }
            cJSON_AddItemToArray(chld, show_item(u, i));
        }
    }
    return json;
}

static cJSON * show_exit(const faction *f, const region * r, int d) {
    char buf[16];
    cJSON *json;
    region * rn = r->connect[d];

    DBG_UNREFERENCED_PARAMETER(f);
    json = cJSON_CreateObject();
    strncpy(buf, keywords[directions[d]], sizeof(buf));
    cJSON_AddStringToObject(json, "direction", rtl_strlwr(buf));
    cJSON_AddStringToObject(json, "terrain", rn->terrain->name);
    return json;
}

static cJSON * show_region(const faction *f, region * r) {
    cJSON *json, *chld;
    int x, y, d;
    const char * str;
    ql_iter qli;
    unit *u;
    
    x = (r->x - f->origin_x + config.width) % config.width;
    y = (r->y - f->origin_y + config.height) % config.height;
    json = cJSON_CreateObject();
    if (!region_isocean(r)) {
        if ((str = region_getname(r))!=0) {
            cJSON_AddStringToObject(json, "name", str);
        }
    }
    cJSON_AddStringToObject(json, "terrain", r->terrain->name);
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
    if (r->buildings) {
        cJSON_AddItemToObject(json, "buildings", chld = cJSON_CreateArray());
        for (qli=qli_init(&r->buildings);qli_more(qli);) {
            building *b = (building *)qli_next(&qli);
            cJSON_AddItemToArray(chld, show_building(f, b));
        }
    }
    if (r->ships) {
        ql_iter sli;

        cJSON_AddItemToObject(json, "ships", chld = cJSON_CreateArray());
        for (sli = qli_init(&r->ships); qli_more(sli);) {
            ship *s = (ship *)qli_next(&sli);
            cJSON_AddItemToArray(chld, show_ship(f, s));
        }
    }
    cJSON_AddItemToObject(json, "units", chld = cJSON_CreateArray());
    for (u=r->units;u;u=u->next) {
        cJSON_AddItemToArray(chld, show_unit(f, r, u));
    }
    return json;
}

cJSON * json_report(faction * f) {
    cJSON *json, *chld;
    ql_iter rli;
    int i;
    
    json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "turn", turn);
    cJSON_AddItemToObject(json, "faction", chld = cJSON_CreateObject());
    cJSON_AddNumberToObject(chld, "id", f->no);
    cJSON_AddStringToObject(chld, "name", faction_getname(f));

    chld = 0;
    for (i = 0; i != MAXSPELLS; i++) {
        if (f->showdata[i]) {
            cJSON *spell;
            if (!chld) {
                cJSON_AddItemToObject(json, "spells", chld = cJSON_CreateArray());
            }
            cJSON_AddItemToArray(chld, spell = cJSON_CreateArray());
            cJSON_AddNumberToObject(spell, "level", spelllevel[i]);
            cJSON_AddStringToObject(spell, "name", spellnames[i]);
            cJSON_AddStringToObject(spell, "description", spelldata[i]);
        }
    }
    
    if (f->mistakes) {
        cJSON_AddItemToObject(json, "mistakes", show_qstrlist(f->mistakes));
    }
    if (f->messages) {
        cJSON_AddItemToObject(json, "messages", show_qstrlist(f->messages));
    }
    if (f->battles) {
        ql_iter bli;
        cJSON_AddItemToObject(json, "battles", chld = cJSON_CreateArray());
        for (bli = qli_init(&f->battles);qli_more(bli);) {
            battle * b = (battle *)qli_next(&bli);
            cJSON *jbtl, *jsd;
            int i;

            cJSON_AddItemToArray(chld, jbtl = cJSON_CreateObject());
            cJSON_AddStringToObject(jbtl, "region", region_getname(b->region));
            cJSON_AddItemToObject(jbtl, "events", show_qstrlist(b->events));
            cJSON_AddItemToObject(jbtl, "sides", jsd = cJSON_CreateArray());
            for (i=0;i!=2;++i) {
                cJSON * jul;
                ql_iter uli;

                cJSON_AddItemToArray(jsd, jul = cJSON_CreateArray());
                for (uli=qli_init(b->units+i);qli_more(uli);) {
                    unit *u = (unit *)qli_next(&uli);
                    cJSON_AddItemToArray(jul, show_unit(f, b->region, u));
                }
            }
        }
    }
    if (f->events) {
        cJSON_AddItemToObject(json, "events", show_qstrlist(f->events));
    }
    /* spells */
    if (f->allies.factions) {
        ql_iter qli;
        cJSON_AddItemToObject(json, "allies", chld = cJSON_CreateArray());
        for (qli = qli_init(&f->allies.factions); qli_more(qli);) {
            faction *rf = (faction *)qli_next(&qli);
            cJSON_AddItemToArray(chld, cJSON_CreateNumber(rf->no));
        }
    }
    cJSON_AddItemToObject(json, "regions", chld = cJSON_CreateArray());
    
    for (rli = qli_init(&regions); qli_more(rli);) {
        region * r = (region *)qli_next(&rli);
        unit *u;

        for (u=r->units;u;u=u->next) {
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
