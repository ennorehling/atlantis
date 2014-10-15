#include "luabind.h"
#include "region.h"
#include "unit.h"
#include "faction.h"

#include <quicklist.h>

#include <lua.h>
#include <tolua++.h>

void set_region_terrain(region *r, const char *terrain) {
    const struct terrain *t = get_terrain_by_name(terrain);
    if (t) {
        r->terrain = t;
    }
}

const char * get_region_terrain(const region *r) {
    return r->terrain ? r->terrain->name : 0;
}

region *regions_create(int x, int y, const char *terrain) {
    const struct terrain *t = get_terrain_by_name(terrain);
    if (t) {
        coor_transform(COOR_TORUS, &x, &y);
        return create_region(0, x, y, t);
    }
    return 0;
}

static int tolua_quicklist_iter(lua_State * L)
{
  quicklist **qlp = (quicklist **) lua_touserdata(L, lua_upvalueindex(1));
  quicklist *ql = *qlp;
  if (ql != NULL) {
    int index = lua_tointeger(L, lua_upvalueindex(2));
    const char *type = lua_tostring(L, lua_upvalueindex(3));
    void *data = ql_get(ql, index);
    tolua_pushusertype(L, data, type);
    ql_advance(qlp, &index, 1);
    tolua_pushnumber(L, index);
    lua_replace(L, lua_upvalueindex(2));
    return 1;
  }
  return 0;
}

int tolua_quicklist_push(struct lua_State *L, const char *list_type,
  const char *elem_type, struct quicklist *list)
{
    quicklist **qlist_ptr =
      (quicklist **) lua_newuserdata(L, sizeof(quicklist *));
    *qlist_ptr = list;
    luaL_getmetatable(L, list_type);
    lua_setmetatable(L, -2);
    lua_pushnumber(L, 0);
    lua_pushstring(L, elem_type);
    lua_pushcclosure(L, tolua_quicklist_iter, 3);       /* OBS: this closure has multiple upvalues (list, index, type_name) */
    return 1;
}

static int tolua_unitlist_next(lua_State * L)
{
    unit **unit_ptr = (unit **) lua_touserdata(L, lua_upvalueindex(1));
    unit *u = *unit_ptr;
    if (!u) {
        return 0;                   /* no more values to return */
    }
    tolua_pushusertype(L, (void *)u, "unit");
    *unit_ptr = u->next;
    return 1;
}

static int tolua_get_region_units(lua_State * L)
{
    region* self = (region*)  tolua_tousertype(L, 1, 0);
#ifndef TOLUA_RELEASE
    if (!self) tolua_error(L, "invalid 'self' in accessing variable 'units'",NULL);
#endif
    unit **unit_ptr = (unit **) lua_newuserdata(L, sizeof(unit *));
    luaL_getmetatable(L, "unit");
    lua_setmetatable(L, -2);
    *unit_ptr = self->units;
    lua_pushcclosure(L, tolua_unitlist_next, 1);
    return 1;
}

static int tolua_get_regions(lua_State * L)
{
    return tolua_quicklist_push(L, "region_list", "region", regions);
}

static int tolua_get_factions(lua_State * L)
{
    return tolua_quicklist_push(L, "faction_list", "faction", factions);
}

extern int tolua_bindings_open(lua_State* tolua_S);

TOLUA_API int luaopen_atlantis (lua_State* tolua_S) {
    tolua_bindings_open(tolua_S);
    tolua_usertype(tolua_S, "faction_list");
    tolua_usertype(tolua_S, "region_list");
    tolua_module(tolua_S, NULL, 0);
    tolua_beginmodule(tolua_S, NULL);
    tolua_module(tolua_S, "atlantis", 1);
    tolua_beginmodule(tolua_S, "atlantis");
    tolua_module(tolua_S, "regions", 1);
    tolua_beginmodule(tolua_S, "regions");
    tolua_variable(tolua_S, "all", tolua_get_regions, NULL);
    tolua_endmodule(tolua_S);
    tolua_module(tolua_S, "factions", 1);
    tolua_beginmodule(tolua_S, "factions");
    tolua_variable(tolua_S, "all", tolua_get_factions, NULL);
    tolua_endmodule(tolua_S);
    tolua_beginmodule(tolua_S, "region");
    tolua_variable(tolua_S, "units", tolua_get_region_units, NULL);
    tolua_endmodule(tolua_S);
    tolua_endmodule(tolua_S);
    tolua_endmodule(tolua_S);
    return 1;
};
