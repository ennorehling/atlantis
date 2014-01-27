#include "luabind.h"
#include "region.h"
#include "unit.h"
#include "faction.h"

void set_region_terrain(region *r, const char *terrain) {
    const struct terrain *t = get_terrain_by_name(terrain);
    if (t) {
        r->terrain = t;
    }
}

const char * get_region_terrain(const region *r) {
    return r->terrain ? r->terrain->name : 0;
}
