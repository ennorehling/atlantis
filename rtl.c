#include "rtl.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>

char * rtl_strlwr(char * str) {
  char * s;
  for (s=str;*s;++s) {
    *s = tolower(*s);
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