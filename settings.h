/*
 * Atlantis v1.0 13 September 1993
 * Copyright 1993 by Russell Wallace
 *
 * This program may be freely used, modified and distributed. It may not be
 * sold or used commercially without prior written permission from the author.
 */

#ifndef ATL_SETTINGS_H
#define ATL_SETTINGS_H

#define CFG_STACKS    0x01
#define CFG_TEACHERS  0x02
#define CFG_MOVES     0x04

struct stream;
struct cJSON;

typedef enum {
    COOR_NONE,
    COOR_TORUS,
    COOR_ALH,
    COOR_ERESSEA
} coor_t;

#define NAMESIZE 81
#define DISPLAYSIZE 161
#define MAXDIRECTIONS 6

typedef struct settings {
    int width, height;
    int moves;
    int upkeep;
    unsigned int features;
    int startmoney;
    int startmen;
    coor_t transform;
    char **directions[MAXDIRECTIONS];
} settings;

extern struct settings config;

void read_config_json(struct cJSON*);
int read_config(const char *filename);

#define ORDERGAP 4
#define MAXPERBLOCK 5
#define BLOCKSIZE 8
#define BLOCKMAX 2
#define MAINTENANCE 10
#define STARTMONEY 5000
#define RECRUITCOST 50
#define RECRUITFRACTION 4
#define ENTERTAININCOME 20
#define ENTERTAINFRACTION 20
#define TAXINCOME 200
#define COMBATEXP 10
#define PRODUCEEXP 10
#define TEACHNUMBER 10
#define STUDYCOST 200
#define POPGROWTH 5
#define PEASANTMOVE 5

#endif
