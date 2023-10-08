class("ParamIAPI", Lua__IAPI)

function ParamIAPI:get(path)
print("get ", path, self.vals[path])
	return self.vals[path]
end

function ParamIAPI:set(path, val)
print("set ", path, val)
	self.vals[path] = val
end

function ParamIAPI:get_type()

	return "ParamIAPI"
end

function ParamIAPI:get_property_hint(path)

	return self.hints[path]
end

function ParamIAPI:get_property_list(p_list)

	local info = IAPI.PropertyInfo()
	for k,v in ipairs(self.params) do
	
		p_list:push_back(v)
	end
end

function ParamIAPI:get_method_list(list)

end

function ParamIAPI:__parent_args__(hints)

end

function ParamIAPI:get_as_param_table()

	local t = Table_String_Variant_()
	for k,v in ipairs(self.params) do
	
		t:set(v.path, self:get(v.path))
	end
	
	return t
end


function ParamIAPI:get_as_param_list()

	local t = List_Variant_()
	for k,v in ipairs(self.params) do
	
		t:push_back(self:get(v.path))
	end
	
	return t
end

function ParamIAPI:__init__(hints)

	self.vals = {}
	
	self.hints = {}
	self.params = {}
	
	if hints.get_property_list then -- iapi
	
		local list = hints:get_property_list()
		local I = list:begin()
		while I do
		
			table.insert(self.params, IAPI.PropertyInfo(I:get()))
			self.hints[I:get().path] = hints:get_property_hint(I:get().path)
			self.vals[I:get().path] = self.hints[I:get().path].default_value
		
			I = I:next()
		end
	else
		local I = hints:begin()
		while I do
	
			table.insert(self.params, IAPI.PropertyInfo(I:get().type, I:get().name))
			self.hints[I:get().name] = IAPI.PropertyHint(I:get().hint)
			self.vals[I:get().name] = I:get().hint.default_value
print("loading with default value", tolua.type(I:get().hint), I:get().name, I:get().hint.default_value)		
			I = I:next()
		end
	end
end

class("ParamsEditor", GUI.Window)

function ParamsEditor:clear()

	if self.iapi then
		self.iapi_editor:clear()
		self.iapi = nil
	end
end

function ParamsEditor:set_parameters(params)

	self:clear()
	self.iapi = ParamIAPI:new(params)
	
	self.iapi_editor:load_iapi(self.iapi)
end

function ParamsEditor:init_gui()

	local gui = {
		
		type = { GUI.WindowBox, "Parameter Editor" },
		
		{ type = IAPIEditor,
			
			name = "iapi_editor",
			add_args = { 1 },
		},
		
		{ type = { GUI.Button, "Ok" },
			
			name = "button",
			init = function(widget)
			
				widget.pressed_signal:connect(self, self.ok_pressed)
			end,
		},
	}
	
	local box = load_gui(self, gui)
	
	self:set_root_frame(box)
end

function ParamsEditor:get_as_param_table()

	return self.iapi:get_as_param_table()
end


function ParamsEditor:get_as_param_list()

	return self.iapi:get_as_param_list()
end

function ParamsEditor:ok_pressed()

	self.parameters_edited_signal:emit()
	--self:clear()
	self:hide()
end

function ParamsEditor:__parent_args__(parent)

	return parent, GUI.Window.MODE_POPUP, GUI.Window.SIZE_TOPLEVEL_CENTER
end

function ParamsEditor:__init__(parent)

	self.parameters_edited_signal = LuaSignal:new(0)

	self:init_gui()
end

return ParamsEditor
