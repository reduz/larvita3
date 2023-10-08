#ifndef LUA_SIGNAL_HANDLER_H
#define LUA_SIGNAL_HANDLER_H

#include "types/signals/signals.h"
#include "lua_slot.h"
#include "types/signals/connection.h"

class LuaSignalHandler : public SignalTarget {

protected:
	
	virtual int connect_signal_internal(void* p_signal, String p_signal_name, LuaSlot &slot)=0;

public:

	void clear_connections();

	int connect_signal(lua_State* p_lua_state, void *p_signal, String p_signal_type, lua_Object p_slot);

	virtual ~LuaSignalHandler();

	static tolua_owned LuaSignalHandler*(*create)();
};

#endif

