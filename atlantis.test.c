#include "atlantis.h"
#include "keywords.h"
#include "region.h"

#include <CuTest.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

static void test_movewhere(CuTest * tc)
{
    region *r, *c;
    makeblock(0, 0);
    c = findregion(0, 0);

    sprintf(buf, "%s %s", keywords[K_MOVE], keywords[K_NORTH]);
    CuAssertIntEquals(tc, K_MOVE, igetkeyword(buf));
    r = movewhere(c);
    CuAssertPtrEquals(tc, r, c->connect[0]);
    CuAssertPtrEquals(tc, c, r->connect[1]);

    sprintf(buf, "%s %s", keywords[K_MOVE], keywords[K_SOUTH]);
    CuAssertIntEquals(tc, K_MOVE, igetkeyword(buf));
    r = movewhere(c);
    CuAssertPtrEquals(tc, r, c->connect[1]);
    CuAssertPtrEquals(tc, c, r->connect[0]);

    sprintf(buf, "%s %s", keywords[K_MOVE], keywords[K_EAST]);
    CuAssertIntEquals(tc, K_MOVE, igetkeyword(buf));
    r = movewhere(c);
    CuAssertPtrEquals(tc, r, c->connect[2]);
    CuAssertPtrEquals(tc, c, r->connect[3]);

    sprintf(buf, "%s %s", keywords[K_MOVE], keywords[K_WEST]);
    CuAssertIntEquals(tc, K_MOVE, igetkeyword(buf));
    r = movewhere(c);
    CuAssertPtrEquals(tc, r, c->connect[3]);
    CuAssertPtrEquals(tc, c, r->connect[2]);
}

static void test_transform(CuTest * tc)
{
    int x, y;
  
    x = 0, y = 0;
    CuAssertIntEquals(tc, EINVAL, transform(&x, &y, K_ENTER));
    CuAssertIntEquals(tc, 0, transform(&x, &y, K_NORTH));
    CuAssertIntEquals(tc, x, 0);
    CuAssertIntEquals(tc, y, -1);

    x = 0, y = 0;
    CuAssertIntEquals(tc, 0, transform(&x, &y, K_SOUTH));
    CuAssertIntEquals(tc, x, 0);
    CuAssertIntEquals(tc, y, 1);

    x = 0, y = 0;
    CuAssertIntEquals(tc, 0, transform(&x, &y, K_WEST));
    CuAssertIntEquals(tc, x, -1);
    CuAssertIntEquals(tc, y, 0);

    x = 0, y = 0;
    CuAssertIntEquals(tc, 0, transform(&x, &y, K_EAST));
    CuAssertIntEquals(tc, x, 1);
    CuAssertIntEquals(tc, y, 0);
}

int main(void)
{
    CuString *output = CuStringNew();
    CuSuite *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_transform);
    SUITE_ADD_TEST(suite, test_movewhere);

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
    return suite->failCount;
}

