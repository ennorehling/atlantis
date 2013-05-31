#ifndef ATL_REPORT_H
#define ATL_REPORT_H

struct unit;
struct faction;
struct ship;
struct building;
struct region;

const char *unitid(const struct unit *u);
const char *shipid(const struct ship * sh);
const char *buildingid(const struct building * b);
const char *factionid(const struct faction * f);
const char *regionid(const struct region * r, const struct faction * f);

extern char buf[];
void scat(const char *s);
void icat(int n);

void mistake(struct faction * f, const char *s, const char *comment);
void mistakes(struct unit * u, const char *str, const char *comment);
void mistakeu(struct unit * u, const char *comment);

#endif
