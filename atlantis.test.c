#include "atlantis.h"
#include "keywords.h"
#include "region.h"

#include <CuTest.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

static void test_makeblock(CuTest * tc)
{
    region * r;
    int d, x, y;

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

static void test_fileops(CuTest * tc)
{
    turn = -1;
    initgame();
    CuAssertPtrNotNull(tc, findregion(0, 0));
    cleargame();
    CuAssertPtrEquals(tc, 0, findregion(0, 0));
    readgame();
    CuAssertPtrNotNull(tc, findregion(0, 0));
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

int main(void)
{
    CuString *output = CuStringNew();
    CuSuite *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_transform);
    SUITE_ADD_TEST(suite, test_movewhere);
    SUITE_ADD_TEST(suite, test_makeblock);
    SUITE_ADD_TEST(suite, test_fileops);

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
    return suite->failCount;
}

