#include "report.h"
#include "settings.h"

#include "region.h"
#include "faction.h"
#include "ship.h"
#include "building.h"
#include "unit.h"

#include "rtl.h"

#include <quicklist.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>

char buf[10240];

void scat(const char *s)
{
    strcat(buf, s);
}

void icat(int n)
{
    char s[20];

    sprintf(s, "%d", n);
    scat(s);
}

const char *factionid(const faction * f)
{
    static char buf[NAMESIZE + 20];

    sprintf(buf, "%s (%d)", faction_getname(f), f->no);
    return buf;
}

static void coor_torus(int *x, int *y)
{
    *x = (*x + config.width) % config.width;
    *y = (*y + config.height) % config.height;
}

static void coor_alh(int *x, int *y)
{
    int ox = (*x + config.width) % config.width;
    int oy = (*y + config.height) % config.height;
    *x = ox;
    *y = (2 *oy -ox)  % config.height;
}

void coor_transform(coor_t transform, int *x, int *y) {
    if (transform==COOR_TORUS) {
        coor_torus(x, y);
    }
    else if (transform==COOR_ALH) {
        coor_alh(x, y);
    }
}

const char *regionid(const region * r, const faction * f)
{
    int x, y;
    static char buf[NAMESIZE + 20];
    assert(r);
    x = r->x - (f ? f->origin_x : 0);
    y = r->y - (f ? f->origin_y : 0);

    coor_transform(config.transform, &x, &y);
    if (region_isocean(r)) {
        sprintf(buf, "(%d,%d)", x, y);
    }
    else {
        sprintf(buf, "%s (%d,%d)", region_getname(r), x, y);
    }
    return buf;
}

const char *buildingid(const building * b)
{
    static char buf[NAMESIZE + 20];

    sprintf(buf, "%s (%d)", building_getname(b), b->no);
    return buf;
}

const char *shipid(const ship * sh)
{
    static char buf[NAMESIZE + 20];

    sprintf(buf, "%s (%d)", ship_getname(sh), sh->no);
    return buf;
}

const char *unitid(const unit * u)
{
    static char buf[NAMESIZE + 20];

    sprintf(buf, "%s (%d)", unit_getname(u), u->no);
    return buf;
}

void mistake(faction * f, const char *s, const char *comment)
{
    static char buf[512];

    sprintf(buf, "%s: %s.", s, comment);
    ql_push(&f->mistakes, _strdup(buf));
}

void mistakes(unit * u, const char *str, const char *comment)
{
    static char buf[512];

    sprintf(buf, "%s: %s - %s.", unitid(u), str, comment);
    ql_push(&u->faction->mistakes, _strdup(buf));
}

void mistakeu(unit * u, const char *comment)
{
    mistakes(u, u->thisorder, comment);
}
