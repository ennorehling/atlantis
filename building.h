/*
 * Atlantis v1.0 13 September 1993
 * Copyright 1993 by Russell Wallace
 *
 * This program may be freely used, modified and distributed. It may not be
 * sold or used commercially without prior written permission from the author.
 */

#ifndef ATL_BUILDING_H
#define ATL_BUILDING_H

typedef struct building {
    struct building *next;
    int no;
    char name[NAMESIZE];
    char display[DISPLAYSIZE];
    int size;
    int sizeleft;
} building;

#endif
