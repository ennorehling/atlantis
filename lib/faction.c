/*
 * Atlantis v1.0 13 September 1993
 * Copyright 1993 by Russell Wallace
 *
 * This program may be freely used, modified and distributed. It may not be
 * sold or used commercially without prior written permission from the author.
 */

#include "faction.h"

#include "atlantis.h"
#include "battle.h"

#include <quicklist.h>
#include <mtrand.h>
#include <md5.h>

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

faction *factions;

faction * create_faction(int no)
{
    faction * f = (faction *)calloc(1, sizeof(faction));
    if (f) {
        faction **iter;

        f->no = no;
        for (iter=&factions; *iter; iter=&(*iter)->next);
        *iter = f;
    }
    return f;
}

void free_faction(faction *f) {
    free(f->name_);
    free(f->addr_);
    free(f->pwhash_);
/*
    ql_foreach(f->messages, free);
    ql_free(f->messages);
    ql_foreach(f->events, free);
    ql_free(f->events);
    ql_foreach(f->mistakes, free);
    ql_free(f->mistakes);
    ql_foreach(f->allies.factions, free);
    ql_free(f->allies.factions);
*/
    ql_foreach(f->battles, (void (*)(void *))free_battle);
    ql_free(f->battles);
    freestrlist(&f->events);
    freestrlist(&f->messages);
    ql_foreach(f->mistakes, free);
    ql_free(f->mistakes);

    ql_free(f->allies.factions);
    ql_foreach(f->accept, free);
    ql_free(f->accept);
    ql_foreach(f->admit, free);
    ql_free(f->admit);
    free(f);
}

void faction_setname(faction * f, const char * name) {
    if (name) {
        f->name_ = (char *)realloc(f->name_, strlen(name)+1);
        strcpy(f->name_, name);
    } else {
        free(f->name_);
        f->name_ = 0;
    }
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

void faction_setpwhash(faction * f, const char * pwhash) {
    if (pwhash) {
        f->pwhash_ = (char *)realloc(f->pwhash_, strlen(pwhash)+1);
        strcpy(f->pwhash_, pwhash);
    } else {
        free(f->pwhash_);
        f->pwhash_ = 0;
    }
}

const char * faction_getpwhash(const faction * f) {
    return f->pwhash_;
}

void hex_encode(unsigned char * in, size_t inlen, char * out) {
    size_t i;
    for (i=0;i!=inlen;++i, out+=2) {
        sprintf(out, "%02x", in[i]);
    }
}

void faction_setpassword(faction * f, const char * password) {
    if (password) {
        char pwhash[64];
        int salt = genrand_int31() % 0x10000;
        md5_state_t pms;
        md5_byte_t digest[16];

        sprintf(pwhash, "%04x", salt);

        md5_init(&pms);
        md5_append(&pms, (const md5_byte_t *)pwhash, 4);
        md5_append(&pms, (const md5_byte_t *)password, strlen(password));
        md5_finish(&pms, digest);

        hex_encode(digest, sizeof(digest), pwhash+4);
        faction_setpwhash(f, pwhash);
    } else {
        faction_setpwhash(f, 0);
    }
}

bool faction_checkpassword(struct faction *f, const char * password)
{
    char buffer[64];
    md5_state_t pms;
    md5_byte_t digest[16];

    if (f->pwhash_) {
        md5_init(&pms);
        md5_append(&pms, (const md5_byte_t *)f->pwhash_, 4);
        md5_append(&pms, (const md5_byte_t *)password, strlen(password));
        md5_finish(&pms, digest);

        hex_encode(digest, sizeof(digest), buffer);
        return strcmp(buffer, f->pwhash_+4)==0;
    }
    return false;
}
