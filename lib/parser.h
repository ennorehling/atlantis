#ifndef ATL_PARSER_H
#define ATL_PARSER_H

struct region;
struct faction;
struct unit;

int findkeyword(const char *s);
int igetkeyword(const char *s);
int getkeyword(void);
int getitem(void);
int getskill(void);
int getspell(void);
char *igetstr(const char *s);
char *getstr(void);
int findspell(const char *s);

enum {
    U_NOTFOUND,
    U_NONE,
    U_UNIT,
    U_PEASANTS,
};

int getunit(const struct region * r, const struct faction *f, struct unit **uptr);
struct unit *getunitg(const struct region *r, const struct faction *f);

extern const char *keywords[];
extern const char *terrainnames[];
extern const char *skillnames[];
extern const char *itemnames[][2];
extern const char *spellnames[];

#endif
