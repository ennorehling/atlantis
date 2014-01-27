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
    int no;
    char *name_;
    char *display_;
    int size;
    int sizeleft;
} building;

struct building *create_building(int no);
void free_building(struct building *b);

const char * building_getname(const struct building *self);
void building_setname(struct building *self, const char *name);
const char * building_getdisplay(const struct building *self);
void building_setdisplay(struct building *self, const char *display);

#endif
