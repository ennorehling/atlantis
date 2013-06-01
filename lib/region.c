#include "region.h"
#include "atlantis.h"
#include "unit.h"
#include "ship.h"
#include "building.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

region *regions;

region * create_region(int x, int y, terrain_t t)
{
    region * r;

    r = (region *)malloc(sizeof(region));
    if (r) {
        memset(r, 0, sizeof(region));

        r->x = x;
        r->y = y;
        r->terrain = t;

        addlist(&regions, r);
    }
    return r;
}

void free_region(region *r) {
    free(r->name_);
    while (r->units) {
        unit * u = r->units;
        r->units = u->next;
        free_unit(u);
    }
    while (r->ships) {
        ship * s = r->ships;
        r->ships = s->next;
        free(s);
    }
    while (r->buildings) {
        building * b = r->buildings;
        r->buildings = b->next;
        free(b);
    }
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

void region_addunit(struct region *self, struct unit *u)
{
    unit ** iter;
    assert(u && !u->next);
    for (iter=&self->units; *iter; iter=&(*iter)->next);
    *iter = u;
}
