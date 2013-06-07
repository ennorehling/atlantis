#include "battle.h"
#include "unit.h"
#include "atlantis.h"

#include <quicklist.h>
#include <stdlib.h>

battle *create_battle(struct region *r) {
    battle * b = (battle *)calloc(1, sizeof(battle));
    if (b) {
        b->region = r;
    }
    return b;
}

void free_battle(battle * b) {
    int i;
    ql_foreach(b->events, free);
    ql_free(b->events);
    for (i=0;i!=2;++i) {
        while (b->units[i]) {
            unit * u = b->units[i];
            b->units[i] = u->next;
            free_unit(u);
        }
    }
    free(b);
}
