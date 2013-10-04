#include "service.h"

#include <region.h>

#include <quicklist.h>
#include <stdlib.h>

static int region_x(HREGION h)
{
    region *r = (region*)h.ptr;
    return r->x;
}

static int region_y(HREGION h)
{
    region *r = (region*)h.ptr;
    return r->y;
}

static const char * region_terrain(HREGION h)
{
    region *r = (region*)h.ptr;
    return r->terrain->name;
}

static const char * region_name(HREGION h)
{
    region *r = (region*)h.ptr;
    return region_getname(r);
}

static HREGION region_create(int x, int y, const char* terrain)
{
    HREGION result = { 0 };
    const struct terrain *t = get_terrain_by_name(terrain);
    if (!t) {
        t = create_terrain(terrain);
    }
    if (t) {
        region *r = create_region(0, x, y, t);
        result.ptr = r;
    }
    return result;
}

static HREGION region_get(int x, int y)
{
    HREGION result;
    result.ptr = findregion(x, y);
    return result;
}

region_svc iregion = {
    region_x,
    region_y,
    region_terrain,
    region_name,
    region_create,
    region_get
};
