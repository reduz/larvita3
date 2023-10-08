#include "lua_iapi_factory.h"

#include "lua/helpers.h"

LuaIAPIFactory* LuaIAPIFactory::instance = NULL;

void LuaIAPIFactory::register_type(lua_State* p_ls, String p_type, lua_Object p_slot, bool p_singleton,const IAPI::MethodInfo& p_instance_hints) {

	TypeInfo ti;
	ti.ls = p_ls;

	lua_pushvalue(p_ls, p_slot);
	ti.slot = lua_ref(p_ls, 1);
	lua_pop(p_ls, 1);

	ERR_FAIL_COND(ti.slot == LUA_NOREF);
	
	type_info_table.set(p_type, ti);
	
	IAPI_Persist::get_singleton()->register_type(p_type, &LuaIAPIFactory::instance_func, p_singleton, p_instance_hints);
};

IRef<IAPI> LuaIAPIFactory::instance_func(const String& p_type, const IAPI::CreationParams& p_params) {
	
	ERR_FAIL_COND_V(!instance, IRef<IAPI>());
	ERR_FAIL_COND_V(!instance->type_info_table.has(p_type), IRef<IAPI>());
	
	lua_State* ls = instance->type_info_table[p_type].ls;
	ERR_FAIL_COND_V(!ls, IRef<IAPI>());
	
	int ref = instance->type_info_table[p_type].slot;
	ERR_FAIL_COND_V(ref == LUA_NOREF, IRef<IAPI>());

	int top = lua_gettop(ls);
	
	lua_getref(ls, ref);
	lua_pushstring(ls, p_type.utf8().get_data());
	tolua_pushusertype(ls, (void*)&p_params, "Table<String,Variant>");
	
	lua_dbcall(ls, 2, 1);
	
	IAPI* val = (IAPI*)tolua_tousertype(ls, -1, 0);
	lua_settop(ls, top);
	
	return val;
};

LuaIAPIFactory* LuaIAPIFactory::get_singleton() {
	
	return instance;
};

LuaIAPIFactory::LuaIAPIFactory() {
	
	ERR_FAIL_COND(instance != NULL);
	instance = this;
};

LuaIAPIFactory::~LuaIAPIFactory() {

	const String *k = type_info_table.next(NULL);
	while (k) {
		
		lua_unref(type_info_table[*k].ls, type_info_table[*k].slot);
		k = type_info_table.next(k);
	};

	
	ERR_FAIL_COND(instance != this);
	instance = NULL;
};

