/*
 * Atlantis v1.0 13 September 1993
 * Copyright 1993 by Russell Wallace
 *
 * This program may be freely used, modified and distributed. It may not be
 * sold or used commercially without prior written permission from the author.
 */

#ifndef ATLANTIS_H
#define ATLANTIS_H

#include "keywords.h"
#include "items.h"
#include <stdio.h>

struct region;
struct faction;
struct stream;
struct unit;
struct troop;
struct ship;
struct building;

typedef struct strlist {
    struct strlist *next;
    char s[1];
} strlist;

typedef struct rect {
    int left, top, width, height;
} rect;

extern rect world;
extern int turn;
extern int ignore_password;
extern const char *keywords[];
extern char buf[];
extern const keyword_t directions[];
extern const char *terrainnames[];
extern const char *skillnames[];
extern const char *itemnames[2][MAXITEMS];

/* untyped list bullshit */
void freestrlist(struct strlist ** slist);
void addlist(void *l1, void *p1);

void read_orders(struct stream * strm);
void processorders(void);
void process_form(struct unit *u, struct region *r);
void reports(void);
void initgame(void);
void cleargame(void);
int writegame(void);
int readgame(void);
void writemap(FILE *);
void writesummary(void);
void makeworld(void);
int autoworld(const char * playerfile);

int transform(int *x, int *y, int kwd);
void rnd_seed(unsigned long x);
void createcontinent(void);
void addplayers(struct region * r, struct stream * strm);
void addunits(void);
int cansee(const struct faction * f, const struct region * r, const struct unit * u);
struct region *movewhere(struct region * r);
int effskill(const struct unit * u, int i);

void makeblock(int x1, int y1);
struct faction * addplayer(struct region * r, const char * email, int no);
const char *regionid(const struct region * r, const struct faction *f);

int findkeyword(const char *s);
int igetkeyword(const char *s);
char *igetstr(const char *s);
char *getstr(void);

struct troop * get_troop(struct troop *ta, int ntroops, int side, int mask);

struct unit *shipowner(const struct region * r, const struct ship * sh);
#endif
