#include "atlantis.h"
#include "keywords.h"
#include "region.h"
#include "faction.h"
#include "game.h"
#include "ship.h"
#include "building.h"
#include "unit.h"
#include "parser.h"
#include "settings.h"

#include "rtl.h"

#include <stream.h>
#include <memstream.h>
#include <filestream.h>
#include <quicklist.h>

#include <CuTest.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>

extern const char *terrainnames[];

static unit *make_unit(faction *f, region *r, int no) {
    unit * u = create_unit(f, no);
    u->number = 1;
    region_addunit(r, u, 0);
    return u;
}

void init_terrains(void) {
    int i;
    for (i=0;i!=NUMTERRAINS;++i) {
        terrain * t = create_terrain(terrainnames[i]);
    }
}

static void test_addunit_takes_hint(CuTest *tc)
{
    faction *f;
    region *r;
    unit *u1, *u2, *u3;
    
    cleargame();
    init_terrains();
    r = create_region(1, 1, get_terrain(T_PLAIN));
    f = create_faction(1);
    u1 = create_unit(f, 1);
    u2 = create_unit(f, 2);
    u3 = create_unit(f, 3);

    region_addunit(r, u1, &r->units);
    region_addunit(r, u2, &r->units);
    region_addunit(r, u3, &u2->next);
    CuAssertPtrEquals(tc, u2, r->units);
    CuAssertPtrEquals(tc, u3, u2->next);
    CuAssertPtrEquals(tc, u1, u3->next);
}

static void test_addunit_order_new_ship(CuTest *tc)
{
    faction *f;
    region *r;
    ship *s1, *s2;
    unit *u1, *u2;
    
    cleargame();
    init_terrains();
    r = create_region(1, 1, get_terrain(T_PLAIN));
    f = create_faction(1);
    u1 = create_unit(f, 1);
    u2 = create_unit(f, 2);
    s1 = create_ship(1, SH_LONGBOAT);
    s2 = create_ship(1, SH_LONGBOAT);
    ql_push(&r->ships, s1);

    u1->ship = s1;
    region_addunit(r, u1, 0);
    region_addunit(r, u2, 0);
    CuAssertPtrEquals(tc, u1, r->units);
    CuAssertPtrEquals(tc, u2, u1->next);

    u1->ship = s2;
    region_addunit(r, u1, 0);
    CuAssertPtrEquals(tc, u1, r->units);
    CuAssertPtrEquals(tc, u2, u1->next);
}

static void test_addunit_order_ships(CuTest *tc)
{
    faction *f;
    region *r;
    ship *s1, *s2;
    unit *u1, *u2, *u3;
    
    cleargame();
    init_terrains();
    r = create_region(1, 1, get_terrain(T_PLAIN));
    f = create_faction(1);
    u1 = create_unit(f, 1);
    u2 = create_unit(f, 2);
    u3 = create_unit(f, 3);
    s1 = create_ship(1, SH_LONGBOAT);
    s2 = create_ship(2, SH_LONGBOAT);
    ql_push(&r->ships, s1);
    ql_push(&r->ships, s2);

    u2->ship = s2;
    region_addunit(r, u2, 0);
    CuAssertPtrEquals(tc, u2, r->units);

    u1->ship = s1;
    region_addunit(r, u1, 0);
    CuAssertPtrEquals(tc, u1, r->units);
    CuAssertPtrEquals(tc, u2, u1->next);

    u3->ship = s2;
    region_addunit(r, u3, 0);
    CuAssertPtrEquals(tc, u3, u2->next);
}

static void test_addunit_order_buildings(CuTest *tc)
{
    faction *f;
    region *r;
    building *b1, *b2;
    unit *u1, *u2, *u3;
    
    cleargame();
    init_terrains();
    r = create_region(1, 1, get_terrain(T_PLAIN));
    f = create_faction(1);
    u1 = create_unit(f, 1);
    u2 = create_unit(f, 2);
    u3 = create_unit(f, 3);
    b1 = create_building(1);
    b2 = create_building(2);
    ql_push(&r->buildings, b1);
    ql_push(&r->buildings, b2);

    u2->building = b2;
    region_addunit(r, u2, 0);
    CuAssertPtrEquals(tc, u2, r->units);

    u1->building = b1;
    region_addunit(r, u1, 0);
    CuAssertPtrEquals(tc, u1, r->units);
    CuAssertPtrEquals(tc, u2, u1->next);

    u3->building = b2;
    region_addunit(r, u3, 0);
    CuAssertPtrEquals(tc, u1, r->units);
    CuAssertPtrEquals(tc, u2, u1->next);
    CuAssertPtrEquals(tc, u3, u2->next);
}

static void test_addunit_order_buildings_mixed(CuTest *tc)
{
    faction *f;
    region *r;
    building *b1, *b2;
    unit *u1, *u2, *u3;
    
    cleargame();
    init_terrains();
    r = create_region(1, 1, get_terrain(T_PLAIN));
    f = create_faction(1);
    u1 = create_unit(f, 1);
    u2 = create_unit(f, 2);
    u3 = create_unit(f, 3);
    b1 = create_building(1);
    b2 = create_building(2);
    ql_push(&r->buildings, b1);
    ql_push(&r->buildings, b2);

    u2->building = b2;
    region_addunit(r, u2, 0);
    CuAssertPtrEquals(tc, u2, r->units);

    region_addunit(r, u3, 0);
    CuAssertPtrEquals(tc, u3, u2->next);

    u1->building = b1;
    region_addunit(r, u1, 0);
    CuAssertPtrEquals(tc, u1, r->units);
    CuAssertPtrEquals(tc, u2, u1->next);
}

static void test_addunit_reorders(CuTest *tc)
{
    faction *f;
    region *r;
    building *b1;
    unit *u1, *u2;
    
    cleargame();
    init_terrains();
    r = create_region(1, 1, get_terrain(T_PLAIN));
    f = create_faction(1);
    u1 = create_unit(f, 1);
    u2 = create_unit(f, 2);
    b1 = create_building(1);
    ql_push(&r->buildings, b1);

    region_addunit(r, u1, 0);
    region_addunit(r, u2, 0);
    CuAssertPtrEquals(tc, u1, r->units);
    u2->building = b1;
}

static void test_setbuilding_reorders(CuTest *tc)
{
    faction *f;
    region *r;
    building *b1;
    unit *u1, *u2;
    
    cleargame();
    init_terrains();
    r = create_region(1, 1, get_terrain(T_PLAIN));
    f = create_faction(1);
    u1 = create_unit(f, 1);
    u2 = create_unit(f, 2);
    b1 = create_building(1);
    ql_push(&r->buildings, b1);

    region_addunit(r, u1, 0);
    region_addunit(r, u2, 0);
    unit_setbuilding(u2, b1);
    CuAssertPtrEquals(tc, u2, r->units);
}

static void test_setship_reorders(CuTest *tc)
{
    faction *f;
    region *r;
    ship *s1;
    unit *u1, *u2;
    
    cleargame();
    init_terrains();
    r = create_region(1, 1, get_terrain(T_PLAIN));
    f = create_faction(1);
    u1 = create_unit(f, 1);
    u2 = create_unit(f, 2);
    s1 = create_ship(1, SH_LONGBOAT);
    ql_push(&r->ships, s1);

    region_addunit(r, u1, 0);
    region_addunit(r, u2, 0);
    unit_setship(u2, s1);
    CuAssertPtrEquals(tc, u2, r->units);
}

static void test_addunit_order_mixed(CuTest *tc)
{
    faction *f;
    region *r;
    building *b1;
    ship *s1;
    unit *u1, *u2, *u3, *u4, *u5;
    
    cleargame();
    init_terrains();
    r = create_region(1, 1, get_terrain(T_PLAIN));
    f = create_faction(1);
    u1 = create_unit(f, 1);
    u2 = create_unit(f, 2);
    u3 = create_unit(f, 3);
    u4 = create_unit(f, 4);
    u5 = create_unit(f, 5);
    b1 = create_building(1);
    s1 = create_ship(1, SH_LONGBOAT);
    ql_push(&r->buildings, b1);
    ql_push(&r->ships, s1);

    region_addunit(r, u3, 0);
    CuAssertPtrEquals(tc, u3, r->units);
    u2->ship = s1;
    region_addunit(r, u2, 0);
    CuAssertPtrEquals(tc, u2, r->units);
    u1->building = b1;
    region_addunit(r, u1, 0);
    CuAssertPtrEquals(tc, u1, r->units);
    region_addunit(r, u5, 0);
    CuAssertPtrEquals(tc, u5, u3->next);
    u4->stack = u3;
    region_addunit(r, u4, 0);
    CuAssertPtrEquals(tc, u4, u3->next);
}

static void test_unit_reordering(CuTest *tc)
{
    faction *f;
    region *r;
    building *b1, *b2;
    unit *u1, *u2, *u3;
    
    cleargame();
    init_terrains();
    r = create_region(0, 0, get_terrain(T_PLAIN));
    f = create_faction(1);
    u1 = make_unit(f, r, 1);
    u2 = make_unit(f, r, 2);
    u3 = make_unit(f, r, 3);
    b1 = create_building(1);
    b2 = create_building(2);
    ql_push(&r->buildings, b1);
    ql_push(&r->buildings, b2);

    u1->building = b2;
    u3->building = b1;
    writegame();
    cleargame();
    init_terrains();
    readgame();
    r = findregion(0, 0);
    u1 = findunitg(1);
    u2 = findunitg(2);
    u3 = findunitg(3);
    CuAssertPtrEquals(tc, u3, r->units);
    CuAssertPtrEquals(tc, u1, u3->next);
    CuAssertPtrEquals(tc, u2, u1->next);
}

static void test_unstack_leader(CuTest *tc)
{
    faction *f;
    region *r;
    unit *u1, *u2, *u3;
    
    cleargame();
    init_terrains();
    r = create_region(1, 1, get_terrain(T_PLAIN));
    f = create_faction(1);
    u1 = make_unit(f, r, 1);
    u2 = make_unit(f, r, 2);
    u3 = make_unit(f, r, 3);
    
    CuAssertPtrEquals(tc, u1, unit_getstack(u1));
    CuAssertPtrEquals(tc, u2, unit_getstack(u2));

    unit_stack(u3, u2);
    unit_stack(u2, u1);

    unit_unstack(u1);
    CuAssertPtrEquals(tc, u1, unit_getstack(u1));
    CuAssertPtrEquals(tc, u2, unit_getstack(u2));
    CuAssertPtrEquals(tc, u2, unit_getstack(u3));
}

static void test_stacking(CuTest *tc)
{
    faction *f;
    region *r;
    unit *u1, *u2, *u3;
    
    cleargame();
    init_terrains();
    r = create_region(1, 1, get_terrain(T_PLAIN));
    f = create_faction(1);
    u1 = make_unit(f, r, 1);
    u2 = make_unit(f, r, 2);
    u3 = make_unit(f, r, 3);
    
    CuAssertPtrEquals(tc, u1, r->units);
    CuAssertPtrEquals(tc, u1, unit_getstack(u1));
    CuAssertPtrEquals(tc, u2, unit_getstack(u2));

    unit_stack(u3, u2);
    CuAssertPtrEquals(tc, u1, r->units);
    CuAssertPtrEquals(tc, u2, unit_getstack(u3));
    CuAssertPtrEquals(tc, u2, unit_getstack(u2));

    unit_stack(u2, u1);
    CuAssertPtrEquals(tc, u1, r->units);
    CuAssertPtrEquals(tc, u1, unit_getstack(u3));
    CuAssertPtrEquals(tc, u1, unit_getstack(u2));

    unit_unstack(u2);
    CuAssertPtrEquals(tc, u2, unit_getstack(u2));
    CuAssertPtrEquals(tc, u1, unit_getstack(u3));

    unit_unstack(u3);
    unit_stack(u2, u1);
    unit_stack(u3, u1);
    CuAssertPtrEquals(tc, u1, unit_getstack(u2));
    CuAssertPtrEquals(tc, u1, unit_getstack(u3));
}

static void test_stacking_moves_units(CuTest *tc)
{
    faction *f;
    region *r;
    unit *u1, *u2, *u3;
    
    cleargame();
    init_terrains();
    r = create_region(1, 1, get_terrain(T_PLAIN));
    f = create_faction(1);
    u1 = make_unit(f, r, 1);
    u2 = make_unit(f, r, 2);
    u3 = make_unit(f, r, 3);
    
    // 1, 2, 3:
    CuAssertPtrEquals(tc, u1, r->units);
    CuAssertPtrEquals(tc, u2, r->units->next);
    CuAssertPtrEquals(tc, u3, r->units->next->next);

    unit_stack(u3, u1);
    // 1->3, 2:
    CuAssertPtrEquals(tc, u3, u1->next);
    CuAssertPtrEquals(tc, u2, u3->next);

    unit_unstack(u3);
    // 1, 3, 2:
    CuAssertPtrEquals(tc, u3, u1->next);
    CuAssertPtrEquals(tc, u2, u3->next);

    unit_stack(u3, u2);
    // 1, 2->3:
    CuAssertPtrEquals(tc, u2, r->units->next);
    CuAssertPtrEquals(tc, u3, r->units->next->next);

    unit_stack(u2, u1);
    // 1->2->3:
    CuAssertPtrEquals(tc, u1, r->units);
    CuAssertPtrEquals(tc, u2, r->units->next);
    CuAssertPtrEquals(tc, u3, r->units->next->next);

    unit_unstack(u2);
    // 1->3, 2:
    CuAssertPtrEquals(tc, u1, r->units);
    CuAssertPtrEquals(tc, u3, r->units->next);
    CuAssertPtrEquals(tc, u2, r->units->next->next);

    unit_unstack(u3);
    // 1, 3, 2:
    CuAssertPtrEquals(tc, u1, r->units);
    CuAssertPtrEquals(tc, u3, r->units->next);
    CuAssertPtrEquals(tc, u2, r->units->next->next);
}

static void test_wrapmap(CuTest * tc)
{
    region *r;

    cleargame();
    init_terrains();
    config.width = 5;
    config.height = 3;
    create_region(4, 0, get_terrain(T_PLAIN));
    create_region(0, 2, get_terrain(T_PLAIN));
    r = create_region(0, 0, get_terrain(T_PLAIN));
    connectregions();
    CuAssertIntEquals(tc, 2, r->connect[0]->y); /* NORTH */
    CuAssertIntEquals(tc, 4, r->connect[3]->x); /* WEST */
}

static void test_good_password(CuTest * tc)
{
    region * r;
    unit * u;
    faction * f;
    char line[256];
    stream strm;

    cleargame();
    init_terrains();
    turn = 0;
    r = create_region(1, 1, get_terrain(T_PLAIN));
    f = addplayer(r, 0, 0);
    u = r->units;

    faction_setpassword(f, "mypassword");
    CuAssertIntEquals(tc, 0, f->lastorders);
    turn = 1;
    mstream_init(&strm);
    sprintf(line, "FACTION %d mypassword", f->no);
    strm.api->writeln(strm.handle, line);
    strm.api->rewind(strm.handle);
    read_orders(&strm);
    CuAssertPtrEquals(tc, 0, u->orders);
    CuAssertIntEquals(tc, 1, f->lastorders);

    mstream_done(&strm);
}

static void test_quoted_password(CuTest * tc)
{
    region * r;
    unit * u;
    faction * f;
    char line[256];
    stream strm;

    cleargame();
    init_terrains();
    turn = 0;
    r = create_region(1, 1, get_terrain(T_PLAIN));
    f = addplayer(r, 0, 0);
    u = r->units;

    faction_setpassword(f, "mypassword");
    CuAssertIntEquals(tc, 0, f->lastorders);
    turn = 1;
    mstream_init(&strm);
    sprintf(line, "FACTION %d \"mypassword\"", f->no);
    strm.api->writeln(strm.handle, line);
    strm.api->rewind(strm.handle);
    read_orders(&strm);
    CuAssertPtrEquals(tc, 0, u->orders);
    CuAssertIntEquals(tc, 1, f->lastorders);

    mstream_done(&strm);
}

static void test_bad_password(CuTest * tc)
{
    region * r;
    unit * u;
    faction * f;
    char line[256];
    stream strm;

    cleargame();
    init_terrains();
    turn = 0;
    r = create_region(1, 1, get_terrain(T_PLAIN));
    f = addplayer(r, 0, 0);
    faction_setpassword(f, "mypassword");
    u = r->units;

    turn = 1;
    mstream_init(&strm);

    sprintf(line, "FACTION %d badpassword", f->no);
    strm.api->writeln(strm.handle, line);
    strm.api->rewind(strm.handle);
    read_orders(&strm);
    CuAssertPtrEquals(tc, 0, u->orders);
    CuAssertIntEquals(tc, 0, f->lastorders);

    mstream_done(&strm);
}

static void test_password_cmd(CuTest * tc)
{
    region * r;
    unit * u;
    faction * f;
    char line[256];
    stream strm;

    cleargame();
    init_terrains();
    turn = 0;
    r = create_region(1, 1, get_terrain(T_PLAIN));
    f = addplayer(r, 0, 0);
    faction_setpassword(f, "mypassword");
    u = r->units;

    mstream_init(&strm);

    sprintf(line, "FACTION %d mypassword", f->no);
    strm.api->writeln(strm.handle, line);
    sprintf(line, "UNIT %d", u->no);
    strm.api->writeln(strm.handle, line);
    strm.api->writeln(strm.handle, "PASSWORD newpassword");
    strm.api->rewind(strm.handle);
    read_orders(&strm);
    ql_foreach(f->messages, free);
    ql_free(f->messages);
    f->messages = 0;
    processorders();
    CuAssertStrEquals(tc, "The faction's password was changed to 'newpassword'.", (const char *)ql_get(f->messages, 0));
    CuAssertIntEquals(tc, 1, (int)faction_checkpassword(f, "newpassword"));
    CuAssertIntEquals(tc, 0, (int)faction_checkpassword(f, "mypassword"));
    mstream_done(&strm);
}

static void test_form(CuTest * tc)
{
    unit *u;
    region *r;
    faction *f;

    cleargame();
    init_terrains();
    f = create_faction(1);
    r = create_region(0, 0, get_terrain(T_PLAIN));
    u = make_unit(f, r, 1);
    ql_push(&u->orders, _strdup("ENTERTAIN"));
    ql_push(&u->orders, _strdup("FORM 42"));
    ql_push(&u->orders, _strdup("WORK"));
    ql_push(&u->orders, _strdup("END"));
    process_form(u, r);
    CuAssertPtrEquals(tc, 0, r->units->next->next);
    CuAssertIntEquals(tc, 1, ql_length(u->orders));
    CuAssertIntEquals(tc, 0, u->alias);
    CuAssertPtrEquals(tc, u, r->units);

    u = r->units->next;
    CuAssertIntEquals(tc, 1, ql_length(u->orders));
    CuAssertIntEquals(tc, 42, u->alias);
    CuAssertTrue(tc, u->no>0);
}

static void test_orders(CuTest * tc)
{
    region * r;
    unit * u;
    faction * f;
    char line[256];
    stream strm;

    cleargame();
    init_terrains();
    turn = 0;
    r = create_region(1, 1, get_terrain(T_PLAIN));
    f = addplayer(r, 0, 0);
    faction_setpassword(f, "mypassword");
    u = r->units;
    CuAssertPtrNotNull(tc, r);
    CuAssertPtrNotNull(tc, f);
    CuAssertPtrNotNull(tc, u);

    CuAssertIntEquals(tc, 0, f->lastorders);
    turn = 1;
    mstream_init(&strm);
    sprintf(line, "FACTION %d mypassword", f->no);
    strm.api->writeln(strm.handle, line);
    sprintf(line, "UNIT %d", u->no);
    strm.api->writeln(strm.handle, line);
    strm.api->writeln(strm.handle, "STUDY magic");
    strm.api->rewind(strm.handle);
    read_orders(&strm);
    CuAssertPtrNotNull(tc, u->orders);
    CuAssertIntEquals(tc, 1, f->lastorders);
    CuAssertStrEquals(tc, "STUDY magic", (const char *)ql_get(u->orders, 0));
    mstream_done(&strm);
}

static void test_addplayers(CuTest * tc)
{
    region * r;
    unit *u;
    stream strm;
    ql_iter fli;
    int n;

    cleargame();
    init_terrains();
    r = create_region(0, 0, get_terrain(T_FOREST));
    mstream_init(&strm);

    strm.api->writeln(strm.handle, "enno@eressea.de");
    strm.api->writeln(strm.handle, "atlantis@eressea.de");
    strm.api->rewind(strm.handle);

    addplayers(r, &strm);
    CuAssertPtrNotNull(tc, factions);
    CuAssertPtrNotNull(tc, r->units);
    for (u=r->units,fli=qli_init(&factions),n=0;qli_more(fli) && u;++n,u=u->next) {
        faction *f = (faction *)qli_next(&fli);
        CuAssertPtrEquals(tc, u->faction, f);
        CuAssertIntEquals(tc, 1, u->number);
    }
    CuAssertIntEquals(tc, 2, n);
    CuAssertPtrEquals(tc, 0, u);
    CuAssertTrue(tc, !qli_more(fli));
}

static void test_addplayer(CuTest * tc)
{
    region * r;
    faction * f;
    unit * u;
    const char * email = "enno@example.com";

    cleargame();
    init_terrains();
    turn = 1;
    r = create_region(1, 1, get_terrain(T_PLAIN));
    f = addplayer(r, email, 0);
    u = r->units;

    CuAssertPtrNotNull(tc, r);
    CuAssertPtrNotNull(tc, f);
    CuAssertPtrNotNull(tc, u);
    CuAssertStrEquals(tc, email, faction_getaddr(f));
    CuAssertIntEquals(tc, r->x, f->origin_x);
    CuAssertIntEquals(tc, r->y, f->origin_y);
    CuAssertPtrNotNull(tc, faction_getpwhash(f));
    CuAssertStrEquals(tc, "", u->thisorder);
    CuAssertStrEquals(tc, keywords[K_WORK], u->lastorder);
    CuAssertPtrEquals(tc, 0, u->orders);
}

static void test_origin(CuTest * tc)
{
    region *r1, *r2, *r3;
    faction * f;
    
    cleargame();
    init_terrains();
    r1 = create_region(1, 1, get_terrain(T_PLAIN));
    r2 = create_region(1, 2, get_terrain(T_OCEAN));
    r3 = create_region(2, 2, get_terrain(T_PLAIN));
    region_setname(r1, "foo");
    region_setname(r3, "bar");
    update_world(0, 0, 2, 2);
    f = addplayer(r1, "enno@example.com", 0);
    CuAssertStrEquals(tc, "foo (0,0)", regionid(r1, f));
    CuAssertStrEquals(tc, "(0,1)", regionid(r2, f));
    CuAssertStrEquals(tc, "bar (1,1)", regionid(r3, f));
}

static void test_createregion(CuTest * tc)
{
    region * r;

    cleargame();
    init_terrains();
    r = create_region(1, 1, get_terrain(T_OCEAN));
    CuAssertPtrNotNull(tc, r);
    CuAssertPtrEquals(tc, 0, (void *)region_getname(r));

    r = create_region(1, 2, get_terrain(T_PLAIN));
    CuAssertPtrNotNull(tc, r);
    CuAssertPtrEquals(tc, 0, (void *)region_getname(r));
    CuAssertPtrEquals(tc, r, findregion(1, 2));
    CuAssertIntEquals(tc, 1, r->x);
    CuAssertIntEquals(tc, 2, r->y);
    CuAssertIntEquals(tc, 0, r->peasants);
    CuAssertPtrEquals(tc, (void *)get_terrain(T_PLAIN), (void *)r->terrain);
}

static void test_readwrite(CuTest * tc)
{
    region * r;
    faction * f;
    unit * u;
    int fno = 1, uno = 1, err;

    errno = 0;
    cleargame();
    init_terrains();
    f = create_faction(fno);
    r = create_region(0, 0, get_terrain(T_PLAIN));
    u = make_unit(f, r, uno);
    CuAssertPtrEquals(tc, f, findfaction(fno));
    CuAssertPtrEquals(tc, u, findunitg(uno));
    CuAssertPtrEquals(tc, r, findregion(0, 0));

    err = writegame();
    CuAssertIntEquals(tc, 0, err);
    CuAssertIntEquals(tc, 0, errno);

    cleargame();
    init_terrains();
    CuAssertPtrEquals(tc, 0, findfaction(fno));
    CuAssertPtrEquals(tc, 0, findunitg(uno));
    CuAssertPtrEquals(tc, 0, findregion(0, 0));

    CuAssertIntEquals(tc, 0, errno);
    err = readgame();
    CuAssertIntEquals(tc, 0, err);
    CuAssertIntEquals(tc, 0, errno);
    CuAssertPtrNotNull(tc, findfaction(fno));
    CuAssertPtrNotNull(tc, findunitg(uno));
    CuAssertPtrNotNull(tc, findregion(0, 0));
}

static void test_fileops(CuTest * tc)
{
    faction * f = 0;
    region * r;
    int fno;

    cleargame();
    init_terrains();

    r = create_region(0, 0, get_terrain(T_PLAIN));
    r = create_region(2, 3, get_terrain(T_PLAIN));
    f = addplayer(r, "enno@example.com", 0);

    CuAssertPtrNotNull(tc, r);
    CuAssertPtrNotNull(tc, factions);
    CuAssertPtrNotNull(tc, f);
    fno = f->no;
    writegame();
    cleargame();
    init_terrains();
    CuAssertPtrEquals(tc, 0, findregion(0, 0));

    CuAssertIntEquals(tc, 0, readgame());
    CuAssertIntEquals(tc, 0, turn);
    CuAssertPtrNotNull(tc, findregion(0, 0));

    f = findfaction(fno);
    CuAssertPtrNotNull(tc, f);
    CuAssertIntEquals(tc, 2, f->origin_x);
    CuAssertIntEquals(tc, 3, f->origin_y);
}

static void test_directions(CuTest * tc)
{
    CuAssertIntEquals(tc, K_NORTH, findkeyword("NORTH"));
    CuAssertIntEquals(tc, K_NORTH, findkeyword("N"));
    CuAssertIntEquals(tc, K_SOUTH, findkeyword("SOUTH"));
    CuAssertIntEquals(tc, K_SOUTH, findkeyword("S"));
    CuAssertIntEquals(tc, K_EAST, findkeyword("EAST"));
    CuAssertIntEquals(tc, K_EAST, findkeyword("E"));
    CuAssertIntEquals(tc, K_WEST, findkeyword("WEST"));
    CuAssertIntEquals(tc, K_WEST, findkeyword("W"));
    CuAssertIntEquals(tc, K_MIR, findkeyword("MIR"));
    CuAssertIntEquals(tc, K_MIR, findkeyword("M"));
    CuAssertIntEquals(tc, K_YDD, findkeyword("YDD"));
    CuAssertIntEquals(tc, K_YDD, findkeyword("Y"));
}

static void test_owners(CuTest * tc) {
    region *r;
    unit *u;
    ship *s;
    faction *f;

    cleargame();
    init_terrains();
    f = create_faction(1);
    r = create_region(0, 0, get_terrain(T_PLAIN));
    u = make_unit(f, r, 1);
    s = create_ship(1, SH_LONGBOAT);
    CuAssertPtrEquals(tc, 0, shipowner(r, s));
    u->ship = s;
    CuAssertPtrEquals(tc, u, shipowner(r, s));
    u = make_unit(f, r, 2);
    u->ship = s;
    u->owner = true;
    CuAssertPtrEquals(tc, u, shipowner(r, s));
}

static void test_movewhere(CuTest * tc)
{
    region *r, *c;
    char buf[256];
    int d;

    cleargame();
    init_terrains();
    config.width = 10;
    config.height = 10;
    c = create_region(1, 1, get_terrain(T_PLAIN));
    for (d=0;d!=MAXDIRECTIONS;++d) {
        int x = 1, y = 1;
        transform(&x, &y, d);
        r = create_region(x, y, get_terrain(T_PLAIN));
    }
    connectregions();

    sprintf(buf, "%s %s", keywords[K_MOVE], keywords[K_NORTH]);
    CuAssertIntEquals(tc, K_MOVE, igetkeyword(buf));
    r = movewhere(c);
    CuAssertPtrNotNull(tc, r);
    CuAssertPtrEquals(tc, r, c->connect[0]);
    CuAssertPtrEquals(tc, c, r->connect[1]);

    sprintf(buf, "%s %s", keywords[K_MOVE], keywords[K_SOUTH]);
    CuAssertIntEquals(tc, K_MOVE, igetkeyword(buf));
    r = movewhere(c);
    CuAssertPtrNotNull(tc, r);
    CuAssertPtrEquals(tc, r, c->connect[1]);
    CuAssertPtrEquals(tc, c, r->connect[0]);

    sprintf(buf, "%s %s", keywords[K_MOVE], keywords[K_EAST]);
    CuAssertIntEquals(tc, K_MOVE, igetkeyword(buf));
    r = movewhere(c);
    CuAssertPtrNotNull(tc, r);
    CuAssertPtrEquals(tc, r, c->connect[2]);
    CuAssertPtrEquals(tc, c, r->connect[3]);

    sprintf(buf, "%s %s", keywords[K_MOVE], keywords[K_WEST]);
    CuAssertIntEquals(tc, K_MOVE, igetkeyword(buf));
    r = movewhere(c);
    CuAssertPtrNotNull(tc, r);
    CuAssertPtrEquals(tc, r, c->connect[3]);
    CuAssertPtrEquals(tc, c, r->connect[2]);

#if MAXDIRECTIONS>5
    sprintf(buf, "%s %s", keywords[K_MOVE], keywords[K_MIR]);
    CuAssertIntEquals(tc, K_MOVE, igetkeyword(buf));
    r = movewhere(c);
    CuAssertPtrNotNull(tc, r);
    CuAssertPtrEquals(tc, r, c->connect[4]);
    CuAssertPtrEquals(tc, c, r->connect[5]);

    sprintf(buf, "%s %s", keywords[K_MOVE], keywords[K_YDD]);
    CuAssertIntEquals(tc, K_MOVE, igetkeyword(buf));
    r = movewhere(c);
    CuAssertPtrNotNull(tc, r);
    CuAssertPtrEquals(tc, r, c->connect[5]);
    CuAssertPtrEquals(tc, c, r->connect[4]);
#endif
}

static void test_transform(CuTest * tc)
{
    int x, y;
  
    x = 0, y = 1;
    CuAssertIntEquals(tc, EINVAL, transform(&x, &y, MAXDIRECTIONS));
    CuAssertIntEquals(tc, 0, transform(&x, &y, 0));
    CuAssertIntEquals(tc, 0, x);
    CuAssertIntEquals(tc, 0, y);

    x = 0, y = -1;
    CuAssertIntEquals(tc, 0, transform(&x, &y, 1));
    CuAssertIntEquals(tc, 0, x);
    CuAssertIntEquals(tc, 0, y);

    x = -1, y = 0;
    CuAssertIntEquals(tc, 0, transform(&x, &y, 2));
    CuAssertIntEquals(tc, 0, x);
    CuAssertIntEquals(tc, 0, y);

    x = 1, y = 0;
    CuAssertIntEquals(tc, 0, transform(&x, &y, 3));
    CuAssertIntEquals(tc, 0, x);
    CuAssertIntEquals(tc, 0, y);

#if MAXDIRECTIONS>5
    x = 1, y = 1;
    CuAssertIntEquals(tc, 0, transform(&x, &y, 4));
    CuAssertIntEquals(tc, 0, x);
    CuAssertIntEquals(tc, 0, y);

    x = -1, y = -1;
    CuAssertIntEquals(tc, 0, transform(&x, &y, 5));
    CuAssertIntEquals(tc, 0, x);
    CuAssertIntEquals(tc, 0, y);
#endif
}

static void test_region_name(CuTest * tc)
{
    const char * name = "Sacco & Vanzetti";
    region * r;

    cleargame();
    init_terrains();
    r = create_region(0, 0, get_terrain(T_PLAIN));
    CuAssertPtrEquals(tc, 0, (void *)region_getname(r));
    region_setname(r, name);
    CuAssertStrEquals(tc, name, region_getname(r));
    writegame();
    cleargame();
    init_terrains();
    readgame();
    r = findregion(0, 0);
    CuAssertStrEquals(tc, name, region_getname(r));
}

static void test_faction_name(CuTest * tc)
{
    const char * name = "Sacco & Vanzetti";
    faction * f;
    region * r;
    unit * u;

    cleargame();
    init_terrains();
    f = create_faction(1);
    r = create_region(0, 0, get_terrain(T_PLAIN));
    u = make_unit(f, r, 1);
    CuAssertPtrEquals(tc, 0, (void *)faction_getname(f));
    faction_setname(f, name);
    CuAssertStrEquals(tc, name, faction_getname(f));
    writegame();
    cleargame();
    init_terrains();
    readgame();
    f = findfaction(1);
    CuAssertStrEquals(tc, name, faction_getname(f));
}


static void test_moneypool(CuTest *tc) {
    unit *u1, *u2;
    region *r;
    faction *f;

    cleargame();
    init_terrains();
    config.upkeep = 10;
    r = create_region(0, 0, get_terrain(T_PLAIN));
    f = create_faction(1);
    u1 = create_unit(f, 1);
    u1->number = 1;
    region_addunit(r, u1, 0);
    u2 = create_unit(f, 2);
    u2->number = 1;
    region_addunit(r, u2, 0);
    u1->money = 100;
    u2->money = 100;
    processorders();
    CuAssertIntEquals(tc, 90, u1->money);
    CuAssertIntEquals(tc, 90, u2->money);
}

static void test_createterrain(CuTest * tc)
{
    terrain * t = create_terrain("ocean");
    CuAssertPtrEquals(tc, t, get_terrain_by_name("ocean"));
    CuAssertPtrEquals(tc, t, get_terrain(T_OCEAN));
}

static void test_connectregions(CuTest * tc)
{
    region *r1, *r2;

    cleargame();
    init_terrains();
    config.width = 3;
    config.height = 3;
    r1 = create_region(0, 0, get_terrain(T_PLAIN));
    r2 = create_region(0, 1, get_terrain(T_PLAIN));
    connectregions();
    CuAssertPtrEquals(tc, r1, r2->connect[0]);
    CuAssertPtrEquals(tc, 0, r1->connect[0]);
    CuAssertPtrEquals(tc, r2, r1->connect[1]);
    CuAssertPtrEquals(tc, 0, r1->connect[2]);
    CuAssertPtrEquals(tc, 0, r1->connect[3]);
}

static void test_cfg_upkeep(CuTest * tc)
{
    unit *u1;
    faction *f1;
    region *r1;

    cleargame();
    init_terrains();
    config.upkeep = 0;
    r1 = create_region(0, 0, get_terrain(T_PLAIN));
    f1 = create_faction(1);
    u1 = create_unit(f1, 1);
    u1->number = 2;
    u1->money = 50;
    region_addunit(r1, u1, 0);

    config.upkeep = 15;
    processorders();
    CuAssertIntEquals(tc, 20, u1->money);

    config.upkeep = 0;
    processorders();
    CuAssertIntEquals(tc, 20, u1->money);
}

static void test_cfg_moves_on(CuTest * tc)
{
    unit *u1;
    faction *f1;
    region *r1, *r2;
    stream strm;
    char line[64];

    cleargame();
    config.upkeep = 0;
    config.features = CFG_MOVES;
    config.moves = 2;
    r1 = create_region(0, 0, get_terrain(T_PLAIN));
    r2 = create_region(0, 1, get_terrain(T_PLAIN));
    r2 = create_region(0, 2, get_terrain(T_PLAIN));
    update_world(0, 0, 2, 2);
    connectregions();
    f1 = create_faction(1);
    u1 = create_unit(f1, 1);
    u1->number = 1;
    region_addunit(r1, u1, 0);

    faction_setpassword(f1, "mypassword");
    mstream_init(&strm);
    sprintf(line, "FACTION %d mypassword", f1->no);
    strm.api->writeln(strm.handle, line);
    sprintf(line, "UNIT %d", u1->no);
    strm.api->writeln(strm.handle, line);
    strm.api->writeln(strm.handle, "MOVE S S");
    strm.api->rewind(strm.handle);

    read_orders(&strm);
    processorders();
    CuAssertPtrEquals(tc, r2, u1->region);
}

static void test_cfg_moves_off(CuTest * tc)
{
    unit *u1;
    faction *f1;
    region *r1, *r2;
    stream strm;
    char line[64];

    cleargame();
    config.features = 0;
    config.moves = 0;
    r1 = create_region(0, 0, get_terrain(T_PLAIN));
    r2 = create_region(0, 2, get_terrain(T_PLAIN));
    r2 = create_region(0, 1, get_terrain(T_PLAIN));
    update_world(0, 0, 2, 2);
    connectregions();
    f1 = create_faction(1);
    u1 = create_unit(f1, 1);
    u1->number = 1;
    region_addunit(r1, u1, 0);

    faction_setpassword(f1, "mypassword");
    mstream_init(&strm);
    sprintf(line, "FACTION %d mypassword", f1->no);
    strm.api->writeln(strm.handle, line);
    sprintf(line, "UNIT %d", u1->no);
    strm.api->writeln(strm.handle, line);
    strm.api->writeln(strm.handle, "MOVE S S");
    strm.api->rewind(strm.handle);

    read_orders(&strm);
    processorders();
    CuAssertPtrEquals(tc, r2, u1->region);
}

static void test_region_addunit(CuTest * tc)
{
    unit *u1, *u2;
    region * r;

    cleargame();
    init_terrains();
    r = create_region(0, 0, get_terrain(T_PLAIN));
    u1 = create_unit(0, 1);
    u2 = create_unit(0, 1);
    region_addunit(r, u1, 0);
    CuAssertPtrEquals(tc, r, u1->region);
    CuAssertPtrEquals(tc, u1, r->units);

    region_addunit(r, u2, 0);
    CuAssertPtrEquals(tc, u1, r->units);
    CuAssertPtrEquals(tc, u2, r->units->next);

}

static void test_region_addunit_building(CuTest * tc)
{
    unit *u1, *u2;
    building *b1;
    region * r;

    cleargame();
    init_terrains();
    r = create_region(0, 0, get_terrain(T_PLAIN));
    u1 = create_unit(0, 1);
    u2 = create_unit(0, 1);
    b1 = create_building(1);
    ql_push(&r->buildings, b1);

    region_addunit(r, u1, 0);
    CuAssertPtrEquals(tc, r, u1->region);
    CuAssertPtrEquals(tc, u1, r->units);

    u2->building = b1;
    region_addunit(r, u2, 0);
    CuAssertPtrEquals(tc, r, u2->region);
    CuAssertPtrEquals(tc, u2, r->units);
    CuAssertPtrEquals(tc, u1, u2->next);

}

static void test_unit_name(CuTest * tc)
{
    const char * name = "Sacco & Vanzetti";
    unit * u;
    faction * f;
    region * r;

    cleargame();
    init_terrains();
    r = create_region(0, 0, get_terrain(T_PLAIN));
    f = create_faction(1);
    u = make_unit(f, r, 1);
    unit_setname(u, name);
    CuAssertStrEquals(tc, name, unit_getname(u));
    writegame();
    cleargame();
    init_terrains();
    readgame();
    u = findunitg(1);
    CuAssertPtrNotNull(tc, u);
    CuAssertStrEquals(tc, name, unit_getname(u));
}

static void test_remove_empty(CuTest * tc)
{
    unit * u;
    faction * f;
    region * r;

    cleargame();
    init_terrains();
    r = create_region(0, 0, get_terrain(T_PLAIN));
    f = create_faction(1);
    u = make_unit(f, r, 1);
    u->number = 0;
    writegame();
    cleargame();
    init_terrains();
    readgame();
    CuAssertPtrEquals(tc, 0, findunitg(1));
    CuAssertPtrEquals(tc, 0, findfaction(1));
}

static void test_unit_display(CuTest * tc)
{
    const char * display = "Sacco & Vanzetti";
    unit * u;
    faction * f;
    region * r;

    cleargame();
    init_terrains();
    r = create_region(0, 0, get_terrain(T_PLAIN));
    f = create_faction(1);
    u = make_unit(f, r, 1);
    unit_setdisplay(u, display);
    CuAssertStrEquals(tc, display, unit_getdisplay(u));
    writegame();
    cleargame();
    init_terrains();
    readgame();
    u = findunitg(1);
    CuAssertStrEquals(tc, display, unit_getdisplay(u));
}

static void test_faction_addr(CuTest * tc)
{
    const char * addr = "enno@example.com";
    faction * f;
	unit *u;
	region *r;

    cleargame();
    init_terrains();
    f = create_faction(1);
    r = create_region(0, 0, get_terrain(T_PLAIN));
    u = make_unit(f, r, 1);
    CuAssertPtrEquals(tc, 0, (void *)faction_getaddr(f));
    faction_setaddr(f, addr);
    CuAssertStrEquals(tc, addr, faction_getaddr(f));
    writegame();
    cleargame();
    init_terrains();
    readgame();
    f = findfaction(1);
    CuAssertStrEquals(tc, addr, faction_getaddr(f));
}

static void test_faction_password(CuTest * tc)
{
    const char * password = "friendshipismagic";
    const char * salted_hash = "0daa920e651e3250fbd8e68980a38b554f1b";
    faction * f;

    cleargame();
    f = create_faction(1);
    faction_setpassword(f, password);
    CuAssertTrue(tc, faction_checkpassword(f, password));
    CuAssertTrue(tc, !faction_checkpassword(f, password+1));
    faction_setpassword(f, password+1);
    CuAssertTrue(tc, faction_checkpassword(f, password+1));
    faction_setpassword(f, password+2);
    CuAssertTrue(tc, faction_checkpassword(f, password+2));

    faction_setpwhash(f, salted_hash);
    CuAssertStrEquals(tc, salted_hash, faction_getpwhash(f));
    CuAssertTrue(tc, faction_checkpassword(f, password));

}

static void test_keywords(CuTest * tc)
{
    CuAssertIntEquals(tc, K_ACCEPT, findkeyword("accept"));
    CuAssertIntEquals(tc, K_NAME, findkeyword("name"));
    CuAssertIntEquals(tc, K_STACK, findkeyword("stack"));
    CuAssertIntEquals(tc, K_UNSTACK, findkeyword("unstack"));
    CuAssertIntEquals(tc, K_WORK, findkeyword("work"));
}

static void test_freeunit(CuTest * tc)
{
    faction *f;
    region *r;
    unit *u1, *u2, *u3;
    
    cleargame();
    init_terrains();
    r = create_region(1, 1, get_terrain(T_PLAIN));
    f = create_faction(1);
    u1 = make_unit(f, r, 1);
    u2 = make_unit(f, r, 2);
    u3 = make_unit(f, r, 3);
    
    CuAssertPtrEquals(tc, 0, u2->stack);
    CuAssertPtrEquals(tc, 0, u3->stack);
    region_rmunit(r, u1, 0);
    free_unit(u1);
    CuAssertPtrEquals(tc, 0, u2->stack);
    CuAssertPtrEquals(tc, 0, u3->stack);
}

static void test_shipbuilding(CuTest * tc)
{
    region * r;
    unit *u;
    faction * f;
    ship * sh;
    char line[256];
    stream strm;

    cleargame();
    init_terrains();
    turn = 0;
    r = create_region(1, 1, get_terrain(T_PLAIN));
    f = create_faction(1);
    faction_setpassword(f, "mypassword");
    u = make_unit(f, r, 1);
    u->number = 10;
    u->money = 10 * u->number;
    u->skills[SK_SHIPBUILDING] = 90 * u->number;
    u->items[I_WOOD] = 100;
    mstream_init(&strm);

    sprintf(line, "FACTION %d mypassword", f->no);
    strm.api->writeln(strm.handle, line);
    strm.api->writeln(strm.handle, "UNIT 1");
    strm.api->writeln(strm.handle, "BUILD LONGBOAT");

    strm.api->rewind(strm.handle);
    read_orders(&strm);
    processorders();
    sh = (ship *)ql_get(r->ships, 0);
    CuAssertPtrNotNull(tc, sh);
    CuAssertPtrEquals(tc, sh, u->ship);
    CuAssertIntEquals(tc, 80, sh->left);
    CuAssertIntEquals(tc, SH_LONGBOAT, sh->type);
    CuAssertIntEquals(tc, 100 * u->number, u->skills[SK_SHIPBUILDING]);
    CuAssertIntEquals(tc, 80, u->items[I_WOOD]);

    mstream_done(&strm);
}

static void test_config_stacks(CuTest * tc)
{
    region * r;
    unit *u1, *u2;
    faction * f;
    char line[256];
    stream strm;

    cleargame();
    init_terrains();
    turn = 0;
    r = create_region(1, 1, get_terrain(T_PLAIN));
    f = create_faction(1);
    faction_setpassword(f, "mypassword");
    u1 = make_unit(f, r, 1);
    u2 = make_unit(f, r, 2);

    mstream_init(&strm);

    sprintf(line, "FACTION %d mypassword", f->no);
    strm.api->writeln(strm.handle, line);
    strm.api->writeln(strm.handle, "UNIT 1");
    strm.api->writeln(strm.handle, "STACK 2");
    strm.api->writeln(strm.handle, "WORK");
    strm.api->writeln(strm.handle, "UNIT 2");
    strm.api->writeln(strm.handle, "WORK");

    config.features = CFG_STACKS;
    strm.api->rewind(strm.handle);
    read_orders(&strm);
    processorders();
    CuAssertPtrEquals(tc, u2, unit_getstack(u1));
    unit_unstack(u1);
    CuAssertPtrEquals(tc, u1, unit_getstack(u1));

    config.features = 0;
    strm.api->rewind(strm.handle);
    read_orders(&strm);
    ql_foreach(f->messages, free);
    ql_free(f->messages);
    processorders();
    CuAssertPtrEquals(tc, u1, unit_getstack(u1));

    mstream_done(&strm);
}

static void test_config_teachers(CuTest * tc)
{
    region * r;
    unit *u1, *u2;
    faction * f;
    char line[256];
    stream strm;

    cleargame();
    init_terrains();
    turn = 0;
    r = create_region(1, 1, get_terrain(T_PLAIN));
    f = create_faction(1);
    faction_setpassword(f, "mypassword");
    u1 = make_unit(f, r, 1);
    u2 = make_unit(f, r, 2);
    u1->money = 20;
    u2->money = 20;

    mstream_init(&strm);

    sprintf(line, "FACTION %d mypassword", f->no);
    strm.api->writeln(strm.handle, line);
    strm.api->writeln(strm.handle, "UNIT 1");
    strm.api->writeln(strm.handle, "TEACH 2");
    strm.api->writeln(strm.handle, "UNIT 2");
    strm.api->writeln(strm.handle, "STUDY LONGBOW");

    config.features = CFG_TEACHERS;
    u1->skills[SK_LONGBOW] = 30;
    u2->skills[SK_LONGBOW] = 0;
    strm.api->rewind(strm.handle);
    read_orders(&strm);
    processorders();
    CuAssertIntEquals(tc, 60, u2->skills[SK_LONGBOW]);

    config.features = 0;
    u1->skills[SK_LONGBOW] = 30;
    u2->skills[SK_LONGBOW] = 0;
    strm.api->rewind(strm.handle);
    read_orders(&strm);
    processorders();
    CuAssertIntEquals(tc, 30, u2->skills[SK_LONGBOW]);

    mstream_done(&strm);
}

static void test_config_terrain(CuTest * tc)
{
    cJSON *json = cJSON_Parse("{ \"terrain\": [ {\"name\": \"swamp\", \"work\": 13, \"food\": 1000, \"output\" : [1, 2, 3, 4] } ] }");
    const terrain * t;

    cleargame();
    read_config_json(json);
    t = get_terrain_by_name("swamp");
    CuAssertPtrNotNull(tc, t);
    CuAssertIntEquals(tc, 13, t->foodproductivity);
    CuAssertIntEquals(tc, 1000, t->maxfoodoutput);
    CuAssertIntEquals(tc, 1, t->maxoutput[I_IRON]);
    CuAssertIntEquals(tc, 2, t->maxoutput[I_WOOD]);
    CuAssertIntEquals(tc, 3, t->maxoutput[I_STONE]);
    CuAssertIntEquals(tc, 4, t->maxoutput[I_HORSE]);
    cJSON_Delete(json);
}

static void test_config_json(CuTest * tc)
{
    cJSON *json = cJSON_Parse("{ \"upkeep\": 13, \"moves\": 2, \"width\": 10, \"height\": 20, \"stacks\": true, \"teachers\": true }");

    cleargame();
    read_config_json(json);
    CuAssertIntEquals(tc, 10, config.width);
    CuAssertIntEquals(tc, 20, config.height);
    CuAssertIntEquals(tc, 2, config.moves);
    CuAssertIntEquals(tc, 13, config.upkeep);
    CuAssertIntEquals(tc, CFG_MOVES, config.features&CFG_MOVES);
    CuAssertIntEquals(tc, CFG_STACKS, config.features&CFG_STACKS);
    CuAssertIntEquals(tc, CFG_TEACHERS, config.features&CFG_TEACHERS);
    cJSON_Delete(json);
}

int main(void)
{
    CuString *output = CuStringNew();
    CuSuite *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_createterrain);
    SUITE_ADD_TEST(suite, test_connectregions);
    SUITE_ADD_TEST(suite, test_moneypool);
    SUITE_ADD_TEST(suite, test_cfg_upkeep);
    // SUITE_ADD_TEST(suite, test_cfg_moves_on);
    // SUITE_ADD_TEST(suite, test_cfg_moves_off);
    SUITE_ADD_TEST(suite, test_region_addunit);
    SUITE_ADD_TEST(suite, test_addunit_takes_hint);
    SUITE_ADD_TEST(suite, test_region_addunit_building);
    SUITE_ADD_TEST(suite, test_addunit_order_new_ship);
    SUITE_ADD_TEST(suite, test_addunit_order_ships);
    SUITE_ADD_TEST(suite, test_addunit_order_buildings);
    SUITE_ADD_TEST(suite, test_addunit_order_buildings_mixed);
    SUITE_ADD_TEST(suite, test_addunit_order_mixed);
    SUITE_ADD_TEST(suite, test_addunit_reorders);
    SUITE_ADD_TEST(suite, test_setship_reorders);
    SUITE_ADD_TEST(suite, test_setbuilding_reorders);
    SUITE_ADD_TEST(suite, test_form);
    SUITE_ADD_TEST(suite, test_keywords);
    SUITE_ADD_TEST(suite, test_wrapmap);
    SUITE_ADD_TEST(suite, test_addplayer);
    SUITE_ADD_TEST(suite, test_orders);
    SUITE_ADD_TEST(suite, test_good_password);
    SUITE_ADD_TEST(suite, test_quoted_password);
    SUITE_ADD_TEST(suite, test_bad_password);
    SUITE_ADD_TEST(suite, test_password_cmd);
    SUITE_ADD_TEST(suite, test_addplayers);
    SUITE_ADD_TEST(suite, test_fileops);
    SUITE_ADD_TEST(suite, test_faction_password);
    SUITE_ADD_TEST(suite, test_readwrite);
    SUITE_ADD_TEST(suite, test_createregion);
    SUITE_ADD_TEST(suite, test_transform);
    SUITE_ADD_TEST(suite, test_owners);
    SUITE_ADD_TEST(suite, test_movewhere);
    SUITE_ADD_TEST(suite, test_origin);
    SUITE_ADD_TEST(suite, test_region_name);
    SUITE_ADD_TEST(suite, test_unit_name);
    SUITE_ADD_TEST(suite, test_remove_empty);
    SUITE_ADD_TEST(suite, test_unit_display);
    SUITE_ADD_TEST(suite, test_faction_name);
    SUITE_ADD_TEST(suite, test_faction_addr);
    SUITE_ADD_TEST(suite, test_stacking);
    SUITE_ADD_TEST(suite, test_unstack_leader);
    SUITE_ADD_TEST(suite, test_stacking_moves_units);
    SUITE_ADD_TEST(suite, test_config_json);
    SUITE_ADD_TEST(suite, test_config_terrain);
    SUITE_ADD_TEST(suite, test_config_stacks);
    SUITE_ADD_TEST(suite, test_config_teachers);
    SUITE_ADD_TEST(suite, test_shipbuilding);
    SUITE_ADD_TEST(suite, test_freeunit);

    SUITE_ADD_TEST(suite, test_unit_reordering);

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
    return suite->failCount;
}
