#ifndef RTL_H
#define RTL_H

#if defined(__GNUC__)
#include <strings.h>
#define strcmpl(a, b) strcasecmp(a, b)
#elif defined(_MSC_VER)
#define strcmpl(a, b) _stricmp(a, b)
#endif

#endif
