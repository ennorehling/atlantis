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
#define NUMSHIPS 3

extern const char *shipnames[];
extern struct quicklist *shiptypes;

typedef struct ship_type {
    char *name;
    int capacity;
    int cost;
    int speed;
} ship_type;

typedef struct ship {
    const struct ship_type *type;
    int no;
    char *name_;
    char *display_;
    int left;
} ship;

struct ship *create_ship(int no, const struct ship_type *type);
void free_ship(struct ship *s);

const char * ship_getname(const struct ship *self);
void ship_setname(struct ship *self, const char *name);
const char * ship_getdisplay(const struct ship *self);
void ship_setdisplay(struct ship *self, const char *display);
int ship_speed(const struct ship *sh);

struct ship_type *create_shiptype(const char *name);
void free_shiptype(struct ship_type *st);
struct ship_type *get_shiptype(ship_t type);
struct ship_type *get_shiptype_by_name(const char *name);

#endif
