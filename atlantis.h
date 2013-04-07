/*
 * Atlantis v1.0 13 September 1993
 * Copyright 1993 by Russell Wallace
 *
 * This program may be freely used, modified and distributed. It may not be
 * sold or used commercially without prior written permission from the author.
 */

#ifndef ATLANTIS_H
#define ATLANTIS_H

struct region;
struct faction;

extern int turn;
extern const char *keywords[];
extern char buf[];

void initgame(void);
void cleargame(void);
void writegame(void);
void readgame(void);

int transform(int *x, int *y, int kwd);
int rnd_seed(unsigned int x);
int processturn(const char *orders);
void createcontinent(void);
void addplayers(void);
void addunits(void);

struct region *movewhere(struct region * r);

void makeblock(int x1, int y1);
struct faction * addplayer(struct region * r, const char * email);
const char *regionid(const struct region * r, const struct faction *f);

int findkeyword(const char *s);
int igetkeyword(const char *s);
char *igetstr(const char *s);
char *getstr(void);

#endif
