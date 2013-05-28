#ifndef ATL_BATTLE_H
#define ATL_BATTLE_H

struct quicklist;
struct region;
struct unit;

typedef struct battle {
    struct battle * next; 
    struct unit * units[2];
    struct region * region;
    struct quicklist * events;
} battle;

struct battle *create_battle(struct region *r);
void free_battle(struct battle * b);

#endif

