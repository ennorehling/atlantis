#include "unit.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

const char * unit_getname(const struct unit *self)
{
    return self->name_;
}

void unit_setname(struct unit *self, const char *name)
{
    assert(name);
    self->name_ = (char *)realloc(self->name_, strlen(name)+1);
    strcpy(self->name_, name);
}

unit * create_unit(struct faction *f, int no)
{
    char name[NAMESIZE];
    unit * self = calloc(1, sizeof(unit));

    self->faction = f;
    self->no = no;
    sprintf(name, "Unit %d", no);
    unit_setname(self, name);
    return self;
}
