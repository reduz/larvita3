require "variant_editor.lua"
require "params_editor.lua"

class("IAPIEditor", GUI.VBoxContainer)

local function load_icon( icon )

	return Main:get_singleton():get_main_loop():get_painter():load_bitmap("res://icons/" .. icon )
end

function IAPIEditor:load_icons()

	if IAPIEditor.variant_icon then
		return
	end

	IAPIEditor.variant_icon={}
	
	IAPIEditor.icon_edit = load_icon("icon_edit.png")
	IAPIEditor.icon_rid = load_icon("icon_rid.png")
	IAPIEditor.icon_folder = load_icon("icon_folder.png")
	IAPIEditor.icon_methods = load_icon("icon_func.png")
	IAPIEditor.iapi_icon=nil
	
	IAPIEditor.variant_icon[Variant.BOOL]=load_icon("icon_bool.png")
	IAPIEditor.variant_icon[Variant.REAL]=load_icon("icon_real.png")
	IAPIEditor.variant_icon[Variant.INT]=load_icon("icon_integer.png")
	IAPIEditor.variant_icon[Variant.BYTE_ARRAY]=load_icon("icon_array_data.png")
	IAPIEditor.variant_icon[Variant.REAL_ARRAY]=load_icon("icon_array_float.png")
	IAPIEditor.variant_icon[Variant.INT_ARRAY]=load_icon("icon_array_int.png")
	IAPIEditor.variant_icon[Variant.STRING_ARRAY]=load_icon("icon_array_string.png")
	IAPIEditor.variant_icon[Variant.VARIANT_ARRAY]=load_icon("icon_array_variant.png")
	IAPIEditor.variant_icon[Variant.STRING]=load_icon("icon_string.png")
	IAPIEditor.variant_icon[Variant.COLOR]=load_icon("icon_color.png")
	IAPIEditor.variant_icon[Variant.MATRIX4]=load_icon("icon_matrix.png")
	IAPIEditor.variant_icon[Variant.VECTOR3]=load_icon("icon_vector.png")
	IAPIEditor.variant_icon[Variant.PLANE]=load_icon("icon_plane.png")
	IAPIEditor.variant_icon[Variant.AABB]=load_icon("icon_rect3.png")
	IAPIEditor.variant_icon[Variant.QUAT]=load_icon("icon_quat.png")
--	IAPIEditor.variant_icon[Variant.RESOURCE_ID]=load_icon("icon_rid.png")

end

function IAPIEditor:value_edited(col, item, prop)
	
	if prop.type == Variant.BOOL then
		
		self.iapi:set(prop.path, item:is_checked(1))
		
	elseif prop.type == Variant.INT or prop.type == Variant.REAL then
		
		self.iapi:set(prop.path, item:get_range(1):get())
		
	elseif prop.type == Variant.STRING then
		
		self.iapi:set(prop.path, item:get_text(1))
	else

		if prop.type == Variant.IOBJ then
			local api = self.iapi:get(prop.path)
			if api then
				self:queue_iapi(api)
				self:iapi_stack_add(api)
				self.iapi_selected_signal:emit(api)
			end
		end
	end
end

function IAPIEditor:iapi_stack_add(p_iapi)

end

function IAPIEditor:custom_value_edited(col, rect, item, prop)

	self.current_editing_path = prop.path
	local val = self.iapi:get(prop.path)
	self.variant_editor:set_pos(rect.pos)
	self.variant_editor:edit(val, prop.type, self.iapi:get_property_hint(prop.path), val == nil)
end

function IAPIEditor:update_values(path)

	if path and self.items[path] and self.item_types[path] then

		self.updating_values = true
	
		local val = self.iapi:get(path)
		
		local t = self.item_types[path].type
		local v = self.items[path]
		if t == Variant.NIL then
		
			v:set_text(1, "<nil>")
			
		elseif t == Variant.BOOL then
		
			v:set_checked(1, val)
		
		elseif t == Variant.INT or t == Variant.REAL then

			if v:get_range(1) then
				v:get_range(1):set(val or 0)
			end
			
		elseif t == Variant.RESOURCE_ID then

			v:set_bitmap(0,self.icon_rid)
			if (not val) or (not tolua.type(val) == "RID") or (not val:is_valid()) then
				
				v:set_text(1, "<Unasigned>")
			else
		
				local iface = Main:get_singleton():get_main_loop():get_asset_manager():get_asset_interface(val)
				if iface then
				
					v:set_text(1, iface:get_name(val))
					
				else
					v:set_text(1, "<RID>")
				end
			end
		elseif val == nil then
			
			v:set_text(1, "<NIL>")

		else
		
			v:set_text(1, tostring(val))
		end
		self.updating_values = false
	
	else

		self:load_property_list()
		for k,v in pairs(self.items) do
	
			self:update_values(k)
		end
	end
end

function IAPIEditor:config_item(item, val, pinfo)

	local t = pinfo.type
	
	if t == Variant.NIL then
	
		item:set_cell_mode(1, GUI.CELL_MODE_STRING)
		item:set_editable(1, true)

	elseif t == Variant.BOOL then
	
		item:set_cell_mode(1, GUI.CELL_MODE_CHECK)
		item:set_text(1, "Active")
		
	elseif t == Variant.INT or t == Variant.REAL then

		local hint = self.iapi:get_property_hint(pinfo.path)

		if hint.type == IAPI.PropertyHint.HINT_FLAGS then 

			item:set_cell_mode(1, GUI.CELL_MODE_CUSTOM)
		else
			item:set_cell_mode(1, GUI.CELL_MODE_RANGE)
			item:set_editable(1, true)
			local range = item:get_range(1)
	
			if hint.type == IAPI.PropertyHint.HINT_NONE then
			
				range:set_min(-16000000)
				range:set_max(16000000)
				range:set_step(0.001)
			end
			if hint.type == IAPI.PropertyHint.HINT_RANGE then
			
				local vals = split(hint.hint, ",")
				range:set_min(vals[1])
				range:set_max(vals[2])
				if vals[3] then
					range:set_step(vals[3])
				end
			end
			if hint.type == IAPI.PropertyHint.HINT_ENUM then

				local vals = split(hint.hint, ",")
				range:set_min(0)
				range:set_max(#vals-1)
				item:set_text(1, hint.hint)
			end
		end
	
	elseif t == Variant.STRING then
		
		local hint = self.iapi:get_property_hint(pinfo.path)
		if hint.type == IAPI.PropertyHint.HINT_FILE then
		
			item:set_cell_mode(1, GUI.CELL_MODE_CUSTOM)
		end
	else
	
		item:set_cell_mode(1, GUI.CELL_MODE_CUSTOM)
		if t == Variant.RESOURCE_ID then
			item:set_bitmap(1, self.icon_edit)
		end
	end
	
	item.edited_signal:connect(self, self.value_edited, item, pinfo)
	item.custom_popup_signal:connect(self, self.custom_value_edited, item, pinfo)
	item:set_editable(1, true)
end

function IAPIEditor:iapi_changed(path, iapi)

	if iapi ~= self.iapi then
		return
	end

	if self.updating_values then
		return
	end
	
	if path == "" then
		self:queue_iapi(iapi)
	else
		self:update_values(path)
	end
end

function IAPIEditor:timer_timeout(iapi)

	self.timer:delete()
	self:load_iapi(iapi)
end

function IAPIEditor:queue_iapi(iapi)

	self:get_window().update_signal:connect(self, self.load_iapi, iapi, self.icon_rid)
end

function IAPIEditor:call_method(n)
	
	if n then
	
		if not self.params_edit then
			self.params_edit = ParamsEditor:new(self:get_window())
			self.params_edit.parameters_edited_signal:connect(self, self.call_method)
		end
		
		local method_list = List_IAPI__MethodInfo_()
		self.iapi:get_method_list( method_list )

		if n >= method_list:size() then
		
			ERROR("Invalid method?", n)
			return
		end

		local method = method_list[n]
		self.current_method = method.name
		
		local pars = method.parameters
		if pars:size() == 0 then

			self.iapi:call(method.name, List_Variant_())
			self.current_method = nil
			return
		end
		
		self.params_edit:set_parameters(method.parameters)
		self.params_edit:show()
	else
	
		if not self.current_method then
			return
		end
		self.iapi:call(self.current_method, self.params_edit:get_as_param_list())
	end
end

function IAPIEditor:load_property_list()

	self.method_menu:clear()
	self.tree:clear()
	local root = self.tree:create_item()
	
	root:set_text(0, (self.iapi.get_name and self.iapi:get_name()) or self.iapi:get_type())
	root:set_text(1, self.iapi:get_type())
	
	root:set_selectable(1,false)
	
	if (iapi_icon) then
		root:set_bitmap(0,iapi_icon)
	end

	local method_list = List_IAPI__MethodInfo_()

	self.iapi:get_method_list( method_list )
	
	if ( self.tools_bar:is_visible() and not method_list:empty() ) then

		self.method_hints = {}
		
		local I = method_list:begin()
		local meth_count = 0
		
		while(I) do

			local skip = false
			if self.filter then
				if type(self.filter) == 'function' then
				
					skip = self.filter(I:get().name, 'method')
				else
					skip = not string.match(I:get().name, self.filter)
				end
			end
			
			if not skip then

				self.method_menu:add_item(I:get().name, meth_count)
			end
			meth_count = meth_count + 1
			I=I:next()
		end
	end

	local list = List_IAPI__PropertyInfo_()
	self.iapi:get_property_list(list)
	self.prop_list = list

	local item_tree = { root }
	
	local function create_item(name,t)
	
		local path = split(name, "/")
		local p = item_tree
		local parent_it = root
				
		
		for i=1,#path do
		
			if not p[path[i]] then
				local it = self.tree:create_item(p[1])
				
				s=path[i];
				--s=string.gsub(s,"_"," ");
				--s = s:gsub("^%l", string.upper)
				it:set_text(0, s)
				
				if (i<#path) then
					it:set_bitmap(0,self.icon_folder)
				else
					if IAPIEditor.variant_icon[t] then
						it:set_bitmap(0,IAPIEditor.variant_icon[t])
					end
				end
				
				p[path[i]] = { it }
				--print(path[i]," ",i," ",#path)
			end
			p = p[path[i]]
		end
		
		return p[1]
	end

	self.items = {}
	self.item_types = {}
	
	local I = list:begin()
	while I do

		if bw_and(I:get().usage_flags, IAPI.PropertyInfo.USAGE_EDITING) ~= 0 then

			local skip = false
			if self.filter then
				if type(self.filter) == 'function' then
				
					skip = self.filter(I:get().path, 'property')
				else
					skip = not string.match(I:get().path, self.filter)
				end
			end
			
			if not skip then

				local item = create_item(I:get().path,I:get().type)
				self.items[I:get().path] = item
				self.item_types[I:get().path] = I:get()
				
				local val = self.iapi:get(I:get().path)
				self:config_item(item, val, I:get())
				item.selected_signal:connect(self, self.item_selected, I:get().path)
			end
		end
		
		I = I:next()
	end

end

function IAPIEditor:load_iapi(p_iapi,iapi_icon, filter)
	
	self:clear()
	
	self.iapi = p_iapi
	self.filter = filter

	if not self.iapi then
		return
	end
	
	local list = List_IAPI__PropertyInfo_()
	self.iapi:get_property_list( list )
	self.prop_list = list
	
	self:update_values()
	
	self.connection = self.iapi.property_changed_signal:connect(self, self.iapi_changed, self.iapi)
end

function IAPIEditor:item_selected(col, path)

	self.selected_signal:emit(col, path)
	
	local hint = self.iapi:get_property_hint(path)
	self.description:set_text(hint.description)
end

function IAPIEditor:variant_edited(val)

	if not self.current_editing_path then
		return
	end

	self.iapi:set(self.current_editing_path, val)
	self:update_values(self.current_editing_path)
end

function IAPIEditor:toggle_description()

	if self.description:is_visible() then
	
		self.description:hide()
	else
		self.description:show()
	end
end

function IAPIEditor:question_answered(answer)

	if self.question_pending then
		self:question_pending(answer)
		self.question_pending = nil
	end
end

function IAPIEditor:save_iapi(ans)

	if not self.iapi then
		return
	end

	if not self.question_dialog then

		self.question_dialog = GUI.StringInputDialog:new(self:get_window())
		self.question_dialog.entered_string_signal:connect(self, self.question_answered)
	end
	
	if not ans then
	
		self.question_pending = self.save_iapi
		local id = self.iapi:get_ID()
		local cur = ""
		local question
		if id == OBJ_INVALID_ID then
			question = "Saving new iapi, select name"
		else
			question = "Saving iapi with id "..tostring(id)..", select name"
			cur = IAPI_Persist:get_singleton():get_db():get_path(id)
		end
		
		self.question_dialog:show(question, cur)
		return
	end
	
	if IAPI_Persist:get_singleton():save(self.iapi, ans) ~= OK then
	
		self.question_pending = nil
		self.question_dialog:show("Error saving.")
	end
end

function IAPIEditor:clear()

	self.tree:clear()
	if self.iapi and self.connection then

		self.iapi.property_changed_signal:disconnect(self.connection)
		self.connection = nil
	end

	self.iapi = nil
	self.filter = nil
	
	self.prop_list = {}
	self.items = {}
	self.item_types = {}
end

function IAPIEditor:init_gui(hide_tools, hide_description)

	local gui = {
		type = false,
		
		{ type = GUI.HBoxContainer,

			name = "tools_bar",
			
			{ type = { GUI.Button, "", load_icon("icon_file.png") },
				
				name = "button_save",
				init = function(widget)
				
					widget.pressed_signal:connect(self, self.save_iapi)
				end,
			},
			
			{ type = { GUI.MenuBox, "", load_icon("icon_func.png") },

				name = "method_menu",
				init = function(widget)
				
					widget.item_selected_signal:connect(self, self.call_method)
				end,
			},
			
			{ type = { GUI.Button, "", load_icon("icon_help.png") },
				
				name = "help_button",
				init = function(widget)
				
					widget.pressed_signal:connect(self, self.toggle_description)
				end,
			},
		},
		
		{ type = GUI.ScrollBox,
			
			name = "scroll_box",

			add_args = {2},
			{ type = { GUI.Tree, 2 },
			
				name = "tree",
				
				add = function(widget, parent)
				
					parent:set_frame(widget)
				end,
			},
			
			init = function(widget)
			
				widget:set_expand_h(true)
				widget:set_expand_v(true)
			end,
		},
		
		{ type = GUI.TextView,

			name = "description",
			add_args = {1},
		},
	}
	
	load_gui(self, gui, self)
	
	if hide_tools then
		self.tools_bar:hide()
	end
	if hide_description then
		self.description:hide()
	end
end

function IAPIEditor:__parent_args__()

end

function IAPIEditor:__init__(hide_tools, hide_description)

	self:init_gui(hide_tools, hide_description)

	self.variant_editor = VariantEditorPopup:new(Main:get_singleton():get_main_loop():get_gui_main_window())
	self.variant_editor:hide()
	self.variant_editor.editor.variant_edited_signal:connect(self, self.variant_edited)

	self.selected_signal = LuaSignal:new(2)
	self.iapi_selected_signal = LuaSignal:new(1)
	
	self:load_icons()
end

return IAPIEditor
