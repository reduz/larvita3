
signal_list = {}
include_list = {
	'gui/widgets/button_group.h',
	'gui/widgets/menu_button.h',
	'gui/widgets/tree.h',
	'scene/nodes/node.h',
	'physics/physics_body.h',
}

function find_type(t)

	local st = string.gsub(t, "[%*&]", "")
	_,_,st = string.find(st, "([^%s]*)%s*$")
	local mod = string.match(t, "([%*&])") or ""

	if not _global_types_hash[st] then
		for i=_global_types.n,1,-1 do
			if string.find(_global_types[i], "::"..st.."$") then
				return _global_types[i]..mod
			end
		end
	end

	return t
end

function normalize(s)

	s = findtype(s) or s

	s = string.gsub(s, "%s*([,<>])%s*", "%1")
	
	while string.find(s, ">>") do
		s = string.gsub(s, ">>", "> >")
	end

	return s
end

function add_signal(s)

	s = normalize(s)
	
	if not signal_list[s] then
	
		local method,n,parlist = string.match(s, "(Method(.))(%b<>)")
		local method_type,method_bind_type
		if not parlist then
			parlist = ""
			method_type = "Method"
			method_bind_type = "Method1"
		else
			parlist = string.sub(parlist, 2, -2)
			method_type = method
			method_bind_type = "Method"..(n+1)
		end
		
		signal_list[s] = {params = parlist, method_type = method_type, method_bind_type = method_bind_type}
	end
end

function is_container(obj)

	local mt = getmetatable(obj)
	while mt do
		if mt == classContainer then return true end
		mt = getmetatable(mt)
	end
	
	return false
end

function add_signals(package)

	foreach(_usertype,function(n,v)
		if (not _global_classes[v]) or _global_classes[v]:check_public_access() then
			if string.find(v, "^Signal<") then
				add_signal(v)
			end
		end
	 end)
end

function method_name(n)

	return "slot"..string.gsub(n, "[<>,%*&%s:]", "_")
end

function output_pigui_slots(file)

	local class_name = "LuaSignalHandler"
	
	for k,v in pairs(signal_list) do

		k = string.gsub(k, "_userdata", "void*") -- do the same with _cstring?
		local method_name = method_name(k)
	
		output_slot(file, class_name, method_name, "void", v.params, false, true, true)
	end
end

function output_slot(file, class_name, method_name, return_value, arguments, ref, in_header, use_p_slot)

	local sep = "."
	if ref then sep = "->" end

	local vars = {}
	local var_args = ""
	local types = {}
	if arguments ~= "" then
		types = split(arguments, ",")
		for i=1,table.getn(types) do
			types[i] = find_type(types[i])
			vars[i] = "var"..i
			var_args = var_args..types[i].." "..vars[i]..","
		end
		var_args = string.gsub(var_args, ",$", "")
	end

	local retn
	if return_value == 'void' then
		retn = 0
	else
		retn = 1
	end

	local p_slot = ""
	if use_p_slot then
		p_slot = "LuaSlot lua_slot"
		if var_args ~= "" then
			p_slot = ","..p_slot
		end
	end

	if in_header then
		file:write("\t"..return_value.." "..method_name.."("..var_args..p_slot..") {\n\n")
	else
		file:write("\t"..return_value.." "..class_name.."::"..method_name.."("..var_args..p_slot..") {\n\n")
	end

	--file:write("\t\tif (!lua_slot"..sep.."is_empty() {\n\n")
	file:write("\t\t\tlua_slot"..sep.."push_call();\n")

	for k,v in ipairs(types) do
		local st = string.gsub(v, "[%*&]", "")
		_,_,st = string.find(st, "([^%s]*)%s*$")
		local t,ct = isbasic(st)
		if t then
			file:write("\t\t\ttolua_push"..t.."(lua_slot"..sep.."ls, ("..ct..")"..vars[k]..");\n");
		else
			local strip = string.gsub(v, "[%*&]", "")
			local f,_,m = string.find(v, "([%*&])")
			if f then
				if m == "*" then m = "" end
				file:write("\t\t\ttolua_pushusertype(lua_slot"..sep.."ls, (void*)"..m..vars[k]..", \""..strip.."\");\n")
			else
				local new_t = string.gsub(strip, "const%s+", "")
	 			file:write("\t\t\tvoid* tolua_obj"..k.." = memnew(("..new_t..")("..vars[k].."));\n")
				file:write('\t\t\ttolua_pushusertype_and_takeownership(lua_slot'..sep..'ls, tolua_obj'..k..', "'..strip..'");\n')
			end
		end
	end
	file:write("\t\t\tlua_dbcall(lua_slot"..sep.."ls, "..tostring(table.getn(types))..", "..retn..");\n")

	--file:write("\t\t};\n") -- is_empty

	if retn > 0 then -- return value not supported yet
		file:write("\t\treturn "..return_value.."(); // Warning!! unitialized value.\n")
	end
	file:write("\t};\n\n")
end

function output_connector(file, classname)

	file:write("\tint connect_signal_internal(void* p_signal, String p_signal_signature, LuaSlot& slot) {\n\n")

	for k,v in pairs(signal_list) do

		k = string.gsub(k, "_userdata", "void*") -- do the same with _cstring?
		k = normalize(k)
	
		file:write("\t\tif (p_signal_signature == \""..k.."\") {\n\n")

		local sep = ""
		local mpars = ""
		if v.params ~= "" then
			sep = ","
			mpars = "<"..v.params..">"
		end
		
		local mtype = v.method_type..mpars

		file:write("\t\t\t"..k.."* signal = ("..k.."*)p_signal;\n")
		file:write("\t\t\t"..mtype.." m("..v.method_bind_type.."<"..v.params..sep.."LuaSlot>(this, &"..classname.."::"..method_name(k).."), slot);\n")
		file:write("\t\t\tint count = signal->connect(m);\n")
		file:write("\t\t\treturn count;\n")
			
		file:write("\t\t};\n\n")
	end

	file:write("\t\tprintf(\"signal type is %ls\\n\", p_signal_signature.c_str());")
	file:write("\t\tERR_PRINT(\"Signal type not found, returning -1\");\n")

	file:write("\t\treturn -1;\n")
	file:write("\t};\n\n")


end

function output_signal_handlers(package, fh)

	if not next(signal_list) then return end
	
	local file = fh
	
	if not fh then
		local outpath = "."
		if flags['o'] then
			b = string.find(flags.o, "[^/]*$")
			outpath = string.sub(flags.o, 1, b-1)
			if outpath == "" then outpath = "." end
		end
	
		local basename = "lua_signal_handlers_"..package.name
		local filename = outpath.."/"..basename..".h"
		print("* opening filename "..filename)
		local e
		file,e = io.open(filename, "wb")
		if not file then
			error("#unable to open file "..filename.." for signal handlers: "..e)
			return
		end
	end

	file:write("#ifndef LUA_SIGNAL_HANDLER_"..package.name.."_H\n")
	file:write("#define LUA_SIGNAL_HANDLER_"..package.name.."_H\n")
	
	file:write("#include \"lua_signal_handler.h\"\n\n")

	--[[
	file:write('//using namespace GUI;\n')
	file:write('#include "defines.h"\n')
	file:write('#include "os/memory.h"\n')
	file:write('#include "lua.hpp"\n\n')
	file:write('#include "lua/helpers.h"\n')
	
	for k,v in ipairs(include_list) do
	
		file:write("#include \""..v.."\"\n")
	end
	--]]

	file:write("\n#define _userdata void*\n\n")
	
	local classname = "LuaSignalHandler__"..package.name
	file:write("class "..classname.." : public LuaSignalHandler {\n")
	file:write("public:\n")
	
	output_pigui_slots(file)

	output_connector(file, classname)

	file:write("\tstatic LuaSignalHandler* create() {\n\n")
	file:write("\t\treturn memnew(("..classname.."));\n");
	file:write("\t};\n")
	
	file:write("\tstatic void set_as_default() {\n\n")
	file:write("\t\tLuaSignalHandler::create = &"..classname.."::create;\n");
	file:write("\t};\n")

	file:write("};\n")

	file:write("#endif\n")
	if not fh then
		file:close()	
	end
end
