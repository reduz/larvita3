#ifndef LUA_DEFINES_H
#define LUA_DEFINES_H

#define tolua_isrstring tolua_isstring
#define tolua_torstring(L, i, d) String::utf8(lua_isnil(L,i)?"":tolua_tostring(L,i,d))
#define tolua_pushrstring(L,s) tolua_pushstring(L, s.utf8().get_data())

#define tolua_pushstate(L, l) (void)l;lua_pushnil(L)

#include "typedefs.h"

#undef Mtolua_new
#define Mtolua_new memnew

#undef Mtolua_new_dim
#define Mtolua_new_dim memnew_arr

#undef Mtolua_delete
#define Mtolua_delete(exp) memdelete(exp)

#define tolua_owned

#endif
