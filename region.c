#include "region.h"
#include "unit.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

const char * region_getname(const struct region *r)
{
    return r->name_;
}

void region_setname(struct region *r, const char *name)
{
    assert(name);
    r->name_ = (char *)realloc(r->name_, strlen(name)+1);
    strcpy(r->name_, name);
}

void region_addunit(struct region *self, struct unit *u)
{
    unit ** iter;
    assert(u && !u->next);
    for (iter=&self->units; *iter; iter=&(*iter)->next);
    *iter = u;
}
