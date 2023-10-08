#ifndef LUA_HELPERS_H
#define LUA_HELPERS_H

#include "variant.h"

struct lua_State;
struct tolua_Error;

void tolua_pushiobj_noref(lua_State* L, void* p_var, const char* p_type);

void tolua_pushiobj(lua_State* L, const IObjRef& var, const char* p_type = "", bool skip_iref = false);
int tolua_isiobj(lua_State* L, int lo, int def, tolua_Error* err);
IObjRef tolua_toiobj(lua_State* L, int lo, void* def);

void tolua_pushVariant(lua_State* L, const Variant& var);
Variant tolua_toVariant(lua_State* L, int lo, void* def);
int tolua_isVariant(lua_State* L, int lo, int def, tolua_Error* err);

int lua_dbcall(lua_State* L, int p, int r, String f = "print_traceback");

#endif
