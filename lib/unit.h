/*
 * Atlantis v1.0 13 September 1993
 * Copyright 1993 by Russell Wallace
 *
 * This program may be freely used, modified and distributed. It may not be
 * sold or used commercially without prior written permission from the author.
 */

#ifndef ATL_UNIT_H
#define ATL_UNIT_H

#include "settings.h"
#include "bool.h"
#include "spells.h"
#include "skills.h"
#include "items.h"

struct building;
struct ship;
struct faction;
struct region;
struct strlist;

typedef struct unit {
    struct unit *next;
    int no;
    char * name_;
    char * display_;
    int number;
    int money;
    struct faction *faction;
    struct building *building;
    struct ship *ship;
    bool owner;
    bool behind;
    bool guard;
    char thisorder[NAMESIZE];
    char lastorder[NAMESIZE];
    int combatspell;
    int skills[MAXSKILLS];
    int items[MAXITEMS];
    spell_t spells[MAXSPELLS];
    struct strlist *orders;
    int alias;
    int dead;
    int learning;
    int n;
    int *litems;
    int side;
    bool isnew;
} unit;

struct unit * findunitg(int n);
struct unit * create_unit(struct faction * f, int no);

const char * unit_getname(const struct unit *u);
void unit_setname(struct unit *u, const char *name);
const char * unit_getdisplay(const struct unit *u);
void unit_setdisplay(struct unit *u, const char *display);

#endif
