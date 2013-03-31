/*
 * Atlantis v1.0 13 September 1993
 * Copyright 1993 by Russell Wallace
 *
 * This program may be freely used, modified and distributed. It may not be
 * sold or used commercially without prior written permission from the author.
 */

#ifndef ATLANTIS_H
#define ATLANTIS_H

int turn;

int rnd_seed(unsigned int x);
void initgame(void);
int processturn(const char *orders);
void createcontinent(void);
void addplayers(void);
void addunits(void);

#endif