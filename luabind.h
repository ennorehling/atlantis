#ifndef ATL_BINDING_H
#define ATL_BINDING_H

/* work around a bug in tolua++ */
#define this self

struct region;
#define set_region_name(r, n)  region_setname(r, n)
#define get_region_name(r) region_getname(r)
void set_region_terrain(struct region *r, const char *terrain);
const char * get_region_terrain(const struct region *r);

struct faction;
#define set_faction_name(self, n) faction_setname(self, n)
#define get_faction_name(self) faction_getname(self)
#define set_faction_addr(self, n) faction_setaddr(self, n)
#define get_faction_addr(self) faction_getaddr(self)

struct unit;
#define set_unit_name(self, n) unit_setname(self, n)
#define get_unit_name(self) unit_getname(self)

struct region *regions_create(int x, int y, const char *terrain);

#endif
