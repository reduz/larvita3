function print_traceback(e)

	if coroutine.running() then
		print(debug.traceback(coroutine.running(), e))
	else
		print(debug.traceback(e))
	end
end

function ERROR(msg)

	print(debug.traceback("ERROR: "..tostring(msg)))
end


------------------------ loading ------------------------

_global_scripts = {}
setmetatable(_global_scripts, { __mode = "v" } ) -- weak values metatable

local global_mt = { __index = _G }

function require(fname, new_env, force)

	if not string.match(fname, "^/") and not string.match(fname, "^.:") and not string.match(fname, "^res://") then
	
		fname = "res://"..fname
	end

	if _global_scripts[fname] and not force then

		return _global_scripts[fname]
	end

	
	local mode = LuaManager:get_singleton():get_load_mode()

	if mode == LuaManager.MODE_FILESYSTEM or mode == LuaManager.MODE_FILESYSTEM_DATABASE then

		local err,f = LuaManager:get_singleton():load_file(fname, "")

		if err == OK then
			if new_env then
				local env = {}
				setmetatable(env, global_mt)
				setfenv(f, env)
			end
			local ret = f()
			_global_scripts[fname] = ret or true
			return ret
		end
	end
		
	if mode == LuaManager.MODE_DATABASE or mode == LuaManager.MODE_FILESYSTEM_DATABASE then

		local path = string.gsub(fname, "res:/", "")
		local id = IAPI_Persist:get_singleton():get_db():get_ID(path)
		if id ~= OBJ_INVALID_ID then

			local data = IAPI_Persist:get_singleton():load(id)

			local err,f = LuaManager:get_singleton():load_buffer(data:get("script_code"):get_byte_array(), fname, "")
			if err == OK then
				if new_env then
					local env = {}
					setmetatable(env, global_mt)
					setfenv(f, env)
				end
				local ret = f()
				_global_scripts[fname] = ret or true
				return ret
			end

			return nil, t
		else
		
			print("script "..path.." not found in database")
		end
		
		return nil
	else
		print("file "..fname.." not found")
	end
end

function instance_script(class, self)

	local instance = class:__alloc_instance(self)
	
	return instance
end

function construct_script(class, self)

	class:__construct_instance(self)
end

----------------------- signals -----------------------
_global_signal_handler = LuaSignalHandler:create()
--tolua.takeownership(_global_signal_handler)

function global_connect_signal(signal, instance, method, ...)

	if not signal then

		return -1
	end
	
	if not method then

		return -1
	end
	
	local sg = instance._signal_handler or _global_signal_handler

	if not sg then

		return -1
	end
	
	local con = sg:connect_signal(signal, tolua.type(signal), LuaSlot(instance, method, ...))
	return con
end

-- add it to signals
function add_connect(reg)
	for k,v in pairs(reg) do
		if type(v) == 'table' and type(k) == 'string' and string.find(k, "^Signal<") then
			rawset(v, "connect", global_connect_signal)
			rawset(v, "disconnect", global_disconnect_signal)
		end
	end
end

local function get_arg(...)

	return {n=select("#", ...), ...}
end

local function join_tables(t1, t2)

	-- first table needs to have 'n'
	for i=1,t2.n do
		t1[t1.n+i] = t2[i]
	end
	t1.n = t1.n + t2.n
end

LuaSlot = {}
LuaSlot.__index = LuaSlot

function LuaSlot:__call(...)

	local arg = get_arg(...)

	if self.bind.n > 0 then
		join_tables(arg, self.bind)
	end

	if self.static then
		if not self.method then
			return nil
		end
		return self.method(unpack(arg, 1, arg.n))
	else
		local inst = self.instance.p
		local m = self.instance.m
		if type(m) == "string" then
			m = inst[m]
		end
			
		if inst then
			return m(inst, unpack(arg, 1, arg.n))
		else
			if self.connection then
				self.connection:disconnect()
				return nil
			end
		end
	end
end

function LuaSlot:set_connection_collector(col)

	if self.static then
		return
	end

	local client = self.instance.p
	if not client then return end

	self.collector = {[client] = col}
	setmetatable(self.collector, {__mode = 'k'})
end

LuaSlot.call = LuaSlot.__call
weak_val_mt = {__mode = 'v'}
weak_key_mt = {__mode = 'k'}

function LuaSlot:new(...)

	local i = {}
	setmetatable(i, LuaSlot)
	i:__init__(...)
	
	return i
end

function LuaSlot:__init__(rcv_instance, method, ...)

	self.bind = get_arg(...)
	if rcv_instance then
		self.instance = setmetatable({}, {__mode = 'v'})
		self.instance.p = rcv_instance
		self.instance.m = method
	else
		self.method = method
		self.static = true
	end
	self.coro = coroutine.running()
end

setmetatable(LuaSlot, {__call = LuaSlot.new})

LuaSignal = {}
LuaSignal.__index = LuaSignal

function LuaSignal:connect(instance, method, ...)

	table.insert(self.slot_list, LuaSlot(instance, method, ...))
end

function LuaSignal:__call(...)

	for k,v in ipairs(self.slot_list) do
		v(...)
	end
end

function LuaSignal:emit(...)

	self(...)
end

function LuaSignal:new(...)

	local i = {}
	setmetatable(i, LuaSignal)
	i:__init__(...)
	
	return i
end

function LuaSignal:__init__()

	self.slot_list = {}
	
end

setmetatable(LuaSignal, {__call = LuaSignal.new} )

---------------------------------------- gui ----------------------------------------

local function tcopy(t)

	local r = {}
	for k,v in pairs(t) do
		r[k] = v
	end
	
	return r
end

local gui_reserved = { type = true, add_args = true, init = true, add = true, name = true, }




function load_gui(root, tree, parent)

	if type(tree) == 'string' then
	
		tree = { type = {GUI.Label, tree} }
	end

	local wtype = tree.type

	local obj
	if wtype then
		local args = {}
		if wtype[1] then
			for i=2,#wtype do
				args[i-1] = wtype[i]
			end
			wtype = wtype[1]
		end

		obj = wtype:new(unpack(args))

		if parent then
			local add_args = tcopy(tree.add_args or {})
			local pos = add_args.self_pos or 1
			table.insert(add_args, pos, obj)

			if type(tree.add) == "function" then
				tree.add(obj, parent, root, tree)
			else
				parent:add(unpack(add_args))
			end
		end

		if tree.name then
			if root[tree.name] then
				if type(root[tree.name]) ~= 'table' then
					root[tree.name] = {root[tree.name]}
				end
				table.insert(root[tree.name], obj)
			else
				root[tree.name] = obj
			end
		end
	else
	
		obj = parent
	end

	for k,v in pairs(tree) do
	
		if type(k) == 'string' and not gui_reserved[k] then

			obj[k] = v
		end
	end

	for k,v in ipairs(tree) do
	
		load_gui(root, v, obj)
	end
	
	if type(tree.init) == 'function' then
	
		tree.init(obj, parent, root, tree)
	end
	
	return obj
end


local function global_add_frame(self, frame, ...)

	if self.add_frame then
		self:add_frame(frame, ...)
	else
		self:set_frame(frame, ...)
	end
	
	return frame
end

rawset(GUI.Container, "add", global_add_frame)

old_tostring = tostring
tostring = variant_tostring

