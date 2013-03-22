#include "rtl.h"
#include <ctype.h>
#include <string.h>

char * rtl_strupr(char * str) {
  for (;*str;++str) {
    *str = tolower(*str);
  }
  return str;
}

int rtl_strcmpl(const char * a, const char * b) {
  for (;*a && *b;++a, ++b) {
    int ca = tolower(*a);
    int cb = tolower(*b);
    if (ca<cb) {
      return -1;
    } else if (ca>cb) {
      return 1;
    }
  }
  if (*b) return -1;
  if (*a) return 1;
  return 0;
}
