#ifndef LUA_SLOT_H
#define LUA_SLOT_H

#include "lua.hpp"
typedef int lua_Object;

#include "os/memory.h"
#include "types/string/rstring.h"

struct LuaSlot {

	int slot;
	lua_State* ls;
	int* refcount;

	void init(lua_State* p_ls, lua_Object p_obj);
	void unreference();
	
	void reference(const LuaSlot& p_r);

	bool is_empty();

	void push_call();

	LuaSlot& operator=(const LuaSlot& p_r);
	
	LuaSlot(const LuaSlot& p_r);
	LuaSlot(lua_State* p_ls, lua_Object p_obj);
	LuaSlot();

	~LuaSlot();
};

#endif

