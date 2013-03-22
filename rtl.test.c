#include <CuTest.h>
#include <stdio.h>
#include "rtl.h"

static void test_strcmpl(CuTest * tc) {
  CuAssertIntEquals(tc, 0, strcmpl("foo", "foo"));
  CuAssertIntEquals(tc, 0, strcmpl("$%^&#", "$%^&#"));
  CuAssertIntEquals(tc, 0, strcmpl("foo", "FOO"));
  CuAssertIntEquals(tc, 0, strcmpl("foo", "Foo"));
  CuAssertTrue(tc, strcmpl("foo", "bar")>0);
  CuAssertTrue(tc, strcmpl("bar", "foo")<0);
  CuAssertTrue(tc, strcmpl("barfly", "bar")>0);
}

extern int rtl_strcmpl(const char *a, const char *b);
static void test_rtl_strcmpl(CuTest * tc) {
  CuAssertIntEquals(tc, 0, rtl_strcmpl("foo", "foo"));
  CuAssertIntEquals(tc, 0, rtl_strcmpl("$%^&#", "$%^&#"));
  CuAssertIntEquals(tc, 0, rtl_strcmpl("foo", "FOO"));
  CuAssertIntEquals(tc, 0, rtl_strcmpl("foo", "Foo"));
  CuAssertTrue(tc, rtl_strcmpl("foo", "bar")>0);
  CuAssertTrue(tc, rtl_strcmpl("bar", "foo")<0);
  CuAssertTrue(tc, rtl_strcmpl("barfly", "bar")>0);
}

int main(int argc, char ** argv) {
  CuString *output = CuStringNew();
  CuSuite *suite = CuSuiteNew();

  SUITE_ADD_TEST(suite, test_strcmpl);
  SUITE_ADD_TEST(suite, test_rtl_strcmpl);

  CuSuiteRun(suite);
  CuSuiteSummary(suite, output);
  CuSuiteDetails(suite, output);
  printf("%s\n", output->buffer);
  return suite->failCount;
}
