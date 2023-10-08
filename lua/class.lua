local class = class or {} -- the module

class._global_objects = {}

local BaseClass = class.BaseClass or {[".classname"] = "BaseClass"}
BaseClass.__index = BaseClass

local function get_arg(...)

	return {n=select("#", ...), ...}
end

function class.create(self, obj, base)

	local o = _G[obj] or {}
	o['.classname'] = obj
	o.__index = o
	if type(base) == 'string' then
		base = class._global_objects[base]
	end

	if string.find(tolua.type(base), "^class ") then
		o['.tolua_base'] = base
		base = nil
		o.__alloc__ = class.alloc_tolua_base
	end

	base = base or BaseClass

	setmetatable(o, base)

	_G[obj] = o
	class._global_objects[obj] = o
end

function class.alloc_tolua_base(self, ...)
	return self['.tolua_base']:new(...)
end

function class.instance_from_script(p_script, page)
	local obj = require(p_script)
	local n = obj:new(page)

	return n
end

function BaseClass.new(self, ...)

	return BaseClass.new_instance(self, nil, ...)
end

function BaseClass.new_instance(self, instance, ...)

	local o = self.__alloc_instance(self, instance)
	
	return self.__construct_instance(self, o, ...)
end

function BaseClass.__alloc_instance(self, instance)
	
	local o, t

	t = {}
	o = instance or t
	if instance and type(instance) == "userdata" then
		tolua.setpeer(o, t)
	end
	if not o._signal_handler then
		t._signal_handler = LuaSignalHandler:create()
	end

	setmetatable(t, self)
	
	return o
end

function BaseClass.__construct_instance(self, o, ...)
	
	local arg = get_arg(...)
	o = class.init_object(self, o, arg) or o
	--o:__init__(unpack(arg))

	return o
end

function class.base__index(self, key)

	local v = rawget(self, key)
	if v ~= nil then
		return v
	end
	v = getmetatable(self)[key]
	if v ~= nil then
		return v
	end
	v = rawget(self, ".c_instance")
	if v ~= nil then
		return v[key]
	end
end

function BaseClass:set_c_instance(ins)

	if ins.tolua__set_instance then

		ins:tolua__set_instance(self)
	end

	--self[".c_instance"] = ins
	tolua.inherit(self, ins)

	getmetatable(self).__index = class.base__index
end

function class.init_object(mt, obj, arg)

	local parg = arg
	local pa = rawget(mt, "__parent_args__")
	if pa then
		parg = get_arg( pa(obj, unpack(arg, 1, arg.n)) )
	end

	local ip = rawget(mt, "__init_parent__")
	if ip then
		ip(obj, parg, mt)
	else
		ip = rawget(BaseClass, "__init_parent__")
		obj = ip(obj, parg, mt) or obj
	end

	local init = rawget(mt, "__init__")

	if init then
		init(obj, unpack(arg, 1, arg.n))
	end

	return obj
end

function class.finalize(obj)

	local t = obj
	if type(t) == "userdata" then
		t = tolua.getpeer(obj)
	end

	if not t then
		return
	end

	local mt = getmetatable(t)
	while mt do
	
		local f = rawget(mt, "__finalize__")
		if f then
			f(obj)
		end
		mt = getmetatable(mt)
	end
end

function BaseClass:__init_parent__(arg, mt)

	local t = getmetatable(mt)
	if t then
		if t == BaseClass then
			if self.__alloc__ and type(self) == 'table' then
				local o = self:__alloc__(unpack(arg, 1, arg.n))
				if type(o) ~= 'table' then
					tolua.setpeer(o, self)
					if o.tolua__set_instance then
						o:tolua__set_instance(o)
					end
				end
				self = o
			end
		else
			self = class.init_object(t, self, arg)
		end
	end
	return self
end

function BaseClass:__finalize__()

	if self._signal_handler then
		self._signal_handler:clear_connections()
	end
end

function BaseClass:__init__()

end

class.BaseClass = BaseClass
setmetatable(class, {__call = class.create})

_G.class = class

return class
