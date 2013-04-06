/*
 * Atlantis v1.0 13 September 1993
 * Copyright 1993 by Russell Wallace
 *
 * This program may be freely used, modified and distributed. It may not be
 * sold or used commercially without prior written permission from the author.
 */

#include "atlantis.h"
#include "faction.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

void faction_setname(faction * f, const char * name) {
    assert(name);
    f->name_ = (char *)realloc(f->name_, strlen(name)+1);
    strcpy(f->name_, name);
}

const char * faction_getname(const faction * f) {
    return f->name_;
}


void faction_setaddr(faction * f, const char * addr) {
    if (addr) {
        f->addr_ = (char *)realloc(f->addr_, strlen(addr)+1);
        strcpy(f->addr_, addr);
    } else {
        free(f->addr_);
        f->addr_ = 0;
    }
}

const char * faction_getaddr(const faction * f) {
    return f->addr_;
}
