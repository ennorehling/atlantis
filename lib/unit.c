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
    unit **up = &u->region->units_;
    unit **sp = &stack->next;

    stack = unit_getstack(stack);
    while (*up) {
        unit *x = *up;
        if (x==u) {
            break;
        }
        up = &x->next;
    }
    assert(u->region);
    assert(stack->region==u->region);

    if (up!=sp) {
        while (*up) {
            unit *x = *up;
            if (x==u || x->stack==u) {
                *up = x->next;
                x->next = *sp;
                *sp = x;
                sp = &x->next;
                x->stack = stack;
            } else {
                break;
            }
        }
    } else {
        unit *x;
        for (x=u->next;x;x=x->next) {
            if (x->stack==u) {
                x->stack = stack;
            } else {
                break;
            }
        }
        u->stack = stack;
    }
}

void unit_unstack(unit* u) {
    if (u->stack) {
        unit ** up = &u->stack->next;
        while (*up!=u) {
            up = &(*up)->next;
        }
        *up = u->next;
        while (*up) {
            unit *x = *up;
            if (x->stack!=u->stack) {
                break;
            }
            up = &x->next;
        }
        u->stack = 0;
        u->next = *up;
        *up = u;
    } else if (u->next) {
        unit *stack = u->next;
        stack->stack = 0;
        for (u = stack->next;u;u=u->next) {
            u->stack = stack;
        }
    }
}

struct unit * unit_getstack(struct unit *u) {
    return u->stack ? u->stack : u;
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

void unit_setbuilding(struct unit *u, struct building *b)
{
    u->building = b;
}