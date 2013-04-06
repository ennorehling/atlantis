/*
 * Atlantis v1.0 13 September 1993
 * Copyright 1993 by Russell Wallace
 *
 * This program may be freely used, modified and distributed. It may not be
 * sold or used commercially without prior written permission from the author.
 */

#include "atlantis.h"
#include "faction.h"

#include <string.h>

void faction_setname(faction * f, const char * name) {
    strncpy(f->name_, name, NAMESIZE);
    f->name_[NAMESIZE] = 0;
}

const char * faction_getname(const faction * f) {
    return f->name_;
}
