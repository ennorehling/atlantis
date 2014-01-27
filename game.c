#include "game.h"

#include "faction.h"
#include "region.h"
#include "ship.h"

#include <quicklist.h>
#include <string.h>

int nextunitid = 1;
int turn;

void cleargame(bool clearconfig)
{
    nextunitid = 1;
    memset(&config, 0, sizeof(config));
    free_regions();
    ql_foreach(factions, (ql_cb)free_faction);
    ql_free(factions);
    factions = 0;
    turn = 0;
    if (clearconfig) {
        ql_foreach(shiptypes, (ql_cb)free_shiptype);
        ql_free(shiptypes);
        shiptypes = 0;
        free_terrains();
    }
}

