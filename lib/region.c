#include "region.h"
#include "atlantis.h"
#include "unit.h"
#include "ship.h"
#include "building.h"

#include <quicklist.h>

#include <assert.h>
#include <string.h>
#include <stdlib.h>

quicklist *regions;

region * create_region(int x, int y, terrain_t t)
{
    region * r;

    r = (region *)malloc(sizeof(region));
    if (r) {
        memset(r, 0, sizeof(region));

        r->x = x;
        r->y = y;
        r->terrain = t;

        ql_push(&regions, r);
    }
    return r;
}

void free_region(region *r) {
    free(r->name_);
    while (r->units) {
        unit *u = r->units;
        r->units = u->next;
        free_unit(u);
    }
    ql_foreach(r->ships, (ql_cb)free_ship);
    ql_free(r->ships);
    r->ships = 0;
    ql_foreach(r->buildings, (ql_cb)free_building);
    ql_free(r->buildings);
    r->buildings = 0;
    free(r);
}

const char * region_getname(const struct region *r)
{
    return r->name_;
}

void region_setname(struct region *r, const char *name)
{
    if (name) {
        r->name_ = (char *)realloc(r->name_, strlen(name)+1);
        strcpy(r->name_, name);
    } else {
        free(r->name_);
        r->name_ = 0;
    }
}

static void reorder_unit(unit *u, unit **ur, unit **ui) {
    assert(ui || !"no insert position given");
    assert (!u->region || ur || !"unit must be removed");
    assert (!ur || *ur==u || !"removal position does not match unit");
    if (ur!=ui) { /* else: nothing to do */
        if (ur) {
            *ur = u->next;
        }
        u->next = *ui;
        *ui = u;
    }
}

void region_addunit(struct region *r, struct unit *u, struct unit **hint)
{
    unit **up = &r->units;
    unit **ui = up, **ur = 0;
    ql_iter bli = qli_init(&r->buildings);
    ql_iter sli = qli_init(&r->ships);
    assert(u);
    assert(!u->region || u->region==r);

    if (hint) {
        u->region = r;
        u->next = *hint;
        *hint = u;
        return;
    }
    while (*up) {
        unit *x = *up;
        if (x==u) ur = up;
        if (u->stack) {
            if (x->stack==u->stack || x==u->stack) {
                ui = &x->next;
            }
        } else if (u->building) {
            if (!x->building) {
                ui = up;
                break;
            }
            else if (x->building==u->building) {
                ui = &x->next;
            }
            else {
                struct building *b = 0;
                while (qli_more(bli)) {
                    b = (struct building *)qli_get(bli);
                    if (b==u->building) {
                        ui = up;
                        break;
                    } else if (b==x->building) {
                        break;
                    }
                    qli_next(&bli);
                }
                if (b==u->building) {
                    break;
                }
            }
        } else if (u->ship) {
            if (!x->ship) {
                ui = up;
                break;
            }
            else if (x->ship==u->ship) {
                ui = &x->next;
            }
            else {
                struct ship *s = 0;
                while (qli_more(sli)) {
                    s = (struct ship *)qli_get(sli);
                    if (s==u->ship) {
                        ui = up;
                        break;
                    } else if (s==x->ship) {
                        break;
                    }
                    qli_next(&sli);
                }
                if (s==u->ship) {
                    break;
                }
            }
        } else if (!x->next) {
            ui = &x->next;
            break;
        }
        up=&x->next;
    }
    if (u->region) {
        while (!ur && *up) {
            unit *x = *up;
            if (x==u) ur = up;
            up = &x->next;
        }
    }
    reorder_unit(u, ur, ui);
    u->region = r;
}

bool region_rmunit(struct region *r, struct unit *u, struct unit **hint)
{
    unit **up = hint ? hint : &r->units;
    while (*up) {
        unit *x = *up;
        if (x==u) {
            *up = x->next;
            u->region = 0;
            return true;
        }
        up = &x->next;
    }
    return false;

}