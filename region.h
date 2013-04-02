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

typedef struct region {
    struct region *next;
    int x, y;
    char name[NAMESIZE];
    struct region *connect[MAXDIRECTIONS];
    int terrain;
    int peasants;
    int money;
    struct building *buildings;
    struct ship *ships;
    struct unit *units;
    int immigrants;
} region;

#endif
