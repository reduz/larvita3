require "params_editor.lua"

class("VariantEditor", GUI.VBoxContainer)

local value_types = {
	
	number = Variant.REAL,
	string = Variant.STRING,
	table = Variant.VARIANT_ARRAY,
	Vector3 = Variant.VECTOR3,
	Matrix4 = Variant.MATRIX4,
	Plane = Variant.PLANE,
	Rect3 = Variant.RECT3,
	AABB = Variant.AABB,
	Quaternion = Variant.QUAT,
	Color = Variant.COLOR,
};

local variant_types = {
	
	{Variant.NIL, "nil"},
	{Variant.BOOL, "bool"}, 
	{Variant.INT, "int"},
	{Variant.REAL, "real"}, 
	{Variant.STRING, "string"}, 
	{Variant.INT_ARRAY, "int array"},
	{Variant.REAL_ARRAY, "real array"}, 
	{Variant.STRING_ARRAY, "string array"},
	{Variant.BYTE_ARRAY, "byte array"},
	{Variant.VARIANT_ARRAY, "variant array"},
	{Variant.VECTOR3, "Vector3"}, 
	{Variant.MATRIX4, "Matrix4"},
	{Variant.PLANE, "Plane"},
	{Variant.RECT3, "Rect3"},
	{Variant.AABB, "AABB"},
	{Variant.QUAT, "Quaterion"},
	{Variant.COLOR, "Color"},
	{Variant.RESOURCE_ID, "IObj"},
	{Variant.ASSET_ID, "Asset id"},
}

function VariantEditor:type_selected(n)

	local value
	if self.current_editor and self.current_editor.get_value then
		value = self.current_editor:get_value()
	end
	
	self:edit(value, variant_types[n+1][1])
end

function VariantEditor:init_gui()

	local init_float_range = function(widget)
	
		local range = widget:get_range()
		range:set_min(-24000000)
		range:set_max(24000000)
		range:set_step(0.001)
	end

	local gui = {
	
		type = false,
		
		{ type = GUI.ComboBox,
			
			name = "type_combo",
			init = function(widget)
			
				for k,v in ipairs(variant_types) do
					
					widget:add_string(v[2])
				end
				
				widget.selected_signal:connect(self, self.type_selected)
			end
		},
		
		{ type = GUI.StackContainer,

			name = "editor_stack",

			-- blank
			{ type = { GUI.Label, "~\\O_o/~" },
				name = "blank_editor",
			},
			
			-- bool
			{ type = { GUI.CheckButton, "Active" },
				
				init = function(widget)
					self:add_editor(Variant.BOOL, widget)
				end,
				set_value = function(widget, val, hint)
				
					if type(val) == 'boolean' then
						widget:set_checked(val)
					else
						if tonumber(val) then
							widget:set_checked(tonumber(val) ~= 0)
						else
							widget:set_checked(false)
						end
					end
				end,
				get_value = function(widget)
				
					return widget:is_checked()
				end,
				name = "bool_editor",
			},
			
			-- string
			{ type = GUI.HBoxContainer,
				
				{ type = GUI.LineEdit,
					name = "string_edit",
				},
				{ type = { GUI.Button, "Browse" },
					name = "string_browse",
					init = function(widget)
					
						widget.pressed_signal:connect(self.file_chooser, self.file_chooser.show, GUI.FileDialog.MODE_OPEN)
					end
				},
				
				get_value = function(widget)
				
					return widget:get_text()
				end,
				set_value = function(widget, val, hint)
				
					if not hint or hint.type == IAPI.PropertyHint.HINT_NONE then
					
						self.string_edit:set_text(tostring(val))
						self.string_edit:set_editable(true)
						self.string_browse:hide()
					
					elseif hint.type == IAPI.PropertyHint.HINT_FILE then
					
						self.string_edit:set_text(tostring(val))
						self.string_edit:set_editable(false)
						self.string_browse:show()
					end
				end,
				
				set_file = function(widget, file)
				
					self.string_edit:set_text(file or "")
					self.variant_edited_signal:emit(file)
				end,
				
				init = function(widget)
				
					self:add_editor(Variant.STRING, widget)
					self.string_edit.text_enter_signal:connect(self, self.value_changed)
				end,
				name = "string_editor",
			},
			
			-- numbers			
			{ type = GUI.StackContainer,

				{ type = GUI.SpinBox,
					name = "number_spin",
				},
				{ type = GUI.ComboBox,
					name = "number_combo",
				},
				{ type = GUI.VBoxContainer,
					name = "flags_vbox",
				},
				
				init = function(widget)
				
					self:add_editor(Variant.INT, widget)
					self:add_editor(Variant.REAL, widget)
					
					self.number_spin:get_range().value_changed_signal:connect(self, self.value_changed)
					self.number_combo.selected_signal:connect(self, self.value_changed)
				end,
				
				set_value = function(widget, val, hint)

					if hint and hint.type == IAPI.PropertyHint.HINT_ENUM then

						widget:raise_frame(self.number_combo)
						local list = split(hint.hint, ",")
						self.number_combo:clear()
						for k,v in ipairs(list) do
						
							self.number_combo:add_string(v)
						end
					elseif hint and hint.type == IAPI.PropertyHint.HINT_FLAGS then

						widget:raise_frame(self.flags_vbox)
						local list = split(hint.hint, ",")

						if widget.buttons then
							for k,v in ipairs(widget.buttons) do
							
								v:delete()
							end
						end
						widget.buttons = {}

						for k,v in ipairs(list) do
							local button = GUI.MenuButton:new(v, -1, true)
							button:set_checked(bw_and(val, 2^(k-1)) ~= 0)
							self.flags_vbox:add(button, 1)
							table.insert(widget.buttons, button)
							button.toggled_signal:connect(self, self.value_changed)
						end
					else
					
						widget:raise_frame(self.number_spin)
						local range = self.number_spin:get_range()
						range:set(tonumber(val) or 0)
						if hint and hint.type == IAPI.PropertyHint.HINT_RANGE then

							local r = split(hint.hint, ",")
							range:set_min(r[1])
							range:set_max(r[2])
							if r[3] then
								range:set_step(r[3])
							end
						end
					end
				end,
				
				get_value = function(widget)
				
					if self.number_combo:is_visible() then
						return self.number_combo:get_selected()
	
					elseif self.flags_vbox:is_visible() then
					
						if not widget.buttons then
							return 0
						end
						local n = 0
						for k,v in ipairs(widget.buttons) do
							if v:is_checked() then
								n = n + 2^(k-1)
							end
						end
						return n
					else
						return self.number_spin:get_range():get()
					end
				end,
				name = "number_editor",
			},
			
			-- Vector3
			{ type = GUI.HBoxContainer,
				
				"X:",
				{ type = GUI.SpinBox,
					
					name = "vector_x",
					init = init_float_range,
				},
				
				"Y:",
				{ type = GUI.SpinBox,
					
					name = "vector_y",
					init = init_float_range,
				},

				"Z:",
				{ type = GUI.SpinBox,
					
					name = "vector_z",
					init = init_float_range,
				},

				get_value = function(widget)
				
					return Vector3(self.vector_x:get_range():get(), self.vector_y:get_range():get(), self.vector_z:get_range():get())
				end,
				
				set_value = function(widget, val, hint)
				
					if val and val.x then
						self.vector_x:get_range():set(val.x)
						self.vector_y:get_range():set(val.y)
						self.vector_z:get_range():set(val.z)
					else
						self.vector_x:get_range():set(0)
						self.vector_y:get_range():set(0)
						self.vector_z:get_range():set(0)
					end
				end,
				
				init = function(widget)
					self:add_editor(Variant.VECTOR3, widget)
					self.vector_x:get_range().value_changed_signal:connect(self, self.value_changed)
					self.vector_y:get_range().value_changed_signal:connect(self, self.value_changed)
					self.vector_z:get_range().value_changed_signal:connect(self, self.value_changed)
				end,
				
				name = "vector_editor",
			},

			-- arrays
			{ type = GUI.List,

				set_value = function(widget, val, hint, type)
				
					widget.hint = hint
					widget.value = val
					widget.val_type = type
					if val then
					
						widget:clear()
						for i=0,val:size()-1 do
						
							widget:add_string(tostring(val:array_get(i)))
						end
					end
				end,
				
				get_value = function(widget)

					if widget.value then

						local t = widget.value:get_type()

						for i=0,widget:get_size()-1 do
	
							if t == Variant.INT_ARRAY then
								
								widget.value:array_set_int(i, tonumber(widget:get_string(i)))
								
							elseif t == Variant.REAL_ARRAY then

								widget.value:array_set_real(i, tonumber(widget:get_string(i)))
							
							elseif t == Variant.STRING_ARRAY then

								widget.value:array_set_string(i, widget:get_string(i))
							end
						end
					end
						
					return widget.value
				end,
				
				init = function(widget)

					widget:set_editable(true)				
					widget:set_minimum_size(GUI.Point(200, 150))
					self:add_editor(Variant.INT_ARRAY, widget)
					self:add_editor(Variant.REAL_ARRAY, widget)
					self:add_editor(Variant.STRING_ARRAY, widget)
					self:add_editor(Variant.BYTE_ARRAY, widget)

					widget.item_edited_signal:connect(self, self.value_changed)
				end,
			},
			
			-- Matrix4
			{ type = GUI.VBoxContainer,
				
				{ type = { GUI.GridContainer, 4 },
					
					name = "matrix_grid",
				},
				
				{ type = GUI.HBoxContainer,
					{ type = { GUI.Button, "Identity" },
						
						name = "matrix_identity",
					},
					
					{ type = { GUI.Button, "Inverse" },
						
						name = "matrix_inverse",
					},
					
					{ type = { GUI.Button, "Clear Scale" },
						
						name = "matrix_clear_scale",
					},
				},
				
				set_value = function(widget, val, hint)

					if not val or tolua.type(val) ~= "Matrix4" then
						val = Matrix4()
					end
					widget.value = val
					widget.hint = hint
					
					local c = 1
					for row = 0,3 do
						for col = 0,3 do
						
							local l = widget.widget_list[c]
							l:set_text(val:get(col, row))
						
							c = c+1
						end
					end
				end,
				
				get_value = function(widget)
				
					local m = Matrix4()
					local c = 1
					for row = 0,3 do
						for col = 0,3 do
					
							local l = widget.widget_list[c]
							m:set(col, row, l:get_text())
							c = c+1
						end
					end
					
					return m
				end,
				
				set_identity = function(widget)
				
					widget:set_value(Matrix4(), widget.hint)
					self:value_changed()
				end,

				invert = function(widget)
				
					local val = widget:get_value()
					widget:set_value(val:inverse(), widget.hint)
					self:value_changed()
				end,
				
				clear_scale = function(widget)
				
					local val = widget:get_value()
					local s = val:get_scale()
					s.x = 1/s.x
					s.y = 1/s.y
					s.z = 1/s.z
					val:scale(s)
					widget:set_value(val)
					self:value_changed()
				end,
				
				init = function(widget)

					widget.widget_list = {}
					for row = 0,3 do
						for col = 0,3 do
						
							local l = GUI.LineEdit:new()
							self.matrix_grid:add_frame(l, true, true)
							l.text_enter_signal:connect(self, self.value_changed)
							table.insert(widget.widget_list, l)
						end
					end
					self:add_editor(Variant.MATRIX4, widget)
					
					self.matrix_identity.pressed_signal:connect(widget, widget.set_identity)
					self.matrix_inverse.pressed_signal:connect(widget, widget.invert)
					self.matrix_clear_scale.pressed_signal:connect(widget, widget.clear_scale)
				end,

				name = "matrix4_editor",
			},
			
			-- Color
			{ type = { GUI.ColorPicker, true },
				
				
				set_value = function(widget, val, hint)
				
					if val and val.r then
					
						widget:set_color(GUI.Color(val.r, val.g, val.b), val.a)
					end
				end,
				
				get_value = function(widget)
				
					local c = widget:get_color()
					return Color(c.r, c.g, c.b, widget:get_alpha())
				end,
				
				init = function(widget)
				
					self:add_editor(Variant.COLOR, widget)
					widget.color_changed_signal:connect(self, self.value_changed)
				end,
			},
			
			-- Image
			{ type = GUI.VBoxContainer,
			
				name = "image_editor",
				
				{ type = GUI.Icon,
				
					name = "image_icon",
				},
				{ type = { GUI.Button, "New" },
					name = "new_image_button",
				},
				
				init = function(widget)
				
					--print(debug.traceback("new image connect here"))
					self:add_editor(Variant.IMAGE, widget)
					print("connection is ", self.new_image_button.pressed_signal:connect(self.file_chooser, self.file_chooser.show, GUI.FileDialog.MODE_OPEN))
				end,
				
				set_value = function(widget, val, hint)
				
					widget:set_image(val)
				end,
				
				set_image = function(widget, img)
				
					local painter = SceneMainLoop:get_singleton():get_painter()
					widget.image = img
					local old_icon = self.image_icon:get_bitmap()
					if img then
						local icon = painter:create_bitmap_from_image(img)
						self.image_icon:set_bitmap(icon)
					else
						self.image_icon:set_bitmap(GUI.INVALID_BITMAP_ID)
					end
					painter:remove_bitmap(old_icon)
					self:value_changed()
				end,
				
				set_file = function(widget, file)
				
					local image = Image()
					if ImageLoader:load_image(file, image) == OK then
						-- show image here
						widget:set_image(image)
					end
				end,
				
				get_value = function(widget)
				
					return widget.image
				end,
			},
			
			-- IObj
			{ type = { GUI.MarginGroup, "IObj list" },
				
				name = "rid_editor",
				
				{ type = GUI.ListBox,
					
					name = "list_box",
					init = function(widget)
					
						widget:set_minimum_size(GUI.Point(500, 150))
					end,
					add_args = {1},
				},

				{ type = GUI.HBoxContainer,
					{ type = { GUI.Button, "New" },
						name = "create_rid_button",
					},
					{ type = { GUI.Button, "Clear" },
						name = "clear_rid_button",
					},
				},

				init = function (widget)
					--print(debug.traceback("value_changed connect here"))
					self:add_editor(Variant.IOBJ, widget)
					self.list_box:get_list().selected_signal:connect(widget, widget.value_changed)
					self.create_rid_button.pressed_signal:connect(widget, widget.variant_new)
					self.clear_rid_button.pressed_signal:connect(widget, widget.variant_clear)
				end,
				
				value_changed = function(widget)
				
					widget.value = IAPI_Persist:get_singleton():load(widget.aid_list[self.list_box:get_list():get_selected()])
					self:value_changed()
				end,
				
				variant_new = function(widget)

					local params = IAPI_Persist:get_singleton():get_creation_param_hints(widget.hint.hint)

					if params.parameters:size() > 0 then
						if not self.params_edit then
							self.params_edit = ParamsEditor:new(self.window)
							self.params_edit.parameters_edited_signal:connect(self.rid_editor, self.rid_editor.new_done)
						end

						self.params_edit:set_parameters(params.parameters)
						self.params_edit:show()
					else
					
						local val = IAPI_Persist:get_singleton():instance_type(widget.hint.hint)
						widget:set_value(val, widget.hint)
						self:value_changed()
						self.request_hide:emit()
					end
				end,
				
				variant_clear = function(widget)
				
					widget:set_value(nil, widget.hint)
					self:value_changed()
					self.request_hide:emit()
				end,
				
				new_done = function(widget, values)
				
					local val = IAPI_Persist:get_singleton():instance_type(widget.hint.hint, self.params_edit:get_as_param_table())
					widget:set_value(val, widget.hint)
					self:value_changed()
					self.request_hide:emit()
				end,
				
				set_value = function(widget, val, hint)

					widget.value = val
					
					self.list_box:get_list():clear()
					widget.aid_list = {}

					widget:set_label_text(hint.hint.." list")

					local db = IAPI_Persist:get_singleton():get_db()

					if not db then
						return
					end
	
					local list = List_IAPI_DB__ListingElement_()

					widget.hint = hint

					db:get_listing(list, IAPI_DB.LISTING_TYPE, hint.hint)
					local elem = 0
					local I = list:begin()
					while I do
					
						local name = I:get().path
						if name == "" then name = "<Unnamed>" end
						name = tostring(I:get().id.." - "..name)
						self.list_box:get_list():add_string(name)
						widget.aid_list[elem] = I:get().id
						if val and val.get_ID and val:get_ID() == I:get().id then

							self.list_box:get_list():select(elem, true)
						end
						elem = elem+1
						I = I:next()
					end
				end,
				
				get_value = function(widget)
					return widget.value
				end,
			},
			
		},
	}

	self.file_chooser = GUI.FileDialog:new(self.window)
	
	local box = load_gui(self, gui, self)

	self.file_chooser.file_activated_signal:connect(self, self.file_activated)
end

function VariantEditor:file_activated(file)

	local rp = strip_resource_path(file)
	if rp then
		file = "res://"..rp
	end

	if self.current_editor.set_file then
		self.current_editor:set_file(file or "")
	end
end

function VariantEditor:edit(value, type, hint, change_type)

	if not type then
		if tolua.type(value) == "Variant" then
			type = value:get_type()
		else
		
			type = value_types[tolua.type(value)]
		end
	end

	self.current_editor = self.editors[type] or self.blank_editor
	if self.current_editor.set_value then
	
		self.current_editor:set_value(value, hint, type)
	end

	self.editor_stack:raise_frame(self.current_editor)

	if change_type then
	
		self.type_combo:show()
	else
	
		self.type_combo:hide()
	end
	
	self:show()
end

function VariantEditor:add_editor(type, editor)
	self.editors[type] = editor
end

function VariantEditor:value_changed()

	if self.current_editor and self.current_editor.get_value then
		self.variant_edited_signal:emit(self.current_editor:get_value())
	end
	
	--self:hide()
end

function VariantEditor:get_value()

	if self.current_editor and self.current_editor.get_value then
		return self.current_editor:get_value()
	end
end

function VariantEditor:__parent_args__()

	return
end

function VariantEditor:__init__(window)

	self.window = window or Main:get_singleton():get_main_loop():get_gui_main_window()
	self.editors = {}
	
	self.variant_edited_signal = LuaSignal:new(1)

	self.request_hide = LuaSignal:new()
	
	self:init_gui()
end

----------------------------------------------------------------------------------

class("VariantEditorPopup", GUI.Lua__Window)

function VariantEditorPopup:edit(...)

	self.editor:edit(...)
	self:set_size(GUI.Point(1, 1))
	self:show()
end

function VariantEditorPopup:__parent_args__(parent)

	return parent, GUI.Window.MODE_POPUP, GUI.Window.SIZE_NORMAL
end

function VariantEditorPopup:__init__()

	self.editor = VariantEditor:new(self)
	self:set_root_frame(self.editor)
	self.editor.request_hide:connect(self, self.hide)
end

return VariantEditorPopup

