#include "service.h"

#include <CuTest.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>

static void test_create_region(CuTest * tc) {
    HREGION r;

    r = region.create(1, 2, "ocean");
    CuAssertPtrNotNull(tc, r.ptr);
    CuAssertPtrEquals(tc, r.ptr, region.get(1, 2).ptr);
    CuAssertStrEquals(tc, "ocean", region.terrain(r));
    CuAssertPtrEquals(tc, 0, (void *)region.name(r));
    CuAssertIntEquals(tc, 1, region.x(r));
    CuAssertIntEquals(tc, 2, region.y(r));
}

int main(void)
{
    CuString *output = CuStringNew();
    CuSuite *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_create_region);

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
    return suite->failCount;
}
