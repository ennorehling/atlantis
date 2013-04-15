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
#include <stdio.h>

struct region;
struct faction;
struct stream;

typedef struct strlist {
    struct strlist *next;
    char s[1];
} strlist;

extern int turn;
extern const char *keywords[];
extern char buf[];
extern const keyword_t directions[];

void read_orders(struct stream * strm);
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
int processturn(const char *orders);
void createcontinent(void);
void addplayers(struct region * r, struct stream * strm);
void addunits(void);

struct region *movewhere(struct region * r);

void makeblock(int x1, int y1);
struct faction * addplayer(struct region * r, const char * email, int no);
const char *regionid(const struct region * r, const struct faction *f);

int findkeyword(const char *s);
int igetkeyword(const char *s);
char *igetstr(const char *s);
char *getstr(void);

#endif
