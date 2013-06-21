#include "unit.h"
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
    ql_foreach(u->orders, free);
    ql_free(u->orders);
    free(u->litems);
    free(u->name_);
    free(u->display_);
    free(u);
}

void unit_stack(struct unit* u, struct unit *stack) {
    unit **up = &stack->child;
    assert(!u->stack);
    while (*up) up = &(*up)->next;
    *up = u;
    u->stack = stack;
}

void unit_unstack(struct unit* u) {
    if (u->stack) {
        unit ** up = &u->stack->child;
        while (*up!=u) {
            up = &(*up)->next;
        }
        if (*up) {
            *up = u->next;
            u->next = 0;
            u->stack = 0;
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
