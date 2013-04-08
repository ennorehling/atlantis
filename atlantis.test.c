#include "atlantis.h"
#include "keywords.h"
#include "region.h"
#include "faction.h"
#include "unit.h"

#include <CuTest.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

static void test_addplayer(CuTest * tc)
{
    region * r;
    faction * f;
    const char * email = "enno@example.com";

    cleargame();
    r = create_region(1, 1, T_PLAIN);
    f = addplayer(r, email, 0);
    CuAssertStrEquals(tc, email, faction_getaddr(f));
    CuAssertIntEquals(tc, r->x, f->origin_x);
    CuAssertIntEquals(tc, r->y, f->origin_y);
    CuAssertPtrNotNull(tc, r->units);
}

static void test_origin(CuTest * tc)
{
    region * r;
    faction * f;
    
    cleargame();
    r = create_region(1, 1, T_PLAIN);
    region_setname(r, "foo");
    f = addplayer(r, "enno@example.com", 0);
    CuAssertStrEquals(tc, "foo (0,0)", regionid(r, f));

    r = create_region(1, 2, T_OCEAN);
    CuAssertStrEquals(tc, "(0,1)", regionid(r, f));

    r = create_region(2, 2, T_PLAIN);
    region_setname(r, "bar");
    CuAssertStrEquals(tc, "bar (1,1)", regionid(r, f));
}

static void test_createregion(CuTest * tc)
{
    region * r;

    cleargame();
    r = create_region(1, 1, T_OCEAN);
    CuAssertPtrNotNull(tc, r);
    CuAssertPtrEquals(tc, 0, (void *)region_getname(r));

    r = create_region(1, 2, T_PLAIN);
    CuAssertPtrNotNull(tc, r);
    CuAssertPtrEquals(tc, 0, (void *)region_getname(r));
    CuAssertPtrEquals(tc, r, findregion(1, 2));
    CuAssertIntEquals(tc, 1, r->x);
    CuAssertIntEquals(tc, 2, r->y);
    CuAssertIntEquals(tc, 0, r->peasants);
    CuAssertIntEquals(tc, T_PLAIN, r->terrain);
}

static void test_makeblock(CuTest * tc)
{
    region * r;
    int d, x, y;

    cleargame();
    makeblock(0, 0);
    r = findregion(1, 1);
    for (d=0;d!=MAXDIRECTIONS;++d) {
        int x = r->x, y = r->y;
        transform(&x, &y, d);
        CuAssertPtrNotNull(tc, r->connect[d]);
        CuAssertIntEquals(tc, x, r->connect[d]->x);
        CuAssertIntEquals(tc, y, r->connect[d]->y);
    }
    for (x = 0; x != BLOCKSIZE + BLOCKBORDER * 2; x++) {
        for (y = 0; y != BLOCKSIZE + BLOCKBORDER * 2; y++) {
            r = findregion(x, y);
            CuAssertPtrNotNull(tc, r);
            CuAssertIntEquals(tc, x, r->x);
            CuAssertIntEquals(tc, y, r->y);
        }
    }
    r = findregion(-1, -1);
    CuAssertPtrEquals(tc, 0, r);
    r = findregion(BLOCKSIZE + BLOCKBORDER * 2, BLOCKSIZE + BLOCKBORDER * 2);
    CuAssertPtrEquals(tc, 0, r);
}

static void test_readwrite(CuTest * tc)
{
    region * r;
    faction * f;
    unit * u;
    int fno = 1, uno = 1, err;

    cleargame();
    f = create_faction(fno);
    u = create_unit(f, uno);
    r = create_region(0, 0, T_PLAIN);
    region_addunit(r, u);
    CuAssertPtrEquals(tc, f, findfaction(fno));
    CuAssertPtrEquals(tc, u, findunitg(uno));
    CuAssertPtrEquals(tc, r, findregion(0, 0));
    err = writegame();
    CuAssertIntEquals(tc, 0, err);
    CuAssertIntEquals(tc, 0, errno);

    cleargame();
    CuAssertPtrEquals(tc, 0, findfaction(fno));
    CuAssertPtrEquals(tc, 0, findunitg(uno));
    CuAssertPtrEquals(tc, 0, findregion(0, 0));

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
    int x = 0, y = 0, fno;

    turn = -1;
    cleargame();
    initgame();

    for (r=regions;r;r=r->next) {
         if (r->terrain!=T_OCEAN) {
             x = r->x, y = r->y;
             f = addplayer(r, "enno@example.com", 0);
             break;
         }
    }
    CuAssertPtrNotNull(tc, factions);
    CuAssertPtrNotNull(tc, f);
    fno = f->no;
    CuAssertPtrNotNull(tc, r);
    writegame();
    cleargame();
    CuAssertPtrEquals(tc, 0, findregion(0, 0));

    readgame();
    CuAssertIntEquals(tc, 0, turn);
    CuAssertPtrNotNull(tc, findregion(0, 0));

    f = findfaction(fno);
    CuAssertPtrNotNull(tc, f);
    CuAssertIntEquals(tc, x, f->origin_x);
    CuAssertIntEquals(tc, y, f->origin_y);
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

static void test_movewhere(CuTest * tc)
{
    region *r, *c;
    makeblock(0, 0);
    c = findregion(0, 0);

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
    CuAssertIntEquals(tc, EINVAL, transform(&x, &y, K_ENTER));
    CuAssertIntEquals(tc, 0, transform(&x, &y, K_NORTH));
    CuAssertIntEquals(tc, x, 0);
    CuAssertIntEquals(tc, y, 0);

    x = 0, y = -1;
    CuAssertIntEquals(tc, 0, transform(&x, &y, K_SOUTH));
    CuAssertIntEquals(tc, x, 0);
    CuAssertIntEquals(tc, y, 0);

    x = 1, y = 0;
    CuAssertIntEquals(tc, 0, transform(&x, &y, K_WEST));
    CuAssertIntEquals(tc, x, 0);
    CuAssertIntEquals(tc, y, 0);

    x = -1, y = 0;
    CuAssertIntEquals(tc, 0, transform(&x, &y, K_EAST));
    CuAssertIntEquals(tc, x, 0);
    CuAssertIntEquals(tc, y, 0);

#if MAXDIRECTIONS>5
    x = 1, y = 1;
    CuAssertIntEquals(tc, 0, transform(&x, &y, K_MIR));
    CuAssertIntEquals(tc, x, 0);
    CuAssertIntEquals(tc, y, 0);

    x = -1, y = -1;
    CuAssertIntEquals(tc, 0, transform(&x, &y, K_YDD));
    CuAssertIntEquals(tc, x, 0);
    CuAssertIntEquals(tc, y, 0);
#endif
}

static void test_region_name(CuTest * tc)
{
    const char * name = "Sacco & Vanzetti";
    region * r;

    cleargame();
    r = create_region(0, 0, T_PLAIN);
    CuAssertPtrEquals(tc, 0, (void *)region_getname(r));
    region_setname(r, name);
    CuAssertStrEquals(tc, name, region_getname(r));
    writegame();
    cleargame();
    readgame();
    r = findregion(0, 0);
    CuAssertStrEquals(tc, name, region_getname(r));
}

static void test_faction_name(CuTest * tc)
{
    const char * name = "Sacco & Vanzetti";
    faction * f;

    cleargame();
    f = create_faction(1);
    CuAssertPtrEquals(tc, 0, (void *)faction_getname(f));
    faction_setname(f, name);
    CuAssertStrEquals(tc, name, faction_getname(f));
    writegame();
    cleargame();
    readgame();
    f = findfaction(1);
    CuAssertStrEquals(tc, name, faction_getname(f));
}

static void test_region_addunit(CuTest * tc)
{
    unit * u;
    region * r;

    cleargame();
    r = create_region(0, 0, T_PLAIN);
    u = create_unit(0, 1);
    region_addunit(r, u);
    CuAssertPtrEquals(tc, u, r->units);
}

static void test_unit_name(CuTest * tc)
{
    const char * name = "Sacco & Vanzetti";
    unit * u;
    faction * f;
    region * r;

    cleargame();
    r = create_region(0, 0, T_PLAIN);
    f = create_faction(1);
    u = create_unit(f, 1);
    region_addunit(r, u);
    unit_setname(u, name);
    CuAssertStrEquals(tc, name, unit_getname(u));
    writegame();
    cleargame();
    readgame();
    u = findunitg(1);
    CuAssertPtrNotNull(tc, u);
    CuAssertStrEquals(tc, name, unit_getname(u));
}

static void test_unit_display(CuTest * tc)
{
    const char * display = "Sacco & Vanzetti";
    unit * u;
    faction * f;
    region * r;

    cleargame();
    r = create_region(0, 0, T_PLAIN);
    f = create_faction(1);
    u = create_unit(f, 1);
    region_addunit(r, u);
    unit_setdisplay(u, display);
    CuAssertStrEquals(tc, display, unit_getdisplay(u));
    writegame();
    cleargame();
    readgame();
    u = findunitg(1);
    CuAssertStrEquals(tc, display, unit_getdisplay(u));
}

static void test_faction_addr(CuTest * tc)
{
    const char * addr = "enno@example.com";
    faction * f;

    cleargame();
    f = create_faction(1);
    CuAssertPtrEquals(tc, 0, (void *)faction_getaddr(f));
    faction_setaddr(f, addr);
    CuAssertStrEquals(tc, addr, faction_getaddr(f));
    writegame();
    cleargame();
    readgame();
    f = findfaction(1);
    CuAssertStrEquals(tc, addr, faction_getaddr(f));
}

int main(void)
{
    CuString *output = CuStringNew();
    CuSuite *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_readwrite);
    SUITE_ADD_TEST(suite, test_createregion);
    SUITE_ADD_TEST(suite, test_makeblock);
    SUITE_ADD_TEST(suite, test_transform);
    SUITE_ADD_TEST(suite, test_movewhere);
    SUITE_ADD_TEST(suite, test_fileops);
    SUITE_ADD_TEST(suite, test_addplayer);
    SUITE_ADD_TEST(suite, test_origin);
    SUITE_ADD_TEST(suite, test_region_name);
    SUITE_ADD_TEST(suite, test_region_addunit);
    SUITE_ADD_TEST(suite, test_unit_name);
    SUITE_ADD_TEST(suite, test_unit_display);
    SUITE_ADD_TEST(suite, test_faction_name);
    SUITE_ADD_TEST(suite, test_faction_addr);
    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
    return suite->failCount;
}

