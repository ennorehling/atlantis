#ifndef ATL_PARSER_H
#define ATL_PARSER_H

#include "keywords.h"

struct region;
struct faction;
struct unit;

enum {
    U_NOTFOUND,
    U_NONE,
    U_UNIT,
    U_PEASANTS,
};

int getunit(struct region * r, const struct faction *f, struct unit **uptr);
struct unit *getunitg(struct region *r, const struct faction *f);

extern const char *keywords[];
extern const char *terrainnames[];
extern const char *skillnames[];
extern const char *itemnames[][2];
extern const char *spellnames[];

keyword_t findkeyword(const char *s);
keyword_t igetkeyword(const char *s);
keyword_t getkeyword(void);
int getitem(void);
int getskill(void);
int getspell(void);
char *igetstr(const char *s);
char *getstr(void);
int findspell(const char *s);

#endif
