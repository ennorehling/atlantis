#include "game.h"

#include "faction.h"
#include "region.h"
#include "ship.h"

#include <quicklist.h>
#include <string.h>
#include <stdlib.h>

int nextunitid = 1;
int turn;

void cleargame(bool clearconfig)
{
    nextunitid = 1;
    free_regions();
    ql_foreach(factions, (ql_cb)free_faction);
    ql_free(factions);
    factions = 0;
    turn = 0;
    if (clearconfig) {
        int d;
        for (d = 0; d != MAXDIRECTIONS; ++d) {
            int k;
            for (k = 0; config.directions[d] && config.directions[d][k]; ++k) {
                free(config.directions[d][k]);
            }
            free(config.directions[d]);
        }
        memset(&config, 0, sizeof(config));
        ql_foreach(shiptypes, (ql_cb)free_shiptype);
        ql_free(shiptypes);
        shiptypes = 0;
        free_terrains();
    }
}

