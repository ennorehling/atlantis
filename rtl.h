#ifndef RTL_H
#define RTL_H

#include <stddef.h>
int rtl_strcmpl(const char *a, const char *b);
int rtl_memicmp(const char *a, const char *b, size_t size);
char *rtl_strdup(const char *a);
char *rtl_strlwr(char *str);

#include "config.h"

#if !defined(HAVE__MEMICMP)
#if defined(HAVE_MEMICMP)
#define _memicmp(a, b, n) memicmp(a, b, n)
#elif defined(HAVE_STRNCASECMP)
#include <strings.h>
#define _memicmp(a, b, n) strncasecmp(a, b, n)
#else
#define _memicmp(a, b, n) rtl_memicmp(a, b, n)
#endif
#endif

#if !defined(HAVE__STRCMPL)
#if defined(HAVE_STRCMPL)
#define _strcmpl(a, b) strcmpl(a, b)
#elif defined(HAVE__STRICMP)
#define _strcmpl(a, b) _stricmp(a, b)
#elif defined(HAVE_STRICMP)
#define _strcmpl(a, b) stricmp(a, b)
#elif defined(HAVE_STRCASECMP)
#include <strings.h>
#define _strcmpl(a, b) strcasecmp(a, b)
#else
#define _strcmpl(a, b) rtl_strcmpl(a, b)
#endif
#endif

#if defined(HAVE_DIRECT__MKDIR)
#include <direct.h>
#elif defined(HAVE_DIRECT_MKDIR)
#include <direct.h>
#define _mkdir(a) mkdir(a)
#elif defined(HAVE_SYS_STAT_MKDIR)
#include <sys/stat.h>
#define _mkdir(a) mkdir(a, 0777)
#endif

#if !defined(HAVE__STRLWR)
#if defined(HAVE_STRLWR)
#define _strlwr(a) strlwr(a)
#else
#define _strlwr(a) rtl_strlwr(a)
#endif
#endif

#if !defined(HAVE__STRDUP)
#if defined(HAVE_STRDUP)
#define _strdup(a) strdup(a)
#else
#define _strdup(a) rtl_strdup(a)
#endif
#endif

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#if !defined(DBG_UNREFERENCED_PARAMETER)
#define DBG_UNREFERENCED_PARAMETER(x) (void)(x)
#endif

#endif
