dofile "lua/bindings/signal_hooks.lua"
--dofile "lua/bindings/function_hooks.lua"

-- support for QString
_basic['String'] = 'rstring'
_basic_ctype.rstring = 'const char*'

_basic['Sint64'] = 'number'
_basic_ctype.Sint64 = 'Sint64'

_basic['Uint64'] = 'number'
_basic_ctype.Uint64 = 'Uint64'

_base_push_functions['IObj'] = 'tolua_pushiobj_noref'
--_base_to_functions['IRefBase'] = 'tolua_toiobj'
--_base_is_functions['IRefBase'] = 'tolua_isiobj'

_basic['Variant'] = 'Variant'
_basic_ctype.Variant = 'Variant'

_basic['IRefBase'] = 'iobj'
_basic_ctype.iobj = 'IObjRef'

-- flags
local disable_virtual_hooks = false
local enable_pure_virtual = true
local default_private_access = true

local access = {public = 0, protected = 1, private = 2}

function extract_code(fn,s)
	local code = ""
	if fn then
		code = '\n$#include "'..fn..'"\n'
	end
	s= "\n" .. s .. "\n" -- add blank lines as sentinels
	local _,e,c,t = strfind(s, "\n([^\n]-)SCRIPT_([%w_]*)[^\n]*\n")
	while e do
		t = strlower(t)
		if t == "bind_begin" then
			_,e,c = strfind(s,"(.-)\n[^\n]*SCRIPT_BIND_END[^\n]*\n",e)
			if not e then
			 tolua_error("Unbalanced 'SCRIPT_BIND_BEGIN' directive in header file")
			end
		end
		if t == "bind_class" or t == "bind_block" then
			local b
			_,e,c,b = string.find(s, "([^{]-)(%b{})", e)
			c = c..'{\n'..extract_code(nil, b)..'\n};\n'
		end
		code = code .. c .. "\n"
	 _,e,c,t = strfind(s, "\n([^\n]-)SCRIPT_([%w_]*)[^\n]*\n",e)
	end
	return code
end

function preparse_hook(p)

	if default_private_access then
		-- we need to make all structs 'public' by default
		p.code = string.gsub(p.code, "(struct[^;{\n]*{)", "%1\npublic:\n")
	end
	-- eliminate anonymous unions
	local function strip(t)
	
		return string.sub(t, 2, -2)
	end
	p.code = string.gsub(p.code, "union%s+(%b{})%s*;", strip)

	local defs = ""
	for r,t in string.gmatch(p.code, "(IRef(%b<>))") do
		if not _basic[r] then
			local t = string.sub(t, 2, -2)
			local nt = string.gsub(t, "[:<>]", "_")
			defs = defs .. "\5#define tolua_pushiobj__"..nt..'(l,v)\ttolua_pushiobj(l,v,"'..t..'")\6\n'
			defs = defs .. "\5#define tolua_toiobj__"..nt.."\ttolua_toiobj\6\n"
			defs = defs .. "\5#define tolua_isiobj__"..nt.."\ttolua_isiobj\6\n"
			_basic[r] = 'iobj__'..nt
			_basic_ctype['iobj__'..nt] = r
		end
	end
	
	p.code = string.gsub(p.code, "([^%w_%d])_FORCE_INLINE_([^%w_%d])", "%1%2")
	
	p.code = defs .. p.code
	--print( "************************************ code is\n"..p.code)
end

function pre_output_hook(p)

end

include_hook_list = {}

function include_hook_list.tmpl(p, filename, type, ...)

	local sep = ""
	local list = ""
	for k,v in ipairs(arg) do
		list = list..sep..v
		sep = ","
	end

	p.code = string.gsub(p.code, "%$TYPE_LIST", list)
end

function include_file_hook(p, filename, type, ...)

	if include_hook_list[type] then
		return include_hook_list[type](p, filename, type, unpack(arg))
	end

	p.code = '$#include \"'..filename..'\"\n'..p.code
	-- eliminate defines
	p.code = string.gsub(p.code, "#define[^\n]*\n", "\n")
	--p.code = add_signals(p.code, filename)
--	p.code = string.gsub(p.code, "friend%s+class[^;]*;", "")
end

function parser_hook(s)

	local container = classContainer.curr -- get the current container

	if default_private_access then
		if not container.curr_member_access and container.classtype == 'class' then
			-- default access for classes is private
			container.curr_member_access = access.private
		end
	end

	-- signal
	do
		local b,e,t = string.find(s, "^%s*(Signal[^<]?%s*%b<>)[^;]*;")
		if b then
			--add_signal(t)
			s = string.gsub(s, "^%s*", "tolua_readonly ")
			return s
		end
	end
	
	-- template function
	do
		local b,e,name = string.find(s, "^%s*template%s*%b<>%s*([%w_]+)[^;}]*%b{}")
		if b and (name ~= "class" and name ~= "struct") then
			return strsub(s, e+1)
		end
	end

	-- template
	do
		local b,e = string.find(s, "^%s*template%s*%b<>")
		if b then
			return strsub(s, e+1)
		end
	end

	-- try macros
	do
		local b,e,mac = string.find(s, "^%s*([%w_%d]*)")
		if b and _extra_parameters[mac] then
			local rep = _extra_parameters[mac]
			if type(rep) == 'boolean' then
				rep = ""
			end
			return rep..strsub(s, e+1)
		end
	end

	-- try TOLUA_NO_CONSTRUCTOR
	do
	local b,e = string.find(s, "^%s*TOLUA_NO_CONSTRUCTOR")
		if b then

			local i=1
			while container[i] do
			
				if container[i].name == 'new' then
					container[i].global_access = false
				end
				i = i+1
			end
		
			return strsub(s, e+1)
		end
	end

	-- try labels (public, private, etc)
	do
		local b,e,label = string.find(s, "^%s*(%w*)%s*:[^:]") -- we need to check for [^:], otherwise it would match 'namespace::type'
		if b then

			-- found a label, get the new access value from the global 'access' table
			if access[label] then
				container.curr_member_access = access[label]
			end -- else ?
			return strsub(s, e) -- normally we would use 'e+1', but we need to preserve the [^:]
		end
	end


	local ret = nil

	if disable_virtual_hooks then

		return ret
	end

	local b,e,decl,arg = string.find(s, "^%s*virtual%s+([^%({~]+)(%b())")
	local const
	if b then
		local ret = string.sub(s, e+1)
		if string.find(ret, "^%s*const") then
			const = "const"
			ret = string.gsub(ret, "^%s*const", "")
		end
		local purev = false
		if string.find(ret, "^%s*=%s*0") then
			purev = true
			ret = string.gsub(ret, "^%s*=%s*0", "")
		end
		ret = string.gsub(ret, "^%s*%b{}", "")

		local func = Function(decl, arg, const)
		func.pure_virtual = purev
		--func.access = access
		func.original_sig = decl

		local curflags = classContainer.curr.flags
		if not curflags.virtual_class then

			curflags.virtual_class = VirtualClass()
		end
		curflags.virtual_class:add(func)
		curflags.pure_virtual = curflags.pure_virtual or purev

		return ret
	end

	return ret
end

function pre_register_hook(p)
--print("post output")
	add_signals(p)

	output_signal_handlers(p, _OUTPUT)
end




-- class VirtualClass
classVirtualClass = {
 classtype = 'class',
 name = '',
 base = '',
 type = '',
 btype = '',
 ctype = '',
}
classVirtualClass.__index = classVirtualClass
setmetatable(classVirtualClass,classClass)

function classVirtualClass:add(f)

	local parent = classContainer.curr
	pop()

	table.insert(self.methods, {f=f})

	local name,sig

	-- doble negative means positive
	if f.name == 'new' and ((not self.flags.parent_object.flags.pure_virtual) or (enable_pure_virtual)) then

		name = self.original_name
	elseif f.name == 'delete' then
		name = '~'..self.original_name
	else
		if f.access ~= 2 and (not f.pure_virtual) and f.name ~= 'new' and f.name ~= 'delete' then
			name = f.mod.." "..f.type..f.ptr.." "..self.flags.parent_object.lname.."__"..f.name
		end
	end

	if name then
		sig = name..self:get_arg_list(f, true)..";\n"
		push(self)
		sig = preprocess(sig)
		self:parse(sig)
		pop()
	end

	push(parent)
end

function preprocess(sig)

	sig = gsub(sig,"([^%w_])void%s*%*","%1_userdata ") -- substitute 'void*'
	sig = gsub(sig,"([^%w_])void%s*%*","%1_userdata ") -- substitute 'void*'
	sig = gsub(sig,"([^%w_])char%s*%*","%1_cstring ")  -- substitute 'char*'
	sig = gsub(sig,"([^%w_])lua_State%s*%*","%1_lstate ")  -- substitute 'lua_State*'

	return sig
end

function classVirtualClass:get_arg_list(f, decl)

	local ret = ""
	local sep = ""
	local i=1
	while f.args[i] do

		local arg = f.args[i]
		if decl then
			local ptr
			if arg.ret ~= '' then
				ptr = arg.ret
			else
				ptr = arg.ptr
			end
			local def = ""
			if arg.def and arg.def ~= "" then

				def = " = "..arg.def
			end
			ret = ret..sep..arg.mod.." "..arg.type..ptr.." "..arg.name..def
		else
			ret = ret..sep..arg.name
		end

		sep = ","
		i = i+1
	end

	return "("..ret..")"
end

function classVirtualClass:output_iapi()

	parent = self.flags.parent_object
	if not parent then return end
	
	while parent do
	
		if parent.type == "IAPI" then
			--output('IAPI_TYPE("', self.original_name, '", "', self.btype, '");')
			return
		end
		parent = _global_classes[parent.btype]
	end

end


function classVirtualClass:add_parent_virtual_methods(parent)

	parent = parent or _global_classes[self.flags.parent_object.btype]

	if not parent then return end

	if parent.flags.virtual_class then

		local vclass = parent.flags.virtual_class
		for k,v in ipairs(vclass.methods) do
			if v.f.name ~= 'new' and v.f.name ~= 'delete' and (not self:has_method(v.f)) then
				table.insert(self.methods, {f=v.f})
			end
		end
	end

	parent = _global_classes[parent.btype]
	if parent then
		self:add_parent_virtual_methods(parent)
	end
end

function classVirtualClass:has_method(f)

	for k,v in pairs(self.methods) do
		-- just match name for now
		if v.f.name == f.name then
			return true
		end
	end

	-- looking for methods that are private and can't be called from the implementation
	local p = self.flags.parent_object
	while p do

		local i = 1

		while p[i] do
			if p[i].name and p[i].name == f.name then
				if (p[i].access and p[i].access ~= 0) then
					return true
				else
					return false
				end
			end
		
			i = i+1
		end
		p = _global_classes[p.btype]
	end
	
	return false
end

function classVirtualClass:add_constructors()

	local i=1
	while self.flags.parent_object[i] do

		local v = self.flags.parent_object[i]
		if getmetatable(v) == classFunction and (v.name == 'new' or v.name == 'delete') and (not v.access or v.access == 0) then

			self:add(v)
		end

		i = i+1
	end

end

--[[
function classVirtualClass:requirecollection(t)

	self:add_constructors()
	local req = classClass.requirecollection(self, t)
	if req then
		output('class ',self.name,";")
	end
	return req
end
--]]

function classVirtualClass:supcode()

	-- pure virtual classes can have no default constructors on gcc 4

	if self.flags.parent_object.flags.pure_virtual and not enable_pure_virtual then
		output('#if (__GNUC__ == 4) || (__GNUC__ > 4 ) // I hope this works on Microsoft Visual studio .net server 2003 XP Compiler\n')
	end

	local ns
	if self.prox.classtype == 'namespace' then
		output('namespace ',self.prox.name, " {")
		ns = true
	end

	output("class "..self.original_name.." : public "..self.btype..", public ToluaBase {")

	self:output_iapi() 
		
	output("public:\n")

	self:add_parent_virtual_methods()

	self:output_methods(self.btype)
	self:output_parent_method_wrappers()

	self:add_constructors()

	-- no constructor for pure virtual classes
	if (not self.flags.parent_object.flags.pure_virtual) or enable_pure_virtual then

		self:output_constructors()
	end

	output("};\n\n")

	if ns then
		output("};")
	end

	classClass.supcode(self)

	if self.flags.parent_object.flags.pure_virtual and not enable_pure_virtual then
		output('#endif // __GNUC__ >= 4\n')
	end

	-- output collector for custom class if required
	if self:requirecollection(_collect) and _collect[self.type] then

		output('\n')
		output('/* function to release collected object via destructor */')
		output('#ifdef __cplusplus\n')
		--for i,v in pairs(collect) do
		i,v = self.type, _collect[self.type]
		 output('\nstatic int '..v..' (lua_State* tolua_S)')
			output('{')
			output(' '..i..'* self = ('..i..'*) tolua_tousertype(tolua_S,1,0);')
			output('	delete self;')
			output('	return 0;')
			output('}')
		--end
		output('#endif\n\n')
	end

end

function classVirtualClass:register(pre)

	-- pure virtual classes can have no default constructors on gcc 4
	if self.flags.parent_object.flags.pure_virtual and not enable_pure_virtual then
		output('#if (__GNUC__ == 4) || (__GNUC__ > 4 )\n')
	end

	classClass.register(self, pre)

	if self.flags.parent_object.flags.pure_virtual and not enable_pure_virtual then
		output('#endif // __GNUC__ >= 4\n')
	end
end


--function classVirtualClass:requirecollection(_c)
--	if self.flags.parent_object.flags.pure_virtual then
--		return false
--	end
--	return classClass.requirecollection(self, _c)
--end

function classVirtualClass:output_parent_method_wrappers()

	for k,v in ipairs(self.methods) do

		if v.f.access ~= 2 and (not v.f.pure_virtual) and v.f.name ~= 'new' and v.f.name ~= 'delete' then

			local rettype = v.f.mod.." "..v.f.type..v.f.ptr.." "
			local parent_name = rettype..self.flags.parent_object.original_name.."__"..v.f.name

			local par_list = self:get_arg_list(v.f, true)
			local var_list = self:get_arg_list(v.f, false)

			-- the parent's virtual function
			output("\t"..parent_name..par_list.." {")

			output("\t\treturn (",rettype,")"..self.btype.."::"..v.f.name..var_list..";")
			output("\t};")
		end
	end
end

function classVirtualClass:output_methods(btype)

	for k,v in ipairs(self.methods) do

		if v.f.name ~= 'new' and v.f.name ~= 'delete' then

			self:output_method(v.f, btype)
		end
	end
	output("\n")
end

function classVirtualClass:output_constructors()

	for k,v in ipairs(self.methods) do

		if v.f.name == 'new' then

			local par_list = self:get_arg_list(v.f, true)
			local var_list = self:get_arg_list(v.f, false)

			output("\t",self.original_name,par_list,":",self.btype,var_list,"{};")
		end
	end
end

function classVirtualClass:output_method(f, btype)
	if f.access == 2 then -- private
		return
	end

	local ptr
	if f.ret ~= '' then
		ptr = f.ret
	else
		ptr = f.ptr
	end

	local rettype = f.mod.." "..f.type..f.ptr.." "
	local par_list = self:get_arg_list(f, true)
	local var_list = self:get_arg_list(f, false)

	if string.find(rettype, "%s*LuaQtGenericFlags%s*") then

		_,_,rettype = string.find(f.original_sig, "^%s*([^%s]+)%s+")
	end

	-- the caller of the lua method
	output("\t"..rettype.." "..f.name..par_list..f.const.." {")
	local fn = f.cname
	if not f:check_public_access() then
		fn = "NULL"
	end
	--output("\t\tLuaManager::ptop pt(lua_state);")
	output('\t\tif (push_method("',f.lname,'", ',fn,')) {')

	--if f.type ~= 'void' then
	--	output("\t\t\tint top = lua_gettop(lua_state)-1;")
	--end

	-- push the parameters
	local argn = 0
	for i,arg in ipairs(f.args) do
		if arg.type ~= 'void' then
			local t,ct = isbasic(arg.type)
			if t and t ~= '' then
				if arg.ret == "*" then
					t = 'userdata'
					ct = 'void*'
				end
				output("\t\t\ttolua_push"..t.."(lua_state, ("..ct..")"..arg.name..");");
			else
				local m = arg.ptr
				local push_func = get_push_function(arg.type)
				if m and m~= "" then
					if m == "*" then m = "" end
					output("\t\t\t",push_func,"(lua_state, (void*)"..m..arg.name..", \""..arg.type.."\");")
				else
					output("\t\t\tvoid* tolua_obj"..i.." = (void*)new "..arg.type.."("..arg.name..");\n")
					output('\t\t\t',push_func,'(lua_state, tolua_obj'..i..', "'..arg.type..'");\n')
					output('\t\t\ttolua_register_gc(lua_state,lua_gettop(lua_state));')
				end
			end
			argn = argn+1
		end
	end

	-- call the function
	output("\t\t\tToluaBase::dbcall(lua_state, ",argn+1,", ")

	-- return value
	if f.type ~= 'void' then
		output("1);")

		local t,ct = isbasic(f.type)
		if t and t ~= '' then
			local enumcast = ""
			if isenum(f.type) then
				enumcast = "(int)"
			end
			--output("\t\t\treturn ("..rettype..")tolua_to"..t.."(lua_state, top, 0);")
			output("\t\t\t",rettype,"tolua_ret = ("..rettype..")"..enumcast.."tolua_to"..t.."(lua_state, lua_gettop(lua_state), 0);")
		else

			local mod = ""
			if f.ptr ~= "*" then
				mod = "*("..f.type.."*)"
			end

			local to_func = get_to_function(rettype)
			--output("\t\t\treturn ("..rettype..")"..mod.."tolua_tousertype(lua_state, top, 0);")
			output("\t\t\t",rettype,"tolua_ret = ("..rettype..")"..mod..to_func.."(lua_state, lua_gettop(lua_state), 0);")
		end
		output("\t\t\tlua_pop(lua_state, 1);")
		output("\t\t\treturn tolua_ret;")
	else
		output("0);")
		output("\t\t\treturn;");
	end

	-- handle non-implemeted function
	output("\t\t};")

	if f.pure_virtual then

		output('\t\tif (lua_state)')
		--output('\t\t\ttolua_error(lua_state, "pure-virtual method '..btype.."::"..f.name..' not implemented.", NULL);')
		output('\t\t\ttolua_error(lua_state, "pure-virtual method not implemented.", NULL);')
		output('\t\telse {')
		--output('\t\t\tprintf("pure-virtual method '..btype.."::"..f.name..' called with no lua_state. Aborting");')
		output('\t\t\tprintf("pure-virtual method called with no lua_state. Aborting");')
		output('\t\t\t::abort();')
		output('\t\t};')
		if string.find(rettype, "%*") then
			output('\t\treturn NULL;')
		elseif f.type ~= 'void'  then
			--output('\t\t',rettype,' junk();')
			--output('\t\treturn junk;')
			output('\t\treturn (',rettype,')*(',rettype,'*)NULL;')
		end
	else

		output('\t\treturn (',rettype,')',btype,'::',f.name,var_list,';')
	end

	output("\t};")
end

function VirtualClass()

	local parent = classContainer.curr
	pop()

	local name = "Lua__"..parent.original_name

	local c = _Class(_Container{name=name, base=parent.name, extra_bases=nil})
	setmetatable(c, classVirtualClass)

	local ft = getnamespace(c.parent)..c.original_name
	append_global_type(ft, c)

	push(parent)
	c.flags.parent_object = parent
	c.methods = {}

	c.curr_member_access = access.public

	push(c)
	c:parse("\nvoid tolua__set_instance(_lstate L, lua_Object lo);\ntolua_readonly LuaSignalHandler _signal_handler;\n")
	pop()

	return c
end

function output_error_hook(e)

	return e
end
