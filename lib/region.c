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

void region_addunit(struct region *r, struct unit *u, struct unit *stack, struct unit **hint)
{
    unit **up, *top = 0;
    assert(u);
    assert(!stack || stack->region==r);
    u->region = r;
	up = hint ? hint : &r->units;
    up = stack ? &stack->next : up;
    while (*up) {
        unit *x = *up;
        up = &x->next;
        if (top) {
             if (x->stack!=stack) {
                 break;
             }
        }
        else if (stack && stack==x->stack) {
            top = stack;
        }
    }
    u->stack = stack;
    u->next = *up;
    *up = u;
}

bool region_rmunit(struct region *r, struct unit *u, struct unit **hint)
{
    unit **up = hint ? hint : &r->units;
    while (*up) {
        unit *x = *up;
        if (x==u) {
            *up = x->next;
            return true;
        }
        up = &x->next;
    }
    return false;

}