/*
 * Atlantis v1.0 13 September 1993
 * Copyright 1993 by Russell Wallace
 *
 * This program may be freely used, modified and distributed. It may not be
 * sold or used commercially without prior written permission from the author.
 */

#include "faction.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "crypto/mtrand.h"
#include "crypto/base64.h"
#include "crypto/md5.h"

faction *factions;

faction * create_faction(int no)
{
    faction * f = (faction *)calloc(1, sizeof(faction));
    faction **iter;

    f->no = no;
    for (iter=&factions; *iter; iter=&(*iter)->next);
    *iter = f;
    return f;
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

void faction_setpassword(faction * f, const char * password) {
    if (password) {
        char pwhash[64];
        int salt = genrand_int31() % 4096;
        md5_state_t pms;
        md5_byte_t digest[16];

        pwhash[0] = base64_encode_value(salt / 64);
        pwhash[1] = base64_encode_value(salt % 64);

        md5_init(&pms);
        md5_append(&pms, (const md5_byte_t *)pwhash, 2);
        md5_append(&pms, (const md5_byte_t *)password, strlen(password));
        md5_finish(&pms, digest);

        base64_encode(digest, sizeof(digest), pwhash+2, sizeof(pwhash)-2);
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
        md5_append(&pms, (const md5_byte_t *)f->pwhash_, 2);
        md5_append(&pms, (const md5_byte_t *)password, strlen(password));
        md5_finish(&pms, digest);

        base64_encode(digest, sizeof(digest), buffer, sizeof(buffer));
        return strcmp(buffer, f->pwhash_+2)==0;
    }
    return false;
}