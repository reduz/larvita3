#include "lua_manager.h"

#include "lua.hpp"
#include "os/memory.h"
#include "os/file_access.h"
#include "types/global_vars.h"
#include "helpers.h"

extern int tolua_larvita_open (lua_State* tolua_S);

LuaManager* LuaManager::instance = NULL;

int LuaManager::ptop::tcount = 0;

LuaManager::ptop::ptop(lua_State* p_ls){
	tnum = ++tcount;
	ls = p_ls;
	top = lua_gettop(ls);
};
LuaManager::ptop::~ptop() {

	ERR_FAIL_COND( top != lua_gettop(ls));
};


LuaManager::retop::retop(lua_State* p_ls, int p_top) {

	ls = p_ls;
	
	if (p_top == LUA_REGISTRYINDEX) {
		top = lua_gettop(ls);
	} else if (p_top < 0) {
		top = lua_gettop(ls) - p_top;
	} else {
		top = p_top;
	};
};

LuaManager::retop::~retop() {

	lua_settop(ls, top);
};

static void* lua_alloc(void* ud, void* ptr, size_t osize, size_t nsize) {

	(void)ud;  (void)osize;  /* not used */
	if (ptr == 0 && nsize == 0) return 0;
	if (nsize == 0) {
		//printf("lua: free %p\n", ptr);
		memfree(ptr);
		return NULL;
	}
	else {
		void* ret;
		if (ptr == NULL) {
			ret = memalloc(nsize);
			//printf("lua: alloc %i bytes: %p\n", nsize, ret);
		} else {
			ret = Memory::realloc_static(ptr, nsize);
			//printf("lua: realloc %i bytes: %p\n", nsize, ret);
		};
		if (!ret) {
			iprintf("error while allocating %i bytes\n", (int)nsize);
		};
		return ret;
	};
};

void LuaManager::load_args(int argc, char** argv, lua_State* ls) {

	if (!ls) {

		ls = get_lua_state();
	};

	int n = -1;
	lua_createtable(ls, argc, 0);

	for (int i=0; i<argc; i++) {

		lua_pushstring(ls, argv[i]);
		lua_rawseti(ls, -2, n++);
	};

	lua_setglobal(ls, "arg");
};


void LuaManager::finalize(lua_State* lua_state, lua_Object instance) {

	int top = lua_gettop(lua_state);
	if (instance < 0) instance = (top+1) + instance;
	lua_getglobal(lua_state, "class");
	lua_getfield(lua_state, -1, "finalize");
	lua_pushvalue(lua_state, instance);
	lua_call(lua_state, 1, 0);
	lua_settop(lua_state, top);
};

lua_State* LuaManager::create_lua_state() {

	lua_State* ret = lua_newstate(lua_alloc, 0);

	// create a table with weak values on the registry
	lua_newtable(ret);
	lua_newtable(ret);
	lua_pushstring(ret, "v");
	lua_setfield(ret, -2, "__mode");
	lua_pushvalue(ret, LUA_GLOBALSINDEX);
	lua_setfield(ret, -2, "__index");
	lua_setmetatable(ret, -2);
	lua_setfield(ret, LUA_REGISTRYINDEX, ".script_cache");

	#define open_lib(n) \
		lua_pushcfunction(ret, luaopen_##n);\
		lua_call(ret, 0, 0);

	open_lib(base);
	open_lib(table);
	open_lib(string);
	open_lib(math);
	open_lib(debug);
	open_lib(os);
	open_lib(io);

	tolua_larvita_open(ret);

	return ret;
};


lua_State* LuaManager::get_lua_state() {

	if (!lua_state) {
		lua_state = create_lua_state();
	};

	return lua_state;
};

struct reader_data {

	FileAccess* fa;
	Uint8 buffer[256];
};

static const char* reader(lua_State* ls, void* data, size_t *size) {

	reader_data* rd = (reader_data*)data;
	if (rd->fa->eof_reached()) return NULL;

	int len = sizeof(rd->buffer);
	int left = rd->fa->get_len() - rd->fa->get_pos();

	if (left < len) len = left;

	rd->fa->get_buffer(rd->buffer, len);
	*size = len;

	return (const char*)rd->buffer;
};

static bool path_is_relative(String p_path) {

	if (p_path[0] == '/') return false;
	if (p_path[1] == ':' && (p_path[2] == '/' || p_path[2] == '\\')) return false;
	if (p_path.find("://") == 3) return false;

	return true;
};

Error LuaManager::do_file(lua_State* L, String p_fname, int nret, String& error, lua_Object environment, bool p_protected) {

	Error err = load_file(L, p_fname, error);
	if (err != OK) {
		return err;
	};
	
	if (environment) {

		lua_pushvalue(L, environment);
		lua_setfenv(L, -2);
	};

	int ret = lua_dbcall(L, 0, nret);
	if (ret && !p_protected) {
		lua_error(L);
	};

	return (Error)ret;
};


Error LuaManager::load_file(lua_State* L, String p_fname, String& error) {

	String abs_fname = p_fname;

	if (path_is_relative(p_fname))
		abs_fname = "res://" + p_fname;

	FileAccess* file = FileAccess::create();
	if (!file || file->open(abs_fname, FileAccess::READ) != OK) {

		if (file)
			memdelete(file);
		error = (String)"Unable to open file " + abs_fname;
		ERR_PRINT(error.utf8().get_data());
		return ERR_FILE_CANT_OPEN;
	};

	reader_data rd;
	rd.fa = file;

	int ret = lua_load(L, &reader, &rd, abs_fname.ascii().get_data());
	memdelete(file);

	if (ret == 0) {
		return OK;
	} else {

		error = lua_tostring(L, -1);
		ERR_PRINT((String("error loading: ") +  lua_tostring(L, -1)).ascii().get_data());
		lua_pop(L, 1);
	};


	return ERR_FILE_UNRECOGNIZED;
};

Error LuaManager::do_buffer(lua_State* L, const DVector<Uint8>& p_vector, String p_name, int nret, String& error, lua_Object environment, bool p_protected) {

	Error err = load_buffer(L, p_vector, p_name, error);
	if (err != OK) {
		return err;
	};

	if (environment) {

		lua_pushvalue(L, environment);
		lua_setfenv(L, -2);
	};

	int ret = lua_dbcall(L, 0, nret);
	if (ret && !p_protected) {
		lua_error(L);
	};

	return (Error)ret;
};


Error LuaManager::load_buffer(lua_State* L, const DVector<Uint8>& p_vector, String p_name, String& error) {

	p_vector.read_lock();
	int ret = luaL_loadbuffer(L, (const char*)p_vector.read(), p_vector.size(), p_name.ascii().get_data());
	p_vector.read_unlock();

	if (ret != 0) {
		error = lua_tostring(L, -1);
		ERR_PRINT((String("error loading: ") +  lua_tostring(L, -1)).ascii().get_data());
		lua_pop(L, 1);
	};

	return (Error)ret;
};

Error LuaManager::require(String p_name, String& p_error) {

	if (!lua_state) {
		lua_state = create_lua_state();
	};
	
	LuaManager::retop rt(lua_state, lua_gettop(lua_state));
	
	lua_getfield(lua_state, LUA_GLOBALSINDEX, "require");
	ERR_FAIL_COND_V(lua_isnil(lua_state, -1), ERR_BUG);
	
	lua_pushstring(lua_state, p_name.ascii().get_data());
	lua_pushboolean(lua_state, 0);

	if (lua_dbcall(lua_state, 2, 2) != 0) {
		
		p_error = lua_tostring(lua_state, -1);
		return FAILED;
	};

	return OK;
};

LuaManager* LuaManager::get_singleton() {

	return instance;
};

void LuaManager::set_load_mode(LuaManager::LoadMode p_mode) {
	
	load_mode = p_mode;
};

LuaManager::LoadMode LuaManager::get_load_mode() {
	
	return load_mode;
};


#include "iapi_persist.h"
#include "lua/script_lua.h"

LuaManager::LuaManager() {

	instance = this;
	lua_state = NULL;
	load_mode = MODE_FILESYSTEM;

	IAPI_REGISTER_TYPE_STATIC(ScriptLua);
};
