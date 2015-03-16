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

int getunit(struct region * r, struct unit **uptr);
struct unit *getunitg(void);

extern const char *keywords[];
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
int findspell(const char *s);
int finddirection(const char *s);
extern const char* direction_name(int);

#endif
