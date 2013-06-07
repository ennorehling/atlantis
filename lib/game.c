#include "game.h"
#include "region.h"
#include "faction.h"

#include <quicklist.h>
#include <string.h>

int nextunitid = 1;

void cleargame(void)
{
    nextunitid = 1;
    memset(&config, 0, sizeof(config));
    ql_foreach(regions, (ql_cb)free_region);
    ql_free(regions);
    regions = 0;

    while (factions) {
        faction * f = factions;
        factions = f->next;

        free_faction(f);
    }
}

