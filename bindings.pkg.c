/*
** Lua binding: bindings
** Generated automatically by tolua++-1.0.93 on Tue Feb 25 07:57:51 2014.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_bindings_open (lua_State* tolua_S);

#include "luabind.h"
#include "atlantis.h"
#include "faction.h"
#include "unit.h"
#include "region.h"
#include "game.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"region");
 tolua_usertype(tolua_S,"faction");
 tolua_usertype(tolua_S,"unit");
}

/* get function: no of class  unit */
#ifndef TOLUA_DISABLE_tolua_get_unit_id
static int tolua_get_unit_id(lua_State* tolua_S)
{
  unit* self = (unit*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'no'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->no);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: no of class  unit */
#ifndef TOLUA_DISABLE_tolua_set_unit_id
static int tolua_set_unit_id(lua_State* tolua_S)
{
  unit* self = (unit*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'no'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->no = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: number of class  unit */
#ifndef TOLUA_DISABLE_tolua_get_unit_number
static int tolua_get_unit_number(lua_State* tolua_S)
{
  unit* self = (unit*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'number'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->number);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: number of class  unit */
#ifndef TOLUA_DISABLE_tolua_set_unit_number
static int tolua_set_unit_number(lua_State* tolua_S)
{
  unit* self = (unit*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'number'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->number = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: money of class  unit */
#ifndef TOLUA_DISABLE_tolua_get_unit_money
static int tolua_get_unit_money(lua_State* tolua_S)
{
  unit* self = (unit*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'money'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->money);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: money of class  unit */
#ifndef TOLUA_DISABLE_tolua_set_unit_money
static int tolua_set_unit_money(lua_State* tolua_S)
{
  unit* self = (unit*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'money'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->money = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: faction of class  unit */
#ifndef TOLUA_DISABLE_tolua_get_unit_faction_ptr
static int tolua_get_unit_faction_ptr(lua_State* tolua_S)
{
  unit* self = (unit*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'faction'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->faction,"faction");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: region of class  unit */
#ifndef TOLUA_DISABLE_tolua_get_unit_region_ptr
static int tolua_get_unit_region_ptr(lua_State* tolua_S)
{
  unit* self = (unit*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'region'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->region,"region");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: unit_name of class  unit */
#ifndef TOLUA_DISABLE_tolua_get_unit_name
static int tolua_get_unit_name(lua_State* tolua_S)
{
  unit* self = (unit*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'unit_name'",NULL);
#endif
  tolua_pushstring(tolua_S,(const char*)get_unit_name(self));
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: unit_name of class  unit */
#ifndef TOLUA_DISABLE_tolua_set_unit_name
static int tolua_set_unit_name(lua_State* tolua_S)
{
  unit* self = (unit*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'unit_name'",NULL);
  if (!tolua_isstring(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  set_unit_name(this, ((tolua_outside  char*)  tolua_tostring(tolua_S,2,0))
)
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: no of class  faction */
#ifndef TOLUA_DISABLE_tolua_get_faction_id
static int tolua_get_faction_id(lua_State* tolua_S)
{
  faction* self = (faction*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'no'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->no);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: no of class  faction */
#ifndef TOLUA_DISABLE_tolua_set_faction_id
static int tolua_set_faction_id(lua_State* tolua_S)
{
  faction* self = (faction*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'no'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->no = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: faction_name of class  faction */
#ifndef TOLUA_DISABLE_tolua_get_faction_name
static int tolua_get_faction_name(lua_State* tolua_S)
{
  faction* self = (faction*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'faction_name'",NULL);
#endif
  tolua_pushstring(tolua_S,(const char*)get_faction_name(self));
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: faction_name of class  faction */
#ifndef TOLUA_DISABLE_tolua_set_faction_name
static int tolua_set_faction_name(lua_State* tolua_S)
{
  faction* self = (faction*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'faction_name'",NULL);
  if (!tolua_isstring(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  set_faction_name(this, ((tolua_outside  char*)  tolua_tostring(tolua_S,2,0))
)
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: faction_addr of class  faction */
#ifndef TOLUA_DISABLE_tolua_get_faction_email
static int tolua_get_faction_email(lua_State* tolua_S)
{
  faction* self = (faction*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'faction_addr'",NULL);
#endif
  tolua_pushstring(tolua_S,(const char*)get_faction_addr(self));
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: faction_addr of class  faction */
#ifndef TOLUA_DISABLE_tolua_set_faction_email
static int tolua_set_faction_email(lua_State* tolua_S)
{
  faction* self = (faction*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'faction_addr'",NULL);
  if (!tolua_isstring(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  set_faction_addr(this, ((tolua_outside  char*)  tolua_tostring(tolua_S,2,0))
)
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: uid of class  region */
#ifndef TOLUA_DISABLE_tolua_get_region_id
static int tolua_get_region_id(lua_State* tolua_S)
{
  region* self = (region*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uid'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->uid);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: uid of class  region */
#ifndef TOLUA_DISABLE_tolua_set_region_id
static int tolua_set_region_id(lua_State* tolua_S)
{
  region* self = (region*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uid'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->uid = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: x of class  region */
#ifndef TOLUA_DISABLE_tolua_get_region_x
static int tolua_get_region_x(lua_State* tolua_S)
{
  region* self = (region*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'x'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->x);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: x of class  region */
#ifndef TOLUA_DISABLE_tolua_set_region_x
static int tolua_set_region_x(lua_State* tolua_S)
{
  region* self = (region*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'x'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->x = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: y of class  region */
#ifndef TOLUA_DISABLE_tolua_get_region_y
static int tolua_get_region_y(lua_State* tolua_S)
{
  region* self = (region*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'y'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->y);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: y of class  region */
#ifndef TOLUA_DISABLE_tolua_set_region_y
static int tolua_set_region_y(lua_State* tolua_S)
{
  region* self = (region*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'y'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->y = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: peasants of class  region */
#ifndef TOLUA_DISABLE_tolua_get_region_peasants
static int tolua_get_region_peasants(lua_State* tolua_S)
{
  region* self = (region*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'peasants'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->peasants);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: peasants of class  region */
#ifndef TOLUA_DISABLE_tolua_set_region_peasants
static int tolua_set_region_peasants(lua_State* tolua_S)
{
  region* self = (region*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'peasants'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->peasants = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: money of class  region */
#ifndef TOLUA_DISABLE_tolua_get_region_money
static int tolua_get_region_money(lua_State* tolua_S)
{
  region* self = (region*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'money'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->money);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: money of class  region */
#ifndef TOLUA_DISABLE_tolua_set_region_money
static int tolua_set_region_money(lua_State* tolua_S)
{
  region* self = (region*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'money'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->money = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: region_name of class  region */
#ifndef TOLUA_DISABLE_tolua_get_region_name
static int tolua_get_region_name(lua_State* tolua_S)
{
  region* self = (region*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'region_name'",NULL);
#endif
  tolua_pushstring(tolua_S,(const char*)get_region_name(self));
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: region_name of class  region */
#ifndef TOLUA_DISABLE_tolua_set_region_name
static int tolua_set_region_name(lua_State* tolua_S)
{
  region* self = (region*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'region_name'",NULL);
  if (!tolua_isstring(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  set_region_name(this, ((tolua_outside  char*)  tolua_tostring(tolua_S,2,0))
)
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: region_terrain of class  region */
#ifndef TOLUA_DISABLE_tolua_get_region_terrain
static int tolua_get_region_terrain(lua_State* tolua_S)
{
  region* self = (region*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'region_terrain'",NULL);
#endif
  tolua_pushstring(tolua_S,(const char*)get_region_terrain(self));
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: region_terrain of class  region */
#ifndef TOLUA_DISABLE_tolua_set_region_terrain
static int tolua_set_region_terrain(lua_State* tolua_S)
{
  region* self = (region*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'region_terrain'",NULL);
  if (!tolua_isstring(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  set_region_terrain(this, ((tolua_outside  char*)  tolua_tostring(tolua_S,2,0))
)
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* function: regions_create */
#ifndef TOLUA_DISABLE_tolua_bindings_atlantis_regions_create00
static int tolua_bindings_atlantis_regions_create00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int x = ((int)  tolua_tonumber(tolua_S,1,0));
  int y = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* terrain = ((const char*)  tolua_tostring(tolua_S,3,0));
  {
   region* tolua_ret = (region*)  regions_create(x,y,terrain);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"region");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'create'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: findregion */
#ifndef TOLUA_DISABLE_tolua_bindings_atlantis_regions_get00
static int tolua_bindings_atlantis_regions_get00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int x = ((int)  tolua_tonumber(tolua_S,1,0));
  int y = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   region* tolua_ret = (region*)  findregion(x,y);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"region");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: create_faction */
#ifndef TOLUA_DISABLE_tolua_bindings_atlantis_factions_create00
static int tolua_bindings_atlantis_factions_create00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int no = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   faction* tolua_ret = (faction*)  create_faction(no);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"faction");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'create'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: findfaction */
#ifndef TOLUA_DISABLE_tolua_bindings_atlantis_factions_get00
static int tolua_bindings_atlantis_factions_get00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int id = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   faction* tolua_ret = (faction*)  findfaction(id);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"faction");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: turn */
#ifndef TOLUA_DISABLE_tolua_get_atlantis_turn
static int tolua_get_atlantis_turn(lua_State* tolua_S)
{
  tolua_pushnumber(tolua_S,(lua_Number)turn);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: turn */
#ifndef TOLUA_DISABLE_tolua_set_atlantis_turn
static int tolua_set_atlantis_turn(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  turn = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* function: cleargame */
#ifndef TOLUA_DISABLE_tolua_bindings_atlantis_free_game00
static int tolua_bindings_atlantis_free_game00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isboolean(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  bool config = ((bool)  tolua_toboolean(tolua_S,1,0));
  {
   cleargame(config);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'free_game'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: read_config */
#ifndef TOLUA_DISABLE_tolua_bindings_atlantis_read_config00
static int tolua_bindings_atlantis_read_config00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* filename = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   int tolua_ret = (int)  read_config(filename);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'read_config'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: read_game */
#ifndef TOLUA_DISABLE_tolua_bindings_atlantis_read_game00
static int tolua_bindings_atlantis_read_game00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* filename = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   int tolua_ret = (int)  read_game(filename);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'read_game'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: write_game */
#ifndef TOLUA_DISABLE_tolua_bindings_atlantis_write_game00
static int tolua_bindings_atlantis_write_game00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* filename = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   int tolua_ret = (int)  write_game(filename);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'write_game'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: write_reports */
#ifndef TOLUA_DISABLE_tolua_bindings_atlantis_write_reports00
static int tolua_bindings_atlantis_write_reports00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"faction",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  faction* f = ((faction*)  tolua_tousertype(tolua_S,1,0));
  {
   write_reports(f);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'write_reports'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: readorders */
#ifndef TOLUA_DISABLE_tolua_bindings_atlantis_read_orders00
static int tolua_bindings_atlantis_read_orders00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* filename = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   int tolua_ret = (int)  readorders(filename);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'read_orders'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: processorders */
#ifndef TOLUA_DISABLE_tolua_bindings_atlantis_process00
static int tolua_bindings_atlantis_process00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   processorders();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'process'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_bindings_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_module(tolua_S,"atlantis",1);
  tolua_beginmodule(tolua_S,"atlantis");
   tolua_cclass(tolua_S,"unit","unit","",NULL);
   tolua_beginmodule(tolua_S,"unit");
    tolua_variable(tolua_S,"id",tolua_get_unit_id,tolua_set_unit_id);
    tolua_variable(tolua_S,"number",tolua_get_unit_number,tolua_set_unit_number);
    tolua_variable(tolua_S,"money",tolua_get_unit_money,tolua_set_unit_money);
    tolua_variable(tolua_S,"faction",tolua_get_unit_faction_ptr,NULL);
    tolua_variable(tolua_S,"region",tolua_get_unit_region_ptr,NULL);
    tolua_variable(tolua_S,"name",tolua_get_unit_name,tolua_set_unit_name);
   tolua_endmodule(tolua_S);
   tolua_cclass(tolua_S,"faction","faction","",NULL);
   tolua_beginmodule(tolua_S,"faction");
    tolua_variable(tolua_S,"id",tolua_get_faction_id,tolua_set_faction_id);
    tolua_variable(tolua_S,"name",tolua_get_faction_name,tolua_set_faction_name);
    tolua_variable(tolua_S,"email",tolua_get_faction_email,tolua_set_faction_email);
   tolua_endmodule(tolua_S);
   tolua_cclass(tolua_S,"region","region","",NULL);
   tolua_beginmodule(tolua_S,"region");
    tolua_variable(tolua_S,"id",tolua_get_region_id,tolua_set_region_id);
    tolua_variable(tolua_S,"x",tolua_get_region_x,tolua_set_region_x);
    tolua_variable(tolua_S,"y",tolua_get_region_y,tolua_set_region_y);
    tolua_variable(tolua_S,"peasants",tolua_get_region_peasants,tolua_set_region_peasants);
    tolua_variable(tolua_S,"money",tolua_get_region_money,tolua_set_region_money);
    tolua_variable(tolua_S,"name",tolua_get_region_name,tolua_set_region_name);
    tolua_variable(tolua_S,"terrain",tolua_get_region_terrain,tolua_set_region_terrain);
   tolua_endmodule(tolua_S);
   tolua_module(tolua_S,"regions",0);
   tolua_beginmodule(tolua_S,"regions");
    tolua_function(tolua_S,"create",tolua_bindings_atlantis_regions_create00);
    tolua_function(tolua_S,"get",tolua_bindings_atlantis_regions_get00);
   tolua_endmodule(tolua_S);
   tolua_module(tolua_S,"factions",0);
   tolua_beginmodule(tolua_S,"factions");
    tolua_function(tolua_S,"create",tolua_bindings_atlantis_factions_create00);
    tolua_function(tolua_S,"get",tolua_bindings_atlantis_factions_get00);
   tolua_endmodule(tolua_S);
   tolua_variable(tolua_S,"turn",tolua_get_atlantis_turn,tolua_set_atlantis_turn);
   tolua_function(tolua_S,"free_game",tolua_bindings_atlantis_free_game00);
   tolua_function(tolua_S,"read_config",tolua_bindings_atlantis_read_config00);
   tolua_function(tolua_S,"read_game",tolua_bindings_atlantis_read_game00);
   tolua_function(tolua_S,"write_game",tolua_bindings_atlantis_write_game00);
   tolua_function(tolua_S,"write_reports",tolua_bindings_atlantis_write_reports00);
   tolua_function(tolua_S,"read_orders",tolua_bindings_atlantis_read_orders00);
   tolua_function(tolua_S,"process",tolua_bindings_atlantis_process00);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_bindings (lua_State* tolua_S) {
 return tolua_bindings_open(tolua_S);
};
#endif

