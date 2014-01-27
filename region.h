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
#include "bool.h"

struct building;
struct ship;
struct unit;

typedef struct terrain {
    char * name;
    int foodproductivity; /* work */
    int maxfoodoutput;    /* food */
    int maxoutput[4];     /* output */
} terrain;

typedef enum {
    T_OCEAN,
    T_PLAIN,
    T_MOUNTAIN,
    T_FOREST,
    T_SWAMP,
} terrain_t;
#define NUMTERRAINS 5

extern const char *terrainnames[];

typedef struct region {
    unsigned int uid;
    int x, y;
    char * name_;
    struct region *connect[MAXDIRECTIONS];
    const struct terrain * terrain;
    int peasants;
    int money;
    struct quicklist *buildings;
    struct quicklist *ships;
    struct unit *units;
    int immigrants;
    struct region *nexthash_;
} region;

extern struct quicklist *regions;

struct region * create_region(unsigned int uid, int x, int y, const struct terrain *t);
void free_region(struct region *r);
struct region * findregion(int x, int y);
struct region * get_region(unsigned int uid);
void free_regions(void);

const char * region_getname(const struct region *r);
void region_setname(struct region *r, const char *name);
void region_addunit(struct region *r, struct unit *u, struct unit **hint);
bool region_rmunit(struct region *r, struct unit *u, struct unit **hint);

bool region_isocean(const struct region *r);

void free_terrain(terrain *t);
struct terrain *create_terrain(const char * name);
struct terrain *get_terrain(terrain_t t);
struct terrain *get_terrain_by_name(const char *name);
void free_terrains(void);

#endif
