#include "lua_slot.h"

void LuaSlot::init(lua_State* p_ls, lua_Object p_obj) {

	unreference();

	refcount = memnew(int);
	*refcount = 1;
	ls = p_ls;
	lua_pushvalue(ls, p_obj);
	slot = lua_ref(ls, 1);
};

void LuaSlot::unreference() {

	if (!ls) return;
	--(*refcount);

	if (*refcount == 0) {
		lua_unref(ls, slot);
		ls = NULL;
		memdelete(refcount);
	};
	ls = NULL;
	refcount = NULL;
};

void LuaSlot::reference(const LuaSlot& p_r) {
	
	if (ls) unreference();
	
	if (!p_r.ls) return;
	refcount = p_r.refcount;
	ls = p_r.ls;
	slot = p_r.slot;
	
	++(*refcount);
};

bool LuaSlot::is_empty() {
	return ls != NULL;
};

void LuaSlot::push_call() {

	lua_getref(ls, slot); // that's all
};


LuaSlot& LuaSlot::operator=(const LuaSlot& p_r) {
	
	reference(p_r);
	return *this;
};

LuaSlot::LuaSlot(const LuaSlot& p_r) {

	ls = NULL;
	reference(p_r);
};

LuaSlot::LuaSlot(lua_State* p_ls, lua_Object p_obj) {
	ls = NULL;
	init(p_ls, p_obj);
};

LuaSlot::LuaSlot() {

	ls = NULL;
};

LuaSlot::~LuaSlot() {

	unreference();
};
