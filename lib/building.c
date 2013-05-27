#include "building.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

const char *buildingtypenames[] = {
    "longboat",
    "clipper",
    "galleon",
};

building *create_building(int no) {
    building *b = (building *)calloc(1, sizeof(building));
    assert(no);
    b->no = no;
    return b;
}

void free_building(building *b) {
    free(b->name_);
    free(b->display_);
    free(b);
}

const char * building_getname(const struct building *self)
{
    return self->name_;
}

void building_setname(struct building *self, const char *name)
{
    assert(name);
    self->name_ = (char *)realloc(self->name_, strlen(name)+1);
    strcpy(self->name_, name);
}

const char * building_getdisplay(const struct building *self)
{
    return self->display_;
}

void building_setdisplay(struct building *self, const char *display)
{
    if (display) {
        self->display_ = (char *)realloc(self->display_, strlen(display)+1);
        strcpy(self->display_, display);
    } else {
        free(self->display_);
        self->display_ = 0;
    }
}
