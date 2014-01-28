#include "luabind.h"
#include "region.h"
#include "unit.h"
#include "faction.h"

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

static int tolua_get_regions(lua_State * L)
{
  region **region_ptr = (region **) lua_newuserdata(L, sizeof(region *));
  luaL_getmetatable(L, "region");
  lua_setmetatable(L, -2);
  *region_ptr = regions;
  lua_pushcclosure(L, tolua_regionlist_next, 1);
  return 1;
}

extern int tolua_bindings_open(lua_State* tolua_S);

TOLUA_API int luaopen_atlantis (lua_State* tolua_S) {
    tolua_open(tolua_S);
    tolua_module(tolua_S,NULL,0);
    tolua_beginmodule(tolua_S,NULL);
    tolua_beginmodule(tolua_S,"atlantis");
    tolua_variable(tolua_S, "regions", tolua_get_regions, NULL);
    tolua_endmodule(tolua_S);
    tolua_endmodule(tolua_S);
    return tolua_bindings_open(tolua_S);
};
