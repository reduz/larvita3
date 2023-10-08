#ifndef LUA_IAPI_FACTORY_H
#define LUA_IAPI_FACTORY_H

#include "types/iapi/iapi_persist.h"
#include "lua/include/lua.hpp"

class LuaIAPIFactory {

	static LuaIAPIFactory* instance;

	struct TypeInfo {

		lua_State* ls;
		int slot;
	};
	
	Table<String,TypeInfo> type_info_table;
	
public:

	void register_type(lua_State* p_ls, String p_type, lua_Object p_slot, bool p_singleton=false,const IAPI::MethodInfo& p_instance_hints=IAPI::MethodInfo());

	static IRef<IAPI> instance_func(const String& p_type, const IAPI::CreationParams& p_params);
	static LuaIAPIFactory* get_singleton();
	
	LuaIAPIFactory();
	~LuaIAPIFactory();
};

#endif
