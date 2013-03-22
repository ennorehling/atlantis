#include <CuTest.h>
#include <stdio.h>
#include "rtl.h"

int main(int argc, char ** argv) {
  CuString *output = CuStringNew();
  CuSuite *suite = CuSuiteNew();
  
  CuSuiteRun(suite);
  CuSuiteSummary(suite, output);
  CuSuiteDetails(suite, output);
  printf("%s\n", output->buffer);
  return suite->failCount;
}
