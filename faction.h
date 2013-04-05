/*
 * Atlantis v1.0 13 September 1993
 * Copyright 1993 by Russell Wallace
 *
 * This program may be freely used, modified and distributed. It may not be
 * sold or used commercially without prior written permission from the author.
 */

#ifndef ATL_FACTION_H
#define ATL_FACTION_H

#include "settings.h"
#include "spells.h"
#include "bool.h"

struct strlist;

typedef struct rfaction {
    struct rfaction *next;
    struct faction *faction;
    int factionno;
} rfaction;

typedef struct faction {
    struct faction *next;
    int no, origin_x, origin_y;
    char name[NAMESIZE];
    char addr[NAMESIZE];
    int lastorders;
    bool seendata[MAXSPELLS];
    bool showdata[MAXSPELLS];
    struct rfaction *accept;
    struct rfaction *admit;
    struct rfaction *allies;
    struct strlist *mistakes;
    struct strlist *messages;
    struct strlist *battles;
    struct strlist *events;
    char alive;
    char attacking;
    bool seesbattle;
    char dh;
    int nunits;
    int number;
    int money;
} faction;

struct faction * create_faction(int no);

#endif
