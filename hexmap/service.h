#ifndef SERVICE_H
#define SERVICE_H

typedef struct { void * ptr; } HREGION;
typedef struct { void * ptr; } HUNIT;
typedef struct { void * ptr; } HFACTION;

typedef struct region_svc {
    int (*x)(HREGION);
    int (*y)(HREGION);
    const char *(*terrain)(HREGION);
    const char *(*name)(HREGION);
    HREGION (*create)(int x, int y, const char *terrain);
    HREGION (*get)(int x, int y);
} region_svc;

extern region_svc region;

#define IS_NULL(h) (h.ptr==0)
#endif
