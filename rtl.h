#ifndef RTL_H
#define RTL_H

#include "config.h"
#if defined(HAVE_STRINGS_H)
#include <strings.h>
#define strcmpl(a, b) strcasecmp(a, b)
#elif defined(HAVE__STRICMP)
#define strcmpl(a, b) _stricmp(a, b)
#elif defined(HAVE_STRICMP)
#define strcmpl(a, b) stricmp(a, b)
#else
char * rtl_strcmpl(char * str);
#define strcmpl(a, b) rtl_strcmpl(a, b)
#endif

#if !defined(HAVE_STRLWR)
char * rtl_strupr(char * str);
#define strupr(a) rtl_strupr(a)
#endif

#endif
