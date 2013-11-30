#include "service.h"

#include <CuTest.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>

static void test_size(CuTest * tc) {
    HREGION r;
    int w, h;

    iregion.set_size(8, 16);
    iregion.get_size(&w, &h);
    CuAssertIntEquals(tc, 8, w);
    CuAssertIntEquals(tc, 16, h);
}

static void test_create_region(CuTest * tc) {
    HREGION r;

    r = iregion.create(1, 2, "ocean");
    CuAssertPtrNotNull(tc, r.ptr);
    CuAssertPtrEquals(tc, r.ptr, iregion.get(1, 2).ptr);
    CuAssertStrEquals(tc, "ocean", iregion.get_terrain(r));
    CuAssertPtrEquals(tc, 0, (void *)iregion.get_name(r));
    CuAssertIntEquals(tc, 1, iregion.x(r));
    CuAssertIntEquals(tc, 2, iregion.y(r));
    iregion.destroy(r);
}

static void test_recreate_region(CuTest * tc) {
    HREGION r1, r2;

    r1 = iregion.create(1, 2, "ocean");
    iregion.destroy(r1);
    r2 = iregion.create(1, 2, "plain");
    CuAssertStrEquals(tc, "plain", iregion.get_terrain(r2));
    CuAssertIntEquals(tc, 1, iregion.x(r2));
    CuAssertIntEquals(tc, 2, iregion.y(r2));
    r1 = iregion.get(1, 2);
    CuAssertPtrEquals(tc, r2.ptr, r1.ptr);
}

int main(void)
{
    CuString *output = CuStringNew();
    CuSuite *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_size);
    SUITE_ADD_TEST(suite, test_create_region);
    SUITE_ADD_TEST(suite, test_recreate_region);

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
    return suite->failCount;
}
