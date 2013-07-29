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
#include "bool.h"
#include <stdio.h>

struct region;
struct faction;
struct stream;
struct unit;
struct ship;
struct building;

extern int turn;
extern int ignore_password;
extern const keyword_t directions[];

int distribute(int old, int nyu, int n);
int getseen(struct region *r, const struct faction *f, struct unit **uptr);

int effskill(const struct unit * u, int i);
int lovar(int n);
bool isallied(const struct unit * u, const struct unit * u2);

void read_orders(struct stream * strm);
void processorders(void);
void process_form(struct unit *u, struct region *r);
void report(struct faction * f);
const char *gamedate(void);

void initgame(void);
void cleargame(void);
int writegame(void);
int readgame(void);
void writemap(FILE *);
void writesummary(void);
void makeworld(void);
void update_world(int minx, int miny, int maxx, int maxy);
int autoworld(const char * playerfile);
void connectregions(void);

int transform(int *x, int *y, int kwd);
void rnd_seed(unsigned long x);
void createcontinent(void);
void addplayers(struct region * r, struct stream * strm);
void addunits(void);
int cansee(const struct faction * f, struct region * r, const struct unit * u);
struct region *movewhere(struct region * r);
int effskill(const struct unit * u, int i);

void makeblock(int x1, int y1);
struct faction * addplayer(struct region * r, const char * email, int no);
const char *regionid(const struct region * r, const struct faction *f);

struct unit *shipowner(struct region * r, const struct ship * sh);

#endif
