/*
 * Atlantis v1.0 13 September 1993
 * Copyright 1993 by Russell Wallace
 *
 * This program may be freely used, modified and distributed. It may not be
 * sold or used commercially without prior written permission from the author.
 */

#ifndef ATL_REGION_H
#define ATL_REGION_H

#include "settings.h"

struct building;
struct ship;
struct unit;

typedef enum {
    T_OCEAN,
    T_PLAIN,
    T_MOUNTAIN,
    T_FOREST,
    T_SWAMP,
} terrain_t;
#define NUMTERRAINS 5

typedef struct region {
    struct region *next;
    int x, y;
    char * name_;
    struct region *connect[MAXDIRECTIONS];
    terrain_t terrain;
    int peasants;
    int money;
    struct building *buildings;
    struct ship *ships;
    struct unit *units;
    int immigrants;
} region;

extern struct region * regions;

struct region * findregion(int x, int y);
struct region * create_region(int x, int y, terrain_t t);

const char * region_getname(const struct region *r);
void region_setname(struct region *r, const char *name);
void region_addunit(struct region *self, struct unit *u);

#endif
