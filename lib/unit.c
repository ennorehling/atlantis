#include "unit.h"
#include "region.h"
#include "atlantis.h"

#include <quicklist.h>

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

unit * create_unit(struct faction *f, int no)
{
    char name[NAMESIZE];
    unit * self = (unit *)calloc(1, sizeof(unit));

    if (self) {
        self->faction = f;
        self->no = no;
        self->combatspell = -1;
        sprintf(name, "Unit %d", no);
        unit_setname(self, name);
    }
    return self;
}

void free_unit(unit *u) {
    unit_unstack(u);
    ql_foreach(u->orders, free);
    ql_free(u->orders);
    free(u->litems);
    free(u->name_);
    free(u->display_);
    free(u);
}

void unit_stack(unit* u, unit *stack) {
    unit **up;
    unit **sp = &stack->next;

    for (up=&u->region->units_;*up;up=&(*up)->next) {
        unit *x = *up;
        if (x==u) {
            break;
        }
    }
    assert(u->region);
    assert(stack->region==u->region);

    while (*up) {
        unit *x = *up;
        if (u->stack && x->stack==u->stack) {
            *up = x->next;
            x->next = *sp;
            *sp = x;
            sp = &x->next;
            x->stack = stack;
        } else {
            break;
        }
    }
    u->stack = stack;
}

void unit_unstack(unit* u) {
    if (u->stack) {
        unit ** up = &u->stack->next;
        while (*up!=u) {
            up = &(*up)->next;
        }
        if (*up) {
            *up = u->next;
            u->next = 0;
            u->stack = 0;
        }
    } else if (u->next) {
        unit *stack = u->next;
        stack->stack = 0;
        for (u = stack->next;u;u=u->next) {
            u->stack = stack;
        }
    }
}

struct unit * unit_getstack(struct unit *u) {
    while (u->stack) { u = u->stack; }
    return u;
}

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

const char * unit_getdisplay(const struct unit *self)
{
    return self->display_;
}

void unit_setdisplay(struct unit *self, const char *display)
{
    if (display) {
        self->display_ = (char *)realloc(self->display_, strlen(display)+1);
        strcpy(self->display_, display);
    } else {
        free(self->display_);
        self->display_ = 0;
    }
}
