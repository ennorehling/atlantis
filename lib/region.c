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
    ql_foreach(r->units, (ql_cb)free_unit);
    ql_free(r->units);
    r->units = 0;
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

void region_addunit(struct region *r, struct unit *u)
{
    assert(u);
    u->region = r;
    ql_push(&r->units, u);
}

void region_rmunit(struct region *r, struct unit *u)
{
    quicklist *ql;
    int qi;
    assert(u && u->region==r);
    for (qi=0,ql=r->units;ql;ql_advance(&ql, &qi, 1)) {
        unit *x = (unit *)ql_get(ql, qi);
        if (x==u) {
            if (ql==r->units) {
                ql_delete(&r->units, qi);
            } else {
                ql_delete(&ql, qi);
            }
            break;
        }
    }    
    u->region = 0;
}
