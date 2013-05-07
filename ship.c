#include "ship.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

const char * ship_getname(const struct ship *self)
{
    return self->name_;
}

void ship_setname(struct ship *self, const char *name)
{
    assert(name);
    self->name_ = (char *)realloc(self->name_, strlen(name)+1);
    strcpy(self->name_, name);
}

const char * ship_getdisplay(const struct ship *self)
{
    return self->display_;
}

void ship_setdisplay(struct ship *self, const char *display)
{
    if (display) {
        self->display_ = (char *)realloc(self->display_, strlen(display)+1);
        strcpy(self->display_, display);
    } else {
        free(self->display_);
        self->display_ = 0;
    }
}
