#include <CuTest.h>
#include <stdio.h>
#include <string.h>
#include "rtl.h"

static void test_strcmpl(CuTest * tc)
{
    CuAssertIntEquals(tc, 0, _strcmpl("foo", "foo"));
    CuAssertIntEquals(tc, 0, _strcmpl("$%^&#", "$%^&#"));
    CuAssertIntEquals(tc, 0, _strcmpl("foo", "FOO"));
    CuAssertIntEquals(tc, 0, _strcmpl("foo", "Foo"));
    CuAssertTrue(tc, _strcmpl("foo", "bar") > 0);
    CuAssertTrue(tc, _strcmpl("bar", "foo") < 0);
    CuAssertTrue(tc, _strcmpl("barfly", "bar") > 0);
}

static void test_memicmp(CuTest * tc)
{
    CuAssertIntEquals(tc, 0, _memicmp("Foo", "foo", 3));
    CuAssertIntEquals(tc, 0, _memicmp("FOo", "foobar", 3));
}

static void test_rtl_memicmp(CuTest * tc)
{
    CuAssertIntEquals(tc, 0, rtl_memicmp("foo", "foo", 3));
    CuAssertIntEquals(tc, 0, rtl_memicmp("foo", "FOO", 3));
    CuAssertTrue(tc, rtl_memicmp("aab", "aaa", 3) > 0);
    CuAssertTrue(tc, rtl_memicmp("aaa", "aab", 3) < 0);
    CuAssertTrue(tc, rtl_memicmp("foo", "foobar", 10) < 0);
}

static void test_rtl_strcmpl(CuTest * tc)
{
    CuAssertIntEquals(tc, 0, rtl_strcmpl("foo", "foo"));
    CuAssertIntEquals(tc, 0, rtl_strcmpl("foo", "FoO"));
    CuAssertIntEquals(tc, 0, rtl_strcmpl("$%^&#", "$%^&#"));
    CuAssertIntEquals(tc, 0, rtl_strcmpl("foo", "FOO"));
    CuAssertIntEquals(tc, 0, rtl_strcmpl("foo", "Foo"));
    CuAssertTrue(tc, rtl_strcmpl("aaa", "aab") < 0);
    CuAssertTrue(tc, rtl_strcmpl("aab", "aaa") > 0);
    CuAssertTrue(tc, rtl_strcmpl("foo", "foobar") < 0);
    CuAssertTrue(tc, rtl_strcmpl("foo", "bar") > 0);
    CuAssertTrue(tc, rtl_strcmpl("foobar", "bar") > 0);
    CuAssertTrue(tc, rtl_strcmpl("bar", "foo") < 0);
    CuAssertTrue(tc, rtl_strcmpl("barfly", "bar") > 0);
}

static void test_strlwr(CuTest * tc)
{
    char str[64];
    char *in = strcpy(str, "FOO");
    CuAssertPtrEquals(tc, in, _strlwr(in));
    CuAssertStrEquals(tc, "hello", _strlwr(strcpy(str, "hello")));
    CuAssertStrEquals(tc, "hello", _strlwr(strcpy(str, "HELlo")));
    CuAssertStrEquals(tc, "$%^&*", _strlwr(strcpy(str, "$%^&*")));
}

static void test_rtl_strlwr(CuTest * tc)
{
    char str[64];
    CuAssertStrEquals(tc, "hello", rtl_strlwr(strcpy(str, "hello")));
    CuAssertStrEquals(tc, "hello", rtl_strlwr(strcpy(str, "HELlo")));
    CuAssertStrEquals(tc, "$%^&*", rtl_strlwr(strcpy(str, "$%^&*")));
}

int main(void)
{
    CuString *output = CuStringNew();
    CuSuite *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_strcmpl);
    SUITE_ADD_TEST(suite, test_strlwr);
    SUITE_ADD_TEST(suite, test_memicmp);
    SUITE_ADD_TEST(suite, test_rtl_strcmpl);
    SUITE_ADD_TEST(suite, test_rtl_memicmp);
    SUITE_ADD_TEST(suite, test_rtl_strlwr);

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
    return suite->failCount;
}
