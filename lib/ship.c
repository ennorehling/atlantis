#include "ship.h"
#include "rtl.h"

#include <quicklist.h>

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

quicklist *shiptypes = 0;

const char *shipnames[] = {
    "longboat",
    "clipper",
    "galleon",
};

ship *create_ship(int no, const struct ship_type *type) {
    ship *s = (ship *)calloc(1, sizeof(ship));
    if (s) {
        assert(no);
        s->type = type;
        s->left = 0;
        s->no = no;
    }
    return s;
}

void free_ship(ship *s) {
    free(s->name_);
    free(s->display_);
    free(s);
}

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

int ship_speed(const struct ship *sh)
{
    return sh->type->speed;
}

struct ship_type *create_shiptype(const char *name)
{
    ship_type * stype = (ship_type *)calloc(1, sizeof(ship_type));
    stype->name = _strdup(name);
    ql_push(&shiptypes, stype);
    return stype;
}

void free_shiptype(struct ship_type *stype) {
    free(stype->name);
    free(stype);
}

struct ship_type *get_shiptype(ship_t type) {
    assert(type>=0 && type<NUMSHIPS);
    return get_shiptype_by_name(shipnames[type]);
}

struct ship_type *get_shiptype_by_name(const char *name) {
    ql_iter qli = qli_init(&shiptypes);
    while (qli_more(qli)) {
        ship_type *stype = (ship_type *)qli_next(&qli);
        if (strcmp(name, stype->name)==0) {
            return stype;
        }
    }
    return 0;
}
