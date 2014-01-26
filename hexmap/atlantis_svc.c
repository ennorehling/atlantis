#include "service.h"

#include <region.h>

#include <quicklist.h>
#include <stdlib.h>

static void svc_region_setsize(int w, int h) {
    config.width = w;
    config.height = h;
}

static void svc_region_getsize(int *w, int *h) {
    *w = config.width;
    *h = config.height;
}

static int svc_region_x(HREGION h)
{
    region *r = (region*)h.ptr;
    return r->x;
}

static int svc_region_y(HREGION h)
{
    region *r = (region*)h.ptr;
    return r->y;
}

static const char * svc_region_getterrain(HREGION h)
{
    region *r = (region*)h.ptr;
    return r->terrain->name;
}

static const char * svc_region_getname(HREGION h)
{
    region *r = (region*)h.ptr;
    return region_getname(r);
}

static HREGION svc_region_create(int x, int y, const char* terrain)
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

static void svc_region_destroy(HREGION h)
{
    region *r = (region*)h.ptr;
    quicklist **qlp = &regions;
    int qi = 0;

    if (ql_find(qlp, &qi, r, 0)) {
        ql_delete(qlp, qi);
    }
    free_region(r);
}

static HREGION svc_region_get(int x, int y)
{
    HREGION result;
    result.ptr = findregion(x, y);
    return result;
}

region_svc iregion = {
    svc_region_setsize,
    svc_region_getsize,
    svc_region_x,
    svc_region_y,
    svc_region_getterrain,
    svc_region_getname,
    svc_region_create,
    svc_region_destroy,
    svc_region_get
};
