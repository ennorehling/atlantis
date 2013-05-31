#include "game.h"
#include "region.h"
#include "faction.h"

#include <string.h>

void cleargame(void)
{
    memset(&config, 0, sizeof(config));
    while (regions) {
        region * r = regions;
        regions = r->next;
        free_region(r);
    }

    while (factions) {
        faction * f = factions;
        factions = f->next;

        free_faction(f);
    }
}

