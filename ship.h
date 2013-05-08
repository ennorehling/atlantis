/*
 * Atlantis v1.0 13 September 1993
 * Copyright 1993 by Russell Wallace
 *
 * This program may be freely used, modified and distributed. It may not be
 * sold or used commercially without prior written permission from the author.
 */

#ifndef ATL_SHIP_H
#define ATL_SHIP_H

#include "settings.h"
#include "bool.h"

struct ship;
struct region;

typedef enum {
    SH_LONGBOAT,
    SH_CLIPPER,
    SH_GALLEON,
} ship_t;

typedef struct ship {
    struct ship *next;
    int no;
    char *name_;
    char *display_;
    ship_t type;
    int left;
} ship;

extern const char *shiptypenames[];

const char * ship_getname(const struct ship *self);
void ship_setname(struct ship *self, const char *name);
const char * ship_getdisplay(const struct ship *self);
void ship_setdisplay(struct ship *self, const char *display);

#endif
