#ifndef SERVICE_H
#define SERVICE_H

typedef struct { void * ptr; } HREGION;
typedef struct { void * ptr; } HUNIT;
typedef struct { void * ptr; } HFACTION;

typedef struct region_svc {
    void (*set_size)(int w, int h);
    void (*get_size)(int *w, int *h);
    int (*x)(HREGION);
    int (*y)(HREGION);
    const char *(*get_terrain)(HREGION);
    const char *(*get_name)(HREGION);
    HREGION (*create)(int x, int y, const char *terrain);
    void (*destroy)(HREGION);
    HREGION (*get)(int x, int y);
} region_svc;

extern region_svc iregion;

#define IS_NULL(h) (h.ptr==0)
#endif
