#include "game.h"

#include "faction.h"
#include "region.h"
#include "ship.h"

#include <quicklist.h>
#include <string.h>

int nextunitid = 1;
int turn;

void cleargame(void)
{
    nextunitid = 1;
    memset(&config, 0, sizeof(config));
    ql_foreach(shiptypes, (ql_cb)free_shiptype);
    ql_free(shiptypes);
    shiptypes = 0;
    ql_foreach(terrains, (ql_cb)free_terrain);
    ql_free(terrains);
    terrains = 0;
    ql_foreach(regions, (ql_cb)free_region);
    ql_free(regions);
    regions = 0;
    ql_foreach(factions, (ql_cb)free_faction);
    ql_free(factions);
    factions = 0;
    turn = 0;
}

