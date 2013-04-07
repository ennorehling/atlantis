#include "region.h"
#include <string.h>

const char * region_getname(const struct region *r)
{
    return r->name;
}
void region_setname(struct region *r, const char *name)
{
    strncpy(r->name, name, NAMESIZE);
}

