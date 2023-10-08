class("InstanceDialog", GUI.Lua__Window)

function InstanceDialog:show()

	self.list:clear()

	local ip = IAPI_Persist:get_singleton()
	local list = List_String_()
	ip:get_registered_types(list)
	
	local I = list:begin()
	while I do
	
		self.list:add_sorted_string(I:get())
	
		I = I:next()
	end
	
	self.at_cursor:set_checked(false)
	
	GUI.Window.show(self)
end

function InstanceDialog:ok_pressed()

	local sel = self.list:get_selected_string()
	if sel ~= "" then
		self.instance_type_selected:emit(sel, self.at_cursor:is_checked())
	end
	self:hide()
end

function InstanceDialog:init_gui()

	local gui = {
 		type = {GUI.WindowBox, "Instance Type"},

		{ type = GUI.List,
			name = "list",
			add_args = {1},
		},

		{ type = { GUI.CheckButton, "Instance at cursor" },

			name = "at_cursor",
		},
		
		{ type = GUI.HBoxContainer,

			{ type = { GUI.Button, "Ok"},

				name = "button_ok",
				init = function(widget)
				
					widget.pressed_signal:connect(self, self.ok_pressed)
				end,
			},
			{ type = {GUI.Button, "Cancel"},

				name = "button_cancel",
				init = function(widget)
				
					widget.pressed_signal:connect(self, self.hide)
				end,
			},
		},
	}

	local container = load_gui(self, gui)

	self:set_root_frame(container)
end


function InstanceDialog:__parent_args__(parent)

	return parent, GUI.Window.MODE_POPUP, GUI.Window.SIZE_TOPLEVEL_CENTER
end

function InstanceDialog:__init__(parent)

	self.instance_type_selected = LuaSignal(2)
	
	self:init_gui()
end

return InstanceDialog

