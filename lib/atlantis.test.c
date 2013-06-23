#include "atlantis.h"
#include "keywords.h"
#include "region.h"
#include "faction.h"
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

static unit *make_unit(faction *f, region *r, int no) {
    unit * u = create_unit(f, no);
    u->number = 1;
    region_addunit(r, u);
    return u;
}

static void test_unstack_leader(CuTest *tc)
{
    faction *f;
    region *r;
    unit *u1, *u2, *u3;
    
    cleargame();
    r = create_region(1, 1, T_PLAIN);
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
    r = create_region(1, 1, T_PLAIN);
    f = create_faction(1);
    u1 = make_unit(f, r, 1);
    u2 = make_unit(f, r, 2);
    u3 = make_unit(f, r, 3);
    
    CuAssertPtrEquals(tc, u1, unit_getstack(u1));
    CuAssertPtrEquals(tc, u2, unit_getstack(u2));

    unit_stack(u3, u2);
    CuAssertPtrEquals(tc, u2, unit_getstack(u3));
    CuAssertPtrEquals(tc, u2, unit_getstack(u2));

    unit_stack(u2, u1);
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
    r = create_region(1, 1, T_PLAIN);
    f = create_faction(1);
    u1 = make_unit(f, r, 1);
    u2 = make_unit(f, r, 2);
    u3 = make_unit(f, r, 3);
    
    // 1, 2, 3:
    CuAssertPtrEquals(tc, u1, ql_get(r->units, 0));
    CuAssertPtrEquals(tc, u2, ql_get(r->units, 1));
    CuAssertPtrEquals(tc, u3, ql_get(r->units, 2));

    unit_stack(u3, u1);
    // 1->3, 2:
    CuAssertPtrEquals(tc, u3, ql_get(r->units, 1));
    CuAssertPtrEquals(tc, u2, ql_get(r->units, 2));

    unit_unstack(u3);
    // 1, 3, 2:
    CuAssertPtrEquals(tc, u3, ql_get(r->units, 1));
    CuAssertPtrEquals(tc, u2, ql_get(r->units, 2));

    unit_stack(u3, u2);
    // 1, 2->3:
    CuAssertPtrEquals(tc, u2, ql_get(r->units, 1));
    CuAssertPtrEquals(tc, u3, ql_get(r->units, 2));

    unit_stack(u2, u1);
    // 1->2->3:
    CuAssertPtrEquals(tc, u1, ql_get(r->units, 0));
    CuAssertPtrEquals(tc, u2, ql_get(r->units, 1));
    CuAssertPtrEquals(tc, u3, ql_get(r->units, 2));

}

static void test_wrapmap(CuTest * tc)
{
    region *r;

    cleargame();
    makeblock(0, 0);
    makeworld();

    CuAssertIntEquals(tc, BLOCKSIZE+2*BLOCKBORDER, config.width);
    CuAssertIntEquals(tc, BLOCKSIZE+2*BLOCKBORDER, config.height);
    r = findregion(0, 0);
    CuAssertIntEquals(tc, config.height-1, r->connect[0]->y); /* NORTH */
    CuAssertIntEquals(tc, config.height-1, r->connect[3]->x); /* WEST */
}

static void test_good_password(CuTest * tc)
{
    region * r;
    unit * u;
    faction * f;
    char line[256];
    stream strm;

    cleargame();
    turn = 0;
    r = create_region(1, 1, T_PLAIN);
    f = addplayer(r, 0, 0);
    u = (unit *)ql_get(r->units, 0);

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
    turn = 0;
    r = create_region(1, 1, T_PLAIN);
    f = addplayer(r, 0, 0);
    u = (unit *)ql_get(r->units, 0);

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
    turn = 0;
    r = create_region(1, 1, T_PLAIN);
    f = addplayer(r, 0, 0);
    faction_setpassword(f, "mypassword");
    u = (unit *)ql_get(r->units, 0);

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
    turn = 0;
    r = create_region(1, 1, T_PLAIN);
    f = addplayer(r, 0, 0);
    faction_setpassword(f, "mypassword");
    u = (unit *)ql_get(r->units, 0);

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
    f = create_faction(1);
    r = create_region(0, 0, T_PLAIN);
    u = make_unit(f, r, 1);
    ql_push(&u->orders, _strdup("ENTERTAIN"));
    ql_push(&u->orders, _strdup("FORM 42"));
    ql_push(&u->orders, _strdup("WORK"));
    ql_push(&u->orders, _strdup("END"));
    process_form(u, r);
    CuAssertIntEquals(tc, 2, ql_length(r->units));
    CuAssertIntEquals(tc, 1, ql_length(u->orders));
    CuAssertIntEquals(tc, 0, u->alias);
    CuAssertPtrEquals(tc, u, (unit *)ql_get(r->units, 0));

    u = (unit *)ql_get(r->units, 1);
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
    turn = 0;
    r = create_region(1, 1, T_PLAIN);
    f = addplayer(r, 0, 0);
    faction_setpassword(f, "mypassword");
    u = (unit *)ql_get(r->units, 0);
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
    stream strm;
    ql_iter fli, uli;
    int n;

    cleargame();
    r = create_region(0, 0, T_FOREST);
    mstream_init(&strm);

    strm.api->writeln(strm.handle, "enno@eressea.de");
    strm.api->writeln(strm.handle, "atlantis@eressea.de");
    strm.api->rewind(strm.handle);

    addplayers(r, &strm);
    CuAssertPtrNotNull(tc, factions);
    CuAssertPtrNotNull(tc, r->units);
    CuAssertIntEquals(tc, ql_length(r->units), ql_length(factions));
    for (uli=qli_init(&r->units),fli=qli_init(&factions),n=0;qli_more(fli) && qli_more(uli);++n) {
        unit * u = (unit *)qli_next(&uli);
        faction *f = (faction *)qli_next(&fli);
        CuAssertPtrEquals(tc, u->faction, f);
        CuAssertIntEquals(tc, 1, u->number);
    }
    CuAssertIntEquals(tc, 2, n);
    CuAssertTrue(tc, !qli_more(fli));
}

static void test_addplayer(CuTest * tc)
{
    region * r;
    faction * f;
    unit * u;
    const char * email = "enno@example.com";

    cleargame();
    turn = 1;
    r = create_region(1, 1, T_PLAIN);
    f = addplayer(r, email, 0);
    u = (unit *)ql_get(r->units, 0);

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
    r1 = create_region(1, 1, T_PLAIN);
    r2 = create_region(1, 2, T_OCEAN);
    r3 = create_region(2, 2, T_PLAIN);
    region_setname(r1, "foo");
    region_setname(r3, "bar");
    makeworld();
    f = addplayer(r1, "enno@example.com", 0);
    CuAssertStrEquals(tc, "foo (0,0)", regionid(r1, f));
    CuAssertStrEquals(tc, "(0,1)", regionid(r2, f));
    CuAssertStrEquals(tc, "bar (1,1)", regionid(r3, f));
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
    int x, y;

    cleargame();
    makeblock(0, 0);
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

    errno = 0;
    cleargame();
    f = create_faction(fno);
    r = create_region(0, 0, T_PLAIN);
    u = make_unit(f, r, uno);
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
    region * r = 0;
    ql_iter rli;
    int x = 0, y = 0, fno;

    turn = -1;
    cleargame();
    initgame();

    for (rli = qli_init(&regions); qli_more(rli);) {
        r = (region *)qli_next(&rli);
        if (r->terrain!=T_OCEAN) {
            x = r->x, y = r->y;
            f = addplayer(r, "enno@example.com", 0);
            break;
        }
    }
    CuAssertPtrNotNull(tc, r);
    CuAssertPtrNotNull(tc, factions);
    CuAssertPtrNotNull(tc, f);
    fno = f->no;
    writegame();
    cleargame();
    CuAssertPtrEquals(tc, 0, findregion(0, 0));

    CuAssertIntEquals(tc, 0, readgame());
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

static void test_owners(CuTest * tc) {
    region *r;
    unit *u;
    ship *s;
    faction *f;

    cleargame();
    f = create_faction(1);
    r = create_region(0, 0, T_PLAIN);
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

    cleargame();
    makeblock(0, 0);
    makeworld();
    c = findregion(1, 1);

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
	region * r;
	unit * u;

    cleargame();
    f = create_faction(1);
    r = create_region(0, 0, T_PLAIN);
    u = make_unit(f, r, 1);
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
    CuAssertPtrEquals(tc, r, u->region);
    CuAssertPtrEquals(tc, u, (unit *)ql_get(r->units, 0));
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
    u = make_unit(f, r, 1);
    unit_setname(u, name);
    CuAssertStrEquals(tc, name, unit_getname(u));
    writegame();
    cleargame();
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
    r = create_region(0, 0, T_PLAIN);
    f = create_faction(1);
    u = make_unit(f, r, 1);
    u->number = 0;
    writegame();
    cleargame();
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
    r = create_region(0, 0, T_PLAIN);
    f = create_faction(1);
    u = make_unit(f, r, 1);
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
	unit *u;
	region *r;

    cleargame();
    f = create_faction(1);
    r = create_region(0, 0, T_PLAIN);
    u = make_unit(f, r, 1);
    CuAssertPtrEquals(tc, 0, (void *)faction_getaddr(f));
    faction_setaddr(f, addr);
    CuAssertStrEquals(tc, addr, faction_getaddr(f));
    writegame();
    cleargame();
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

static void test_settings(CuTest * tc)
{
    stream strm;
    mstream_init(&strm);
    strm.api->writeln(strm.handle, "# comments are okay");
    strm.api->writeln(strm.handle, "width = 10");
    strm.api->writeln(strm.handle, " height = 20");
    strm.api->writeln(strm.handle, " stacks=yes");
    strm.api->rewind(strm.handle);
    read_config(&strm);
    CuAssertIntEquals(tc, 10, config.width);
    CuAssertIntEquals(tc, 20, config.height);
    CuAssertIntEquals(tc, CFG_STACKS, config.features);

    mstream_done(&strm);
}

int main(void)
{
    CuString *output = CuStringNew();
    CuSuite *suite = CuSuiteNew();

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
    SUITE_ADD_TEST(suite, test_makeblock);
    SUITE_ADD_TEST(suite, test_transform);
    SUITE_ADD_TEST(suite, test_owners);
    SUITE_ADD_TEST(suite, test_movewhere);
    SUITE_ADD_TEST(suite, test_origin);
    SUITE_ADD_TEST(suite, test_region_name);
    SUITE_ADD_TEST(suite, test_region_addunit);
    SUITE_ADD_TEST(suite, test_unit_name);
    SUITE_ADD_TEST(suite, test_remove_empty);
    SUITE_ADD_TEST(suite, test_unit_display);
    SUITE_ADD_TEST(suite, test_faction_name);
    SUITE_ADD_TEST(suite, test_faction_addr);
    SUITE_ADD_TEST(suite, test_stacking);
    SUITE_ADD_TEST(suite, test_unstack_leader);
    // SUITE_ADD_TEST(suite, test_stacking_moves_units);
    SUITE_ADD_TEST(suite, test_settings);

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
    return suite->failCount;
}

