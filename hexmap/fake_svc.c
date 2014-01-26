#include "service.h"

#include <quicklist.h>
#include <stdlib.h>

typedef struct region_s {
    int x, y;
    char *name;
    const char *terrain;
} region_s;

static quicklist * regions = 0;
static int width, height;

static void region_setsize(int w, int h) {
    width = w;
    height = h;
}

static void region_getsize(int *w, int *h) {
    *w = width;
    *h = height;
}

static int region_x(HREGION h)
{
    region_s *r = (region_s*)h.ptr;
    return r->x;
}

static int region_y(HREGION h)
{
    region_s *r = (region_s*)h.ptr;
    return r->y;
}

static const char * region_getterrain(HREGION h)
{
    region_s *r = (region_s*)h.ptr;
    return r->terrain;
}

static const char * region_getname(HREGION h)
{
    region_s * r = (region_s *)h.ptr;
    return r->name;
}

static HREGION region_create(int x, int y, const char *terrain)
{
    HREGION result;
    region_s *r = (region_s *)calloc(1, sizeof(region_s));
    r->x = x;
    r->y = y;
    r->terrain = terrain;
    ql_push(&regions, r);
    result.ptr = r;
    return result;
}

static void region_destroy(HREGION h)
{
    region_s *r = (region_s *)h.ptr;
    quicklist **qlp = &regions;
    int qi = 0;

    if (ql_find(qlp, &qi, r, 0)) {
        ql_delete(qlp, qi);
    }
    free(r);
}

static HREGION region_get(int x, int y)
{
    HREGION result = { 0 };
    ql_iter qli = qli_init(&regions);
    while (qli_more(qli)) {
        region_s *r = (region_s *)qli_next(&qli);
        if (r->x==x && r->y==y) {
            result.ptr = r;
            break;
        }
    }
    return result;
}

region_svc iregion = {
    region_setsize,
    region_getsize,
    region_x,
    region_y,
    region_getterrain,
    region_getname,
    region_create,
    region_destroy,
    region_get
};
