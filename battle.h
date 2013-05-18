#ifndef ATL_BATTLE_H
#define ATL_BATTLE_H

struct strlist;
struct region;
struct unit;

typedef struct battle {
    struct battle * next; 
    struct unit * units[2];
    struct region * region;
    struct strlist * events;
} battle;

#endif

