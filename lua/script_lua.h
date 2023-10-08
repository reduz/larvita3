#ifndef LUA_SCRIPT_MANAGER_H
#define LUA_SCRIPT_MANAGER_H

#include "types/script/script.h"

#include "lua/include/lua.hpp"

class ScriptLua;

class ScriptInstanceLua : public ScriptInstance {

friend class ScriptLua;

	IAPI_TYPE( ScriptInstanceLua, ScriptInstance );

	lua_State* lua_state;
	String script_name;
	int instance;

	Error push_instance() const;
	WeakRef<ScriptLua> parent_script;

	WeakRef<IAPI> script_self;
	
public:

	void set_instance(lua_State* p_lua_state, WeakRef<IAPI> p_script_self, int p_instance, String p_script_name);

	virtual void set(String p_path, const Variant& p_value); /** set a property, by path */
	virtual Variant get(String p_path) const; /** get a property, by path */
	virtual void get_property_list( List<PropertyInfo> *p_list ) const; ///< Get a list with all the properies inside this object
	virtual PropertyHint get_property_hint(String p_path) const; ///< get a property hint
	virtual Variant call(String p_method, const List<Variant>& p_params);
	virtual void get_method_list( List<MethodInfo> *p_list ) const; ///< Get a list with all the properies inside this object

	Error construct();
	
	void clear();

	ScriptInstanceLua(WeakRef<ScriptLua> p_parent);
	~ScriptInstanceLua();
};



class ScriptLua : public Script {

	IAPI_TYPE( ScriptLua, Script );
	IAPI_INSTANCE_FUNC( ScriptLua );

private:
	
	lua_State* lua_state;
	int script_instance;
	String script_file;
	String script_code;
	Script::StorageMode storage_mode;

	String get_script_code();

public:

	virtual void set(String p_path, const Variant& p_value); /** set a property, by path */
	virtual Variant get(String p_path) const; /** get a property, by path */
	virtual void get_property_list( List<PropertyInfo> *p_list ) const; ///< Get a list with all the properies inside this object
	virtual IAPI::PropertyHint get_property_hint(String p_path) const;

	Error construct_instance(IRef<ScriptInstanceLua> p_instance);

	IRef<ScriptInstance> instance(IRef<IAPI> p_self=IRef<IAPI>());

	Error parse(const String& p_text);
	Error load(const String& p_path);

	String get_file() const;

	void set_storage_mode(StorageMode p_mode);
	StorageMode get_storage_mode() const;
	void set_storage_file(String p_file);	

	void clear();
	
	ScriptLua();
	~ScriptLua();
};

#endif
