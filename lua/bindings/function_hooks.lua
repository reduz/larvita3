
------------- OLD SUPCODE
function classFunction:supcode(local_constructor)

 local overload = strsub(self.cname,-2,-1) - 1  -- indicate overloaded func
 local nret = 0      -- number of returned values
 local class = self:inclass()
 local _,_,static = strfind(self.mod,'^%s*(static)')
 if class then

 	if self.name == 'new' and self.parent.flags.pure_virtual then
 		-- no constructor for classes with pure virtual methods
 		return
 	end

 	if local_constructor then
		output("/* method: new_local of class ",class," */")
	else
		output("/* method:",self.name," of class ",class," */")
	end
 else
  output("/* function:",self.name," */")
 end

 if local_constructor then
  output("#ifndef TOLUA_DISABLE_"..self.cname.."_local")
  output("\nstatic int",self.cname.."_local","(lua_State* tolua_S)")
 else
  output("#ifndef TOLUA_DISABLE_"..self.cname)
  output("\nstatic int",self.cname,"(lua_State* tolua_S)")
 end
 output("{")

 -- check types
	if overload < 0 then
	 output('#ifndef TOLUA_RELEASE\n')
	end
	output(' tolua_Error tolua_err;')
 output(' if (\n')
 -- check self
 local narg
 if class then narg=2 else narg=1 end
 if class then
		local func = get_is_function(self.parent.type)
		local type = self.parent.type
		if self.name=='new' or static~=nil then
			func = 'tolua_isusertable'
			type = self.parent.type
		end
		if self.const ~= '' then
			type = "const "..type
		end
		output('     !'..func..'(tolua_S,1,"'..type..'",0,&tolua_err) ||\n')
 end
 -- check args
 if self.args[1].type ~= 'void' then
  local i=1
  while self.args[i] do
   local btype = isbasic(self.args[i].type)
   if btype ~= 'value' and btype ~= 'state' then
    output('     '..self.args[i]:outchecktype(narg)..' ||\n')
   end
   if btype ~= 'state' then
	   narg = narg+1
   end
   i = i+1
  end
 end
 -- check end of list
 output('     !tolua_isnoobj(tolua_S,'..narg..',&tolua_err)\n )')
	output('  goto tolua_lerror;')

 output(' else\n')
	if overload < 0 then
	 output('#endif\n')
	end
	output(' {')

 -- declare self, if the case
 local narg
 if class then narg=2 else narg=1 end
 if class and self.name~='new' and static==nil then
  output(' ',self.const,self.parent.type,'*','self = ')
  output('(',self.const,self.parent.type,'*) ')
  local to_func = get_to_function(self.parent.type)
  output(to_func,'(tolua_S,1,0);')
 elseif static then
  _,_,self.mod = strfind(self.mod,'^%s*static%s%s*(.*)')
 end
 -- declare parameters
 if self.args[1].type ~= 'void' then
  local i=1
  while self.args[i] do
   self.args[i]:declare(narg)
   if isbasic(self.args[i].type) ~= "state" then
	   narg = narg+1
   end
   i = i+1
  end
 end

 -- check self
 if class and self.name~='new' and static==nil then
	 output('#ifndef TOLUA_RELEASE\n')
  output('  if (!self) tolua_error(tolua_S,"invalid \'self\' in function \''..self.name..'\'",NULL);');
	 output('#endif\n')
 end

 -- get array element values
 if class then narg=2 else narg=1 end
 if self.args[1].type ~= 'void' then
  local i=1
  while self.args[i] do
   self.args[i]:getarray(narg)
   narg = narg+1
   i = i+1
  end
 end

 pre_call_hook(self)

 local out = string.find(self.mod, "tolua_outside")
 -- call function
 if class and self.name=='delete' then
  output('  Mtolua_delete(self);')
 elseif class and self.name == 'operator&[]' then
  if flags['1'] then -- for compatibility with tolua5 ?
	output('  self->operator[](',self.args[1].name,'-1) = ',self.args[2].name,';')
  else
    output('  self->operator[](',self.args[1].name,') = ',self.args[2].name,';')
  end
 else
  output('  {')
  if self.type ~= '' and self.type ~= 'void' then
   output('  ',self.mod,self.type,self.ptr,'tolua_ret = ')
   output('(',self.mod,self.type,self.ptr,') ')
  else
   output('  ')
  end
  if class and self.name=='new' then
   output('Mtolua_new((',self.type,')(')
  elseif class and static then
	if out then
		output(self.name,'(')
	else
		output(class..'::'..self.name,'(')
	end
  elseif class then
	if out then
		output(self.name,'(')
	else
	  if self.cast_operator then
	  	--output('static_cast<',self.mod,self.type,self.ptr,' >(*self')
		output('self->operator ',self.mod,self.type,'(')
	  else
		output('self->'..self.name,'(')
	  end
	end
  else
   output(self.name,'(')
  end

  if out and not static then
  	output('self')
	if self.args[1] and self.args[1].name ~= '' then
		output(',')
	end
  end
  -- write parameters
  local i=1
  while self.args[i] do
   self.args[i]:passpar()
   i = i+1
   if self.args[i] then
    output(',')
   end
  end

  if class and self.name == 'operator[]' and flags['1'] then
	output('-1);')
  else
	if class and self.name=='new' then
		output('));') -- close Mtolua_new(
	else
		output(');')
	end
  end

  -- return values
  if self.type ~= '' and self.type ~= 'void' then
   nret = nret + 1
   local t,ct = isbasic(self.type)
   if t and self.name ~= "new" then
   	if self.cast_operator and _basic_raw_push[t] then
		output('   ',_basic_raw_push[t],'(tolua_S,(',ct,')tolua_ret);')
   	else
	    output('   tolua_push'..t..'(tolua_S,(',ct,')tolua_ret);')
	end
   else
	t = self.type
	new_t = string.gsub(t, "const%s+", "")
	local owned = false
	if string.find(self.mod, "tolua_owned") then
		owned = true
	end
    local push_func = get_push_function(t)
    if self.ptr == '' then
     output('   {')
     output('#ifdef __cplusplus\n')
     output('    void* tolua_obj = Mtolua_new((',new_t,')(tolua_ret));')
     output('    ',push_func,'(tolua_S,tolua_obj,"',t,'");')
     output('    tolua_register_gc(tolua_S,lua_gettop(tolua_S));')
     output('#else\n')
     output('    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(',t,'));')
     output('    ',push_func,'(tolua_S,tolua_obj,"',t,'");')
     output('    tolua_register_gc(tolua_S,lua_gettop(tolua_S));')
     output('#endif\n')
     output('   }')
    elseif self.ptr == '&' then
     output('   ',push_func,'(tolua_S,(void*)&tolua_ret,"',t,'");')
    else
	 output('   ',push_func,'(tolua_S,(void*)tolua_ret,"',t,'");')
	 if owned or local_constructor then
      output('    tolua_register_gc(tolua_S,lua_gettop(tolua_S));')
	 end
    end
   end
  end
  local i=1
  while self.args[i] do
   nret = nret + self.args[i]:retvalue()
   i = i+1
  end
  output('  }')

  -- set array element values
  if class then narg=2 else narg=1 end
  if self.args[1].type ~= 'void' then
   local i=1
   while self.args[i] do
    self.args[i]:setarray(narg)
    narg = narg+1
    i = i+1
   end
  end

  -- free dynamically allocated array
  if self.args[1].type ~= 'void' then
   local i=1
   while self.args[i] do
    self.args[i]:freearray()
    i = i+1
   end
  end
 end

 post_call_hook(self)

 output(' }')
 output(' return '..nret..';')

 self:output_upvalue_call()
 --[[
 -- call overloaded function or generate error
	if overload < 0 then

		output('#ifndef TOLUA_RELEASE\n')
		output('tolua_lerror:\n')
		output(' tolua_error(tolua_S,"#ferror in function \''..self.lname..'\'.",&tolua_err);')
		output(' return 0;')
		output('#endif\n')
	else
		local _local = ""
		if local_constructor then
			_local = "_local"
		end
		output('tolua_lerror:\n')
		output(' return '..strsub(self.cname,1,-3)..format("%02d",overload).._local..'(tolua_S);')
	end
 output('}')
 output('#endif //#ifndef TOLUA_DISABLE\n')
 output('\n')
 --]]

	-- recursive call to write local constructor
	if class and self.name=='new' and not local_constructor then

		self:supcode(1)
	end

end


local old_supcode = classFunction.supcode

local _output_list = {}

local function arg_type(self)

	local array = self.dim ~= '' and tonumber(self.dim)==nil
	local line = ""
	local ptr = ''
	local mod
	local type = self.type
	local nctype = gsub(self.type,'const%s+','')
	if self.dim ~= '' then
		type = gsub(self.type,'const%s+','')  -- eliminates const modifier for arrays
	end
	if self.ptr~='' and not isbasic(type) then ptr = '*' end
		line = concatparam(line," ",self.mod,type,ptr)
	if array then
		line = concatparam(line,'*')
	end
	
	return line
end

function classFunction:get_param_signature()


	local p = ""

	local i=1
	while self.args[i] do
	
		if i > 1 then
			p = p..","
		end
	
		p = p..arg_type(self.args[i])
	
		i = i+1
	end

	return p
end

function classFunction:get_signature()

	local t = self.type
	if t == '' then
		t = 'void'
	end
	local c = ''
	if self:inclass() then
		c = '::'
	end
	
	local p = '('..self:get_param_signature()..')'

	return t.." "..c..p
end

function classFunction:get_method_type(name)

	local t = self.type
	if t == '' then
		t = 'void'
	end

	local c = "(__GenericClass::*"..name..")("..self:get_param_signature()..")"
	
	return t.." "..c
end

function classFunction:get_method_name()

	return self.parent.type.."::"..self.name
end

function classFunction:has_default_params()

	if self.has_defaults then
		return true
	end

	local i=1
	while self.args[i] do
		if self.args[i].def and self.args[i].def ~= '' then
			return true
		end
		i = i+1
	end

	return false
end

function classFunction:output_upvalue_call()

	-- call overloaded function or generate error
	output(' tolua_lerror:\n')
	output(" if (lua_isnil(tolua_S,lua_upvalueindex(3))) {")
	output('  tolua_error(tolua_S,"#ferror in function \''..self.lname..'\'.",&tolua_err);')
	output('  return 0;')
	output(' } else {')
	output('  lua_pushvalue(tolua_S, lua_upvalueindex(3));') 
	output('  lua_insert(tolua_S, 1);')
	output('  lua_call(tolua_S, lua_gettop(tolua_S)-1, LUA_MULTRET);')
	output('  return lua_gettop(tolua_S);')
	output(' };')

end

function classFunction:supcode(local_constructor)

	if self:has_default_params() or self.name == 'new' or self.name == 'delete' then
	
		return old_supcode(self, local_constructor)
	end

	local sig = self:get_signature()
	local csig = string.gsub(sig, "[:<>%*& %(%)]", "_")
	self.csigname = csig

	if _output_list[sig] then
	
		return
	end

	local overload = strsub(self.cname,-2,-1) - 1  -- indicate overloaded func
	local nret = 0      -- number of returned values
	local class = self:inclass()
	local _,_,static = strfind(self.mod,'^%s*(static)')
	if class then
	
		output("/* method:",sig," */")
	else
		output("/* function:",sig," */")
	end

	output("#ifndef TOLUA_DISABLE_"..csig)
	output("\nstatic int",csig,"(lua_State* tolua_S)")
	
	output("{")
	
	-- check types
	if overload < 0 then
		output('#ifndef TOLUA_RELEASE\n')
	end
	output(' tolua_Error tolua_err;')
	if class then
	
		output(' const char* self_type = lua_tostring(lua_upvalueindex(2));')
	end
	output(' if (\n')
	
	-- check self
	local narg
	if class then narg=2 else narg=1 end
	if class then
		local func = get_is_function(self.parent.type)
		local type = self.parent.type
		if self.name=='new' or static~=nil then
			func = 'tolua_isusertable'
			type = self.parent.type
		end
		if self.const ~= '' then
			type = "const "..type
		end
		output('     !'..func..'(tolua_S,1,self_type,0,&tolua_err) ||\n')
	end

	-- check args
	if self.args[1].type ~= 'void' then
		local i=1
		while self.args[i] do
			local btype = isbasic(self.args[i].type)
			if btype ~= 'value' and btype ~= 'state' then
				output('     '..self.args[i]:outchecktype(narg)..' ||\n')
			end
			if btype ~= 'state' then
				narg = narg+1
			end
			i = i+1
		end
	end
	
	-- check end of list
	output('     !tolua_isnoobj(tolua_S,'..narg..',&tolua_err)\n )')
	output('  goto tolua_lerror;')
	
	output(' else\n')
	if overload < 0 then
		output('#endif\n')
	end
	output(' {')
	
	-- declare self, if the case
	local narg
	if class then narg=2 else narg=1 end
	if class and self.name~='new' and static==nil then
	
		if out then
			output(' ',self.const,self.parent.type,'*','self = ')
			output('(',self.const,self.parent.type,'*) ')
		else
			output('__GenericClass* self = (__GenericClass*)')
		end
		local to_func = get_to_function(self.parent.type)
		output(to_func,'(tolua_S,1,0);')

	elseif static then
		_,_,self.mod = strfind(self.mod,'^%s*static%s%s*(.*)')
	end
	
	-- declare parameters
	if self.args[1].type ~= 'void' then
		local i=1
		while self.args[i] do
			self.args[i]:declare(narg)
			if isbasic(self.args[i].type) ~= "state" then
				narg = narg+1
			end
			i = i+1
		end
	end

	-- check self
	if class and self.name~='new' and static==nil then
		output('#ifndef TOLUA_RELEASE\n')
		output('  if (!self) tolua_error(tolua_S,"invalid \'self\' in function \''..self.name..'\'",NULL);');
		output('#endif\n')
	end

	-- get array element values
	if class then narg=2 else narg=1 end
	if self.args[1].type ~= 'void' then
		local i=1
		while self.args[i] do
			self.args[i]:getarray(narg)
			narg = narg+1
			i = i+1
		end
	end
	
	pre_call_hook(self)

	local out = string.find(self.mod, "tolua_outside")
	output('  ', self:get_method_type("method"), ' = *lua_touserdata(lua_upvalueindex(1));')

	-- call function
	if class and self.name == 'operator&[]' and flags['1'] then -- for compatibility with tolua5 ?
		output('  (self->*method(',self.args[1].name,'-1) = ',self.args[2].name,';')
	else

		output('  {')
		if self.type ~= '' and self.type ~= 'void' then
			output('  ',self.mod,self.type,self.ptr,'tolua_ret = ')
			output('(',self.mod,self.type,self.ptr,') ')
		else
			output('  ')
		end
		if class then
			output('  (self->*method(')
		else
			output('  (method(')
		end
	end

	if out and not static then
		output('self')
		if self.args[1] and self.args[1].name ~= '' then
			output(',')
		end
	end

	-- write parameters
	local i=1
	local nargs = 0
	while self.args[i] do
		self.args[i]:passpar()
		i = i+1
		if self.args[i] then
			output(',')
		end
		nargs = nargs + 1
	end

	if class and self.name == 'operator[]' and flags['1'] then
		output('-1));')
	else
		output('));')
	end

  -- return values
  if self.type ~= '' and self.type ~= 'void' then
   nret = nret + 1
   local t,ct = isbasic(self.type)
   if t and self.name ~= "new" then
   	if self.cast_operator and _basic_raw_push[t] then
		output('   ',_basic_raw_push[t],'(tolua_S,(',ct,')tolua_ret);')
   	else
	    output('   tolua_push'..t..'(tolua_S,(',ct,')tolua_ret);')
	end
   else
	t = self.type
	new_t = string.gsub(t, "const%s+", "")
	local owned = false
	if string.find(self.mod, "tolua_owned") then
		owned = true
	end
    local push_func = get_push_function(t)
    if self.ptr == '' then
     output('   {')
     output('#ifdef __cplusplus\n')
     output('    void* tolua_obj = Mtolua_new((',new_t,')(tolua_ret));')
     output('    ',push_func,'(tolua_S,tolua_obj,"',t,'");')
     output('    tolua_register_gc(tolua_S,lua_gettop(tolua_S));')
     output('#else\n')
     output('    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(',t,'));')
     output('    ',push_func,'(tolua_S,tolua_obj,"',t,'");')
     output('    tolua_register_gc(tolua_S,lua_gettop(tolua_S));')
     output('#endif\n')
     output('   }')
    elseif self.ptr == '&' then
     output('   ',push_func,'(tolua_S,(void*)&tolua_ret,"',t,'");')
    else
	 output('   ',push_func,'(tolua_S,(void*)tolua_ret,"',t,'");')
	 if owned or local_constructor then
      output('    tolua_register_gc(tolua_S,lua_gettop(tolua_S));')
	 end
    end
   end
  end
  local i=1
  while self.args[i] do
   nret = nret + self.args[i]:retvalue()
   i = i+1
  end
  output('  }')
  -- set array element values
  if class then narg=2 else narg=1 end
  if self.args[1].type ~= 'void' then
   local i=1
   while self.args[i] do
    self.args[i]:setarray(narg)
    narg = narg+1
    i = i+1
   end
  end

  -- free dynamically allocated array
  if self.args[1].type ~= 'void' then
   local i=1
   while self.args[i] do
    self.args[i]:freearray()
    i = i+1
   end
  end

 post_call_hook(self)

 output(' }')
 output(' return '..nret..';')

 	self:output_upvalue_call()

	_output_list[sig] = true

	self.method_ptr = true
end

local old_register = classFunction.register

function classFunction:register (pre)

	if not self.method_ptr then
	
		return old_register(self, pre)
	end


	if not self:check_public_access() then
		return
	end

	if self:inclass() then
		output(pre..'tolua_mfunction(tolua_S,"'..self.lname..'",'..self.csigname..', "',self.parent.type,'", generic_method(&',self:get_method_name(),'));')
	else
		output(pre..'tolua_pfunction(tolua_S,"'..self.lname..'",'..self.csigname..');')
	end
end


function Function (d,a,c)
 --local t = split(strsub(a,2,-2),',') -- eliminate braces
 --local t = split_params(strsub(a,2,-2))

	if not flags['W'] and string.find(a, "%.%.%.%s*%)") then

		warning("Functions with variable arguments (`...') are not supported. Ignoring "..d..a..c)
		return nil
	end


 local i=1
 local l = {n=0}

 	a = string.gsub(a, "%s*([%(%)])%s*", "%1")
	local t,strip,last = strip_pars(strsub(a,2,-2));
	if strip then
		f.has_defaults = true
		--local ns = string.sub(strsub(a,1,-2), 1, -(string.len(last)+1))
		local ns = join(t, ",", 1, last-1)

		ns = "("..string.gsub(ns, "%s*,%s*$", "")..')'
		--ns = strip_defaults(ns)

		local f = Function(d, ns, c)
		f.has_defaults = true
		for i=1,last do
			t[i] = string.gsub(t[i], "=.*$", "")
		end
	end

 while t[i] do
  l.n = l.n+1
  l[l.n] = Declaration(t[i],'var',true)
  i = i+1
 end
 local f = Declaration(d,'func')
 f.args = l
 f.const = c
 return _Function(f)
end


