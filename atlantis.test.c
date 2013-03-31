#include "atlantis.h"
#include "keywords.h"
#include <CuTest.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

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

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
    return suite->failCount;
}

