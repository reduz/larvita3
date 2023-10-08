#ifndef LUA_MANAGER_H
#define LUA_MANAGER_H

#include "error_list.h"
#include "types/string/rstring.h"
#include "lua_iapi_factory.h"
#include "lua.hpp"


class LuaManager {

public:
	
	enum LoadMode {
		MODE_FILESYSTEM,
		MODE_DATABASE,
		MODE_FILESYSTEM_DATABASE,
	};

private:
	static LuaManager* instance;

	lua_State* lua_state;
	
	LuaIAPIFactory factory;

	LoadMode load_mode;
	
public:

	class retop {
		
		int top;
		lua_State* ls;
		
	public:
		retop(lua_State* p_ls, int p_top = LUA_REGISTRYINDEX);
		~retop();
	};
	class ptop {
		
		static int tcount;
		int tnum;
		int top;
		lua_State* ls;
		
	public:
		ptop(lua_State* p_ls);
		~ptop();
	};

	void set_load_mode(LoadMode p_mode);
	LoadMode get_load_mode();
	
	
	void load_args(int argc, char** argv, lua_State* ls = NULL);

	lua_State* create_lua_state();
	lua_State* get_lua_state();

	static void finalize(lua_State* L, lua_Object p_instance);

	Error require(String p_name, String& p_error);

	Error do_file(lua_State* L, String p_fname, int nret, String& error, lua_Object environment = 0, bool p_protected = true);
	Error do_buffer(lua_State* L, const DVector<Uint8>& p_vector, String p_name, int nret, String& error, lua_Object environment = 0, bool p_protected = true);
	
	Error load_file(lua_State* L, String p_fname, String& error);
	Error load_buffer(lua_State* L, const DVector<Uint8>& p_vector, String p_name, String& error);

	static LuaManager* get_singleton();

	LuaManager();
};







#endif
