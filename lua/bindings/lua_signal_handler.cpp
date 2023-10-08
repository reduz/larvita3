#include "lua_signal_handler.h"

LuaSignalHandler*(*LuaSignalHandler::create)() = 0;

void LuaSignalHandler::clear_connections() {
	
	SignalTarget::clear_connections();
};


int LuaSignalHandler::connect_signal(lua_State* p_lua_state, void *p_signal, String p_signal_type, lua_Object p_slot) {

	LuaSlot slot(p_lua_state, p_slot);

	return connect_signal_internal(p_signal, p_signal_type, slot);
};

LuaSignalHandler::~LuaSignalHandler() {

};
