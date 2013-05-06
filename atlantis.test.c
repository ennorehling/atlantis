#include "atlantis.h"
#include "keywords.h"
#include "region.h"
#include "faction.h"
#include "unit.h"
#include "json.h"

#include <stream.h>
#include <memstream.h>
#include <filestream.h>

#include <CuTest.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>

static void test_json_report(CuTest * tc) {
    region * r;
    faction * f;
    unit * u;
    cJSON * json;

    cleargame();
    turn = 0;
    r = create_region(1, 1, T_PLAIN);
    f = addplayer(r, 0, 0);
    u = r->units;

    json = json_report(f);
    CuAssertIntEquals(tc, turn, cJSON_GetObjectItem(json, "turn")->valueint);
    free(json);
}

static void test_json_write(CuTest * tc) {
    char buf[256];
    cJSON * json = cJSON_CreateObject();
    stream strm;
    mstream_init(&strm);
    cJSON_AddNumberToObject(json, "turn", 1);
    json_write(json, &strm);
    strm.api->rewind(strm.handle);
    strm.api->readln(strm.handle, buf, sizeof(buf));
    CuAssertStrEquals(tc, "{", buf);
    strm.api->readln(strm.handle, buf, sizeof(buf));
    CuAssertStrEquals(tc, "\t\"turn\":\t1", buf);
    strm.api->readln(strm.handle, buf, sizeof(buf));
    CuAssertStrEquals(tc, "}", buf);
    cJSON_Delete(json);
    mstream_done(&strm);
}

static void test_wrapmap(CuTest * tc)
{
    region *r;

    cleargame();
    makeblock(0, 0);
    makeworld();

    CuAssertIntEquals(tc, 0, world.left);
    CuAssertIntEquals(tc, 0, world.top);
    CuAssertIntEquals(tc, BLOCKSIZE+2*BLOCKBORDER, world.width);
    CuAssertIntEquals(tc, BLOCKSIZE+2*BLOCKBORDER, world.height);
    r = findregion(0, 0);
    CuAssertIntEquals(tc, world.height-world.top-1, r->connect[0]->y); /* NORTH */
    CuAssertIntEquals(tc, world.height-world.top-1, r->connect[3]->x); /* WEST */
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
    turn = 0;
    r = create_region(1, 1, T_PLAIN);
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
    processorders();
    CuAssertIntEquals(tc, 1, (int)faction_checkpassword(f, "newpassword"));
    CuAssertIntEquals(tc, 0, (int)faction_checkpassword(f, "mypassword"));

    mstream_done(&strm);
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
    CuAssertStrEquals(tc, "STUDY magic", u->orders->s);
    mstream_done(&strm);
}

static void test_addplayers(CuTest * tc)
{
    region * r;
    unit * u;
    stream strm;
    faction * f;
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
    for (u=r->units,f=factions,n=0;f && u;f=f->next,u=u->next,++n) {
        CuAssertPtrEquals(tc, u->faction, f);
        CuAssertIntEquals(tc, 1, u->number);
    }
    CuAssertIntEquals(tc, 2, n);
    CuAssertPtrEquals(tc, 0, u);
    CuAssertPtrEquals(tc, 0, f);
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
    u = r->units;

    CuAssertPtrNotNull(tc, r);
    CuAssertPtrNotNull(tc, f);
    CuAssertPtrNotNull(tc, u);
    CuAssertStrEquals(tc, email, faction_getaddr(f));
    CuAssertIntEquals(tc, r->x, f->origin_x);
    CuAssertIntEquals(tc, r->y, f->origin_y);
    CuAssertPtrNotNull(tc, r->units);
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

int main(void)
{
    CuString *output = CuStringNew();
    CuSuite *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_wrapmap);
    SUITE_ADD_TEST(suite, test_addplayer);
    SUITE_ADD_TEST(suite, test_orders);
    SUITE_ADD_TEST(suite, test_good_password);
    SUITE_ADD_TEST(suite, test_bad_password);
    SUITE_ADD_TEST(suite, test_password_cmd);
    SUITE_ADD_TEST(suite, test_addplayers);
    SUITE_ADD_TEST(suite, test_fileops);
    SUITE_ADD_TEST(suite, test_faction_password);
    SUITE_ADD_TEST(suite, test_readwrite);
    SUITE_ADD_TEST(suite, test_createregion);
    SUITE_ADD_TEST(suite, test_makeblock);
    SUITE_ADD_TEST(suite, test_transform);
    SUITE_ADD_TEST(suite, test_movewhere);
    SUITE_ADD_TEST(suite, test_origin);
    SUITE_ADD_TEST(suite, test_region_name);
    SUITE_ADD_TEST(suite, test_region_addunit);
    SUITE_ADD_TEST(suite, test_unit_name);
    SUITE_ADD_TEST(suite, test_unit_display);
    SUITE_ADD_TEST(suite, test_faction_name);
    SUITE_ADD_TEST(suite, test_faction_addr);
    SUITE_ADD_TEST(suite, test_json_report);
    SUITE_ADD_TEST(suite, test_json_write);

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
    return suite->failCount;
}

