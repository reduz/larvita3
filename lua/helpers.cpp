#include "helpers.h"

#include "lua.hpp"
#include "hashfuncs.h"

#include "lua_manager.h"

#include <stdlib.h>
#include <string.h>

void tolua_pushiobj_noref(lua_State* L, void* p_var, const char* p_type) {

	IObjRef ref((IObj*)p_var);
	tolua_pushiobj(L, ref, p_type);
};

void tolua_pushiobj(lua_State* L, const IObjRef& var, const char* p_iapi_type, bool skip_iref) {

	if (!var) {
		lua_pushnil(L);
		return;
	};

	IObj* ptr = (IObj*)var.operator->();

	do {
	
		if (strncmp(p_iapi_type, "Lua__", 5) == 0) {

			tolua_pushusertype(L, ptr, p_iapi_type);
			break;
		}

		bool found;
		
		// try iapi type
		String iapi_type = var->get_type();
		lua_getfield(L, LUA_REGISTRYINDEX, iapi_type.ascii().get_data());
		found = !lua_isnil(L, -1);
		lua_pop(L, 1);
		if (found) {
			tolua_pushusertype(L, ptr, iapi_type.ascii().get_data());
			break;
		};
		
		// try p_iapi_type
		if (*p_iapi_type) {
			lua_getfield(L, LUA_REGISTRYINDEX, p_iapi_type);
			found = !lua_isnil(L, -1);
			lua_pop(L, -1);
			if (found) {
				tolua_pushusertype(L, ptr, p_iapi_type);
				break;
			};
		};
		
		// try persist_as
		iapi_type  = var->persist_as();
		lua_getfield(L, LUA_REGISTRYINDEX, iapi_type.ascii().get_data());
		found = !lua_isnil(L, -1);
		lua_pop(L, 1);
		if (found) {
			tolua_pushusertype(L, ptr, iapi_type.ascii().get_data());
			break;
		};
		
		// fallback
		tolua_pushusertype(L, ptr, "IObj");
	} while (0);

	if (!skip_iref) {
		int top = lua_gettop(L);
		
		lua_getfield(L, LUA_REGISTRYINDEX, "tolua_irefbox");
		lua_pushvalue(L, -2);
		lua_gettable(L, -2);
		
		if (lua_isnil(L, -1)) {
	
			lua_pop(L, 1);
			lua_pushvalue(L, -2);
		
			IRef<IObj>* obj = memnew(IRef<IObj>(var));
			tolua_pushusertype_and_takeownership(L, obj, "IRef<IObj>");
			lua_settable(L, -3);
		};
		
		lua_settop(L, top);
	};
};

int tolua_isiobj(lua_State* L, int lo, int def, tolua_Error* err) {

	LuaManager::retop rt(L);
	
	if (def && lua_gettop(L)<abs(lo))
		return 1;

	if (lua_isnil(L, lo)) {
		return 1;
	};
	
	if (tolua_isusertype(L, lo, "IObj", def, err) || tolua_isusertype(L, lo, "IRef<IObj>", def, err)) {
		
		return 1;
	};
	
	return 0;
};

IObjRef tolua_toiobj(lua_State* L, int lo, void* def) {

	LuaManager::retop rt(L);
	
	if (lua_gettop(L) < abs(lo)) {
		
		return IObjRef((IObj*)def);
	};

	if (lua_isnil(L, lo)) {
		return IObjRef();
	};
	
	tolua_Error err;
	if (tolua_isusertype(L, lo, "IRef<IObj>", def?1:0, &err)) {
		
		return IObjRef(*(IObjRef*)tolua_tousertype(L, lo, 0));
	};

	if (!tolua_isusertype(L, lo, "IObj", def?1:0, &err)) {
		return IObjRef();
	};
	
	IObj* obj = (IObj*)tolua_tousertype(L, lo, 0);

	return obj;
};

void tolua_pushVariant(lua_State* L, const Variant& var) {
	
	Variant::Type type = var.get_type();
	switch (type) {
		
	case Variant::NIL:
		lua_pushnil(L);
		break;

	#define PUSH_VALUE(TYPE) \
	{ \
		TYPE* v = memnew(TYPE(var.operator TYPE())); \
		tolua_pushusertype_and_takeownership(L, v, #TYPE); \
	}; break;
		
		/* Basic, built in types */
	case Variant::BOOL:
		lua_pushboolean(L, var.get_bool());
		break;
		
	case Variant::INT:
		lua_pushnumber(L, var.get_int());
		break;
		
	case Variant::REAL:
		lua_pushnumber(L, var.get_real());
		break;
		
	case Variant::STRING:
		lua_pushstring(L, var.get_string().utf8().get_data());
		break;

	case Variant::VECTOR3:

		PUSH_VALUE(Vector3);
		
	case Variant::MATRIX4:

		PUSH_VALUE(Matrix4);
		
	case Variant::PLANE:
		
		PUSH_VALUE(Plane);
		
	case Variant::RECT3:
		PUSH_VALUE(Rect3);
		
	case Variant::QUAT:
		
		PUSH_VALUE(Quat);
		
	case Variant::COLOR:
		
		PUSH_VALUE(Color);
		
	case Variant::IMAGE:
		
		PUSH_VALUE(Image);
		
	case Variant::IOBJ:
		tolua_pushiobj(L, var.get_IOBJ(), "");
		break;
		
	case Variant::INT_ARRAY:
	case Variant::REAL_ARRAY:
	case Variant::STRING_ARRAY:
	case Variant::BYTE_ARRAY:
	case Variant::VARIANT_ARRAY:
	default:
		Variant* variant = memnew(Variant(var));
		tolua_pushusertype_and_takeownership(L, variant, "Variant");
		break;
	};
	
};

Variant tolua_toVariant(lua_State* L, int lo, void* def) {

	LuaManager::retop rt(L);
	
	if (lua_gettop(L) < abs(lo)) {
		
		return Variant();
	};
	
	int type = lua_type(L, lo);

	#define CONVERT_UDATA(TYPE) \
		static const Uint32 TYPE##_hash = hash_djb2(#TYPE);\
		if (var_hash == TYPE##_hash && var_type == #TYPE) { \
			return Variant(*((TYPE*)tolua_tousertype(L, lo, def)));\
		};
		
	
	switch (type) {
		
	case LUA_TNIL:
		return Variant();

	case LUA_TSTRING:
		return Variant(String::utf8(tolua_tostring(L, lo, 0)));
		
	case LUA_TBOOLEAN:
		return Variant(tolua_toboolean(L, lo, 0)?true:false);
		
	case LUA_TNUMBER:
		return Variant(tolua_tonumber(L, lo, 0));
		
	case LUA_TUSERDATA: {
		
		String var_type = tolua_typename(L, lo);
		Uint32 var_hash = hash_djb2(var_type.ascii().get_data());

		CONVERT_UDATA(Variant);
		CONVERT_UDATA(Vector3);
		CONVERT_UDATA(Matrix4);
		CONVERT_UDATA(Plane);
		CONVERT_UDATA(Rect3);
		CONVERT_UDATA(Quat);
		CONVERT_UDATA(Color);
		CONVERT_UDATA(Image);
		

		tolua_Error err;
		if (tolua_isiobj(L, lo, def?1:0, &err)) {
			
			return Variant(tolua_toiobj(L, lo, def));
		};
	}; break;

	case LUA_TTABLE:
		Variant ret;
		int len = lua_objlen(L, lo);
		ret.create(Variant::VARIANT_ARRAY, len);
		int vartop = lua_gettop(L) + 1;

		for (int i=0; i<len; i++) {
			
			lua_rawgeti(L, lo, i+1);
			Variant val = tolua_toVariant(L, vartop, 0);
			ret.array_set(i, val);
			lua_pop(L, 1);
		};
		
		return ret;

	};
	
	return Variant();
	
};

int tolua_isVariant(lua_State* L, int lo, int def, tolua_Error* err) {

	LuaManager::retop rt(L);
	
	if (def && lua_gettop(L)<abs(lo))
		return 1;
	
	int type = lua_type(L, lo);
	
	switch (type) {
		
	case LUA_TNIL:
	case LUA_TSTRING:
	case LUA_TBOOLEAN:
	case LUA_TNUMBER:
		return 1;
		
	case LUA_TUSERDATA:
		if (tolua_isusertype(L, lo, "Variant", def, err)) {
			
			return 1;
		} else if (tolua_isiobj(L, lo, def, err)) {
			
			return 1;
		} else {
			
			int top = lua_gettop(L);
			String type = String(".") + tolua_typename(L, lo);
			lua_getfield(L, LUA_REGISTRYINDEX, "Variant");
			lua_getfield(L, -1, type.ascii().get_data());
			if (lua_isnil(L, -1)) {
				lua_settop(L, top);
				goto error;
			} else {
				lua_settop(L, top);
				return 1;
			};
		};
		break;

	case LUA_TTABLE:
		
		return 1;
	default:
		goto error;
		
	};

	error:
	
	err->index = lo;
	err->array = 0;
	err->type = "Variant";
	
	return 0;
};


int lua_dbcall(lua_State* L, int p, int r, String f) {

	int errfunc = lua_gettop(L) - p;

	lua_getglobal(L,f.ascii());
	lua_insert(L,(errfunc));
	if (lua_isnil(L, errfunc)) {
		errfunc = 0;
	};
	int ret = lua_pcall(L,p,r,errfunc);
	if (ret) {
		return ret;
	} else {
		if (errfunc)
			lua_remove(L, errfunc);
	};
	return ret;
};

