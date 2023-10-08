#include "script_lua.h"

#include "helpers.h"
#include "lua_manager.h"

#include "os/file_access.h"

struct ref_check {
	
	lua_State* lua_state;
	int top;
	int refs;
	IRef<IAPI> ref;
	String where;
	
	ref_check(lua_State* ls, IRef<IAPI> p_ref, String p_where) {
		
		lua_state = ls;
		top = lua_gettop(lua_state);
		ref = p_ref;
		p_ref.clear();
		refs = ref.get_refcount(); // 1 for the parameter
		where = p_where;
		printf("****** %p begin check with %i refs on %ls\n", ref.operator->(), ref.get_refcount(), p_where.c_str());
	};
	
	~ref_check() {
	
		printf("%i refs before retop\n", ref.get_refcount());
		lua_settop(lua_state, top);
		printf("%i refs before collection\n", ref.get_refcount());
		lua_gc(lua_state, LUA_GCCOLLECT, 0);
		lua_gc(lua_state, LUA_GCCOLLECT, 0);
		lua_gc(lua_state, LUA_GCCOLLECT, 0);
		printf("****** end with %i refs on %ls\n", ref.get_refcount(), where.c_str());
		ERR_FAIL_COND( refs != ref.get_refcount());
	};
};

void ScriptInstanceLua::set(String p_path, const Variant& p_value) {

	ERR_FAIL_COND(!lua_state || instance == LUA_NOREF);

	//ref_check rc(lua_state, script_self, "set");
		
	LuaManager::retop rt(lua_state, lua_gettop(lua_state));
	
	ERR_FAIL_COND(push_instance() != OK);

	lua_getfield(lua_state, -1, "_set");
	if (lua_isnil(lua_state, -1) || lua_iscfunction(lua_state, -1)) {
		return;
	};
	
	lua_pushvalue(lua_state, -2);
	lua_pushstring(lua_state, p_path.utf8().get_data());
	tolua_pushVariant(lua_state, p_value);
	
	ERR_FAIL_COND(lua_dbcall(lua_state, 3, 0) != 0);
};

Variant ScriptInstanceLua::get(String p_path) const {

	ERR_FAIL_COND_V(!lua_state || instance == LUA_NOREF, Variant());
	//ref_check rc(lua_state, script_self, "get");
	LuaManager::retop rt(lua_state, lua_gettop(lua_state));
	
	ERR_FAIL_COND_V(push_instance() != OK, Variant());

	lua_getfield(lua_state, -1, "_get");
	if (lua_isnil(lua_state, -1) || lua_iscfunction(lua_state, -1)) {
		return Variant();
	};
	
	lua_pushvalue(lua_state, -2);
	lua_pushstring(lua_state, p_path.utf8().get_data());
	
	ERR_FAIL_COND_V(lua_dbcall(lua_state, 2, 1) != 0, Variant());
	
	Variant ret = tolua_toVariant(lua_state, lua_gettop(lua_state), 0);
	
	return ret;
};

void ScriptInstanceLua::get_property_list( List<PropertyInfo> *p_list ) const {

	ERR_FAIL_COND(!lua_state || instance == LUA_NOREF);
	//ref_check rc(lua_state, script_self, "get_property_list");
	LuaManager::retop rt(lua_state, lua_gettop(lua_state));

	ERR_FAIL_COND(push_instance() != OK);

	lua_getfield(lua_state, -1, "_get_property_list");
	if (lua_isnil(lua_state, -1) || lua_iscfunction(lua_state, -1)) {
		
		return;
	};
	
	lua_pushvalue(lua_state, -2);
	tolua_pushusertype(lua_state, p_list, "List<IAPI::PropertyInfo>");
	
	ERR_FAIL_COND(lua_dbcall(lua_state, 2, 0) != 0);
};

IAPI::PropertyHint ScriptInstanceLua::get_property_hint(String p_path) const {
	
	ERR_FAIL_COND_V(!lua_state || instance == LUA_NOREF, IAPI::PropertyHint());
	//ref_check rc(lua_state, script_self, "get_property_hint");
	LuaManager::retop rt(lua_state, lua_gettop(lua_state));

	ERR_FAIL_COND_V(push_instance() != OK, IAPI::PropertyHint());
	lua_getfield(lua_state, -1, "_get_property_hint");
	if (lua_isnil(lua_state, -1) || lua_iscfunction(lua_state, -1)) {
		
		return IAPI::PropertyHint();
	};

	lua_pushvalue(lua_state, -2);
	lua_pushstring(lua_state, p_path.utf8().get_data());
	
	ERR_FAIL_COND_V(lua_dbcall(lua_state, 2, 1) != 0, IAPI::PropertyHint());
	
	IAPI::PropertyHint ret(*((IAPI::PropertyHint*)tolua_tousertype(lua_state, lua_gettop(lua_state), 0)));
	
	return ret;
};

Variant ScriptInstanceLua::call(String p_method, const List<Variant>& p_params) {
	
	ERR_FAIL_COND_V(!lua_state || instance == LUA_NOREF, Variant());

	ERR_FAIL_COND_V( lua_checkstack(lua_state, 6 + p_params.size()) == 0, Variant() );

	//ref_check rc(lua_state, script_self, "call");

	LuaManager::retop rt(lua_state, lua_gettop(lua_state));

	ERR_FAIL_COND_V(push_instance() != OK, Variant());

	lua_getfield(lua_state, -1, "_call");
	if (lua_isnil(lua_state, -1) || lua_iscfunction(lua_state, -1)) {
		
		return Variant();
	};

	lua_pushvalue(lua_state, -2);
	lua_pushstring(lua_state, p_method.utf8().get_data());
	
	const List<Variant>::Iterator* I = p_params.begin();
	while (I) {
		
		tolua_pushVariant(lua_state, I->get());
		I = I->next();
	};
	
	ERR_FAIL_COND_V(lua_dbcall(lua_state, 2+p_params.size(), 1) != 0, Variant());

	Variant ret = tolua_toVariant(lua_state, -1, 0);

	return ret;
};

void ScriptInstanceLua::get_method_list( List<MethodInfo> *p_list ) const {

	ERR_FAIL_COND(!lua_state || instance == LUA_NOREF);

	//ref_check rc(lua_state, script_self, "get_method_list");
	LuaManager::retop rt(lua_state, lua_gettop(lua_state));

	ERR_FAIL_COND(push_instance() != OK);
	lua_getfield(lua_state, -1, "_get_method_list");
	if (lua_isnil(lua_state, -1) || lua_iscfunction(lua_state, -1)) {
		
		return;
	};
	
	lua_pushvalue(lua_state, -2);
	tolua_pushusertype(lua_state, p_list, "List<IAPI::MethodInfo>");
	
	ERR_FAIL_COND(lua_dbcall(lua_state, 2, 0) != 0);
};

void ScriptInstanceLua::set_instance(lua_State* p_lua_state, WeakRef<IAPI> p_script_self, int p_instance, String p_script_name) {

	if (lua_state && script_self) {
	
		clear();
	};
	
	lua_state = p_lua_state;
	script_self = p_script_self;
	instance = p_instance;
	script_name = p_script_name;
};

Error ScriptInstanceLua::construct() {

	//ref_check rc(lua_state, script_self, "construct");
	
	Error err = parent_script->construct_instance(this);
	ERR_FAIL_COND_V(err != OK, err);

	return OK;
};

Error ScriptInstanceLua::push_instance() const {
	
	ERR_FAIL_COND_V(!lua_state, FAILED);
	
	if (script_self) {
		
		tolua_pushiobj(lua_state, IRef<IAPI>(script_self));
		if (instance != LUA_NOREF) {
			
			lua_getref(lua_state, instance);
			ERR_FAIL_COND_V( lua_setfenv(lua_state, -2) == 0, FAILED );
		};
	} else {
		ERR_FAIL_COND_V( instance == LUA_NOREF, FAILED );
		lua_getref(lua_state, instance);
	};
	
	if (lua_isnil(lua_state, -1)) {
		lua_pop(lua_state, 1);
		return FAILED;
	};
	
	return OK;
};

void ScriptInstanceLua::clear() {

	if (lua_state && instance != LUA_NOREF) {

		push_instance();
		LuaManager::finalize(lua_state, -1);
		lua_pop(lua_state, 1);
		
		lua_unref(lua_state, instance);
		instance = LUA_NOREF;
		
		script_self = WeakRef<IAPI>();
	};
	script_name = "";
	lua_state = NULL;
};

ScriptInstanceLua::ScriptInstanceLua(WeakRef<ScriptLua> p_parent) {

	instance = LUA_NOREF;
	lua_state = NULL;
	parent_script = p_parent;
};

ScriptInstanceLua::~ScriptInstanceLua() {

	clear();
};

/*************************************************************************************************/

Error ScriptLua::parse(const String& p_text) {

	return FAILED;	
};

Error ScriptLua::construct_instance(IRef<ScriptInstanceLua> p_instance) {

	ERR_FAIL_COND_V(!p_instance || !lua_state, FAILED);
	ERR_FAIL_COND_V( script_instance == LUA_NOREF, FAILED );

	LuaManager::retop rt(lua_state, lua_gettop(lua_state));
	
	lua_getglobal(lua_state, "construct_script");
	ERR_FAIL_COND_V(lua_isnil(lua_state, -1), FAILED);

	lua_getref(lua_state, script_instance);
	ERR_FAIL_COND_V(lua_isnil(lua_state, -1), FAILED);

	ERR_FAIL_COND_V(p_instance->push_instance() != OK, FAILED);

	ERR_FAIL_COND_V(lua_dbcall(lua_state, 2, 0), FAILED);
	//ERR_FAIL_COND_V(lua_isnil(lua_state, -1), FAILED);
	
	return OK;
};

IRef<ScriptInstance> ScriptLua::instance(IRef<IAPI> p_self) {

	ERR_FAIL_COND_V(!lua_state, IRef<ScriptInstance>());
	ERR_FAIL_COND_V( script_instance == LUA_NOREF, IRef<ScriptInstance>() );

	//ref_check rc(lua_state, p_self, "set");
	LuaManager::retop rt(lua_state, lua_gettop(lua_state));
	
	lua_getglobal(lua_state, "instance_script");
	ERR_FAIL_COND_V(lua_isnil(lua_state, -1), IRef<ScriptInstance>());
	
	lua_getref(lua_state, script_instance);
	ERR_FAIL_COND_V(lua_isnil(lua_state, -1), IRef<ScriptInstance>());

	tolua_pushiobj(lua_state, p_self);
	
	ERR_FAIL_COND_V(lua_dbcall(lua_state, 2, 1), IRef<ScriptInstance>());
	ERR_FAIL_COND_V(lua_isnil(lua_state, -1), IRef<ScriptInstance>());

	IRef<ScriptInstanceLua> ref = memnew(ScriptInstanceLua(this));
	
	int instance;
	if (lua_istable(lua_state, -1)) {

		instance = lua_ref(lua_state, 1);
	} else {
	
		lua_getfenv(lua_state, -1);
		instance = lua_ref(lua_state, 1);
	};

	ref->set_instance(lua_state, p_self, instance, script_file);

	return ref;
};


Error ScriptLua::load(const String& p_path) {

	ERR_FAIL_COND_V(!lua_state, ERR_BUG);

	clear();
	
	LuaManager::retop rt(lua_state, lua_gettop(lua_state));
	
	lua_getfield(lua_state, LUA_GLOBALSINDEX, "require");
	ERR_FAIL_COND_V(lua_isnil(lua_state, -1), ERR_BUG);
	
	lua_pushstring(lua_state, p_path.ascii().get_data());
	lua_pushboolean(lua_state, 1);

	ERR_FAIL_COND_V(lua_dbcall(lua_state, 2, 1) != 0, FAILED);
	
	ERR_FAIL_COND_V(lua_isnil(lua_state, -1), ERR_BUG);

	script_instance = lua_ref(lua_state, 1);
	ERR_FAIL_COND_V(script_instance == LUA_NOREF, FAILED);

	script_file = p_path;
	storage_mode = STORAGE_FILE;
	
	return OK;
};

String ScriptLua::get_file() const {
	
	return script_file;
};

void ScriptLua::set_storage_mode(Script::StorageMode p_mode) {

	storage_mode = p_mode;
};

Script::StorageMode ScriptLua::get_storage_mode() const {
	
	return storage_mode;
};

void ScriptLua::set_storage_file(String p_file) {

	// wut?
};	

void ScriptLua::clear() {
	
	if (lua_state && script_instance != LUA_NOREF) {
		
		lua_unref(lua_state, script_instance);
		script_instance = LUA_NOREF;
	};
};

void ScriptLua::set(String p_path, const Variant& p_value) {

	if (p_path == "storage_mode") {
		set_storage_mode((StorageMode)p_value.get_int());
	} else if (p_path == "code") {
		
		if (storage_mode == STORAGE_BUILT_IN) {
			parse(p_value.get_string());
		};
	} else if (p_path == "file") {

		if (storage_mode == STORAGE_FILE) {
			load(p_value.get_string());
		};
		script_file = p_value;
	};
};

String ScriptLua::get_script_code() {

	if (script_code == "" && script_file != "") {
		
		FileAccess* fa = FileAccess::create();
		if (fa->open(script_file, FileAccess::READ) != OK) {
			
			memdelete(fa);
			return "";
		};
		
		for (int i=0; i<fa->get_len(); i++) {
			
			script_code += String::chr(fa->get_8());
		};
		
		fa->close();
		memdelete(fa);
	};
	
	return script_code;
};

Variant ScriptLua::get(String p_path) const {

	if (p_path == "storage_mode") {
		
		return storage_mode;
		
	} else if (p_path == "code") {
		
		if (storage_mode != STORAGE_BUILT_IN) {
			return "";
		} else {
			return ((ScriptLua*)this)->get_script_code();
		};
		
	} else if (p_path == "file") {
		
		return script_file;
	};
	
	return Variant();
};

IAPI::PropertyHint ScriptLua::get_property_hint(String p_path) const {

	if (p_path == "storage_mode") {
	
		return IAPI::PropertyHint(IAPI::PropertyHint::HINT_ENUM, "File, Built in");
		
	} else if (p_path == "file") {
		
		return IAPI::PropertyHint(IAPI::PropertyHint::HINT_FILE, "*.lua");
	};
	
	return PropertyHint();
};

void ScriptLua::get_property_list( List<PropertyInfo> *p_list ) const {

	p_list->push_back( IAPI::PropertyInfo( Variant::INT, "storage_mode" ) );
	p_list->push_back( IAPI::PropertyInfo( Variant::STRING, "code" ) );
	p_list->push_back( IAPI::PropertyInfo( Variant::STRING, "file" ) );
};

ScriptLua::ScriptLua() {
	
	lua_state = LuaManager::get_singleton()->get_lua_state();
	script_instance = LUA_NOREF;
	storage_mode = STORAGE_FILE;
};

ScriptLua::~ScriptLua() {

	clear();
};

