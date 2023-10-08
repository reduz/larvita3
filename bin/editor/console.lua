class("Console", GUI.Window)

local instance

local function find_file(f, da)

	return nil
end

local special_vals = {
	
	["true"] = true,
	["false"] = false,
}

-- static
function Console:toggle()

	if instance:is_visible() then
	
		instance:hide()
	else
		instance:show()
	end
end

function Console:show()

	self:update_var_buttons()
	self.cmdline:get_focus()
	GUI.Window.show(self)
end

function Console:unhandled_key(p_unicode, p_scan_code, p_press, p_repeat,p_modifier_mask)

	if p_press and p_scan_code == KEY_BACKQUOTE then
		self:toggle()
	end
end

function Console:entered_command(cmd)

	if cmd == "" then
		return
	end
	
	self.cmdline:set_text("")
	
	if string.match(cmd, "^%s*reload") then
	
		local f = string.match(cmd, "^%s*reload%s+(.*)%s*$")
		if not f then
			self:error("invalid command")
			return
		end
		
		if not self.dir_access:file_exists("res://"..f) then
		
			f = find_file(f, self.dir_access)
		end
		
		if not f then
			self:error("script "..f.." not found")
			return
		end
		
		require(f, false, true)
		self:msg("Reloading script "..f)
		
		return
	end
	
	if string.match(cmd, "^%s*set%s") then
	
		local n, v = string.match(cmd, "^%s*set%s+(.*)%s+(.*)%s*$")
		if n then
		
			v = tonumber(v) or v
			if special_vals[v] ~= nil then
				v = special_vals[v]
			end
			if v == "nil" then
				v = nil
			end
		
			GlobalVars:get_singleton():set(n, v)
			self:update_var_buttons()
			self:msg("setting variable "..n.." to "..tostring(v))
		else
			self:error("invalid variable for set")
		end

		return
	end
	
	self:error("invalid command")
end

function Console:msg(msg)

	self.text:set_text(self.text:get_text()..tostring(msg))
end

function Console:error(msg)

	self.text:set_text(self.text:get_text().."Error: "..tostring(msg).."\n")
end

function Console:init_gui()

	local gui = {

		type = { GUI.WindowBox, "Console" },

		{ type = GUI.HBoxContainer,
			{ type = GUI.TextEdit,
				
				name = "text",
				init = function(widget)
				
					widget:set_minimum_size(GUI.Point(200,200))
				end,
			},
			{ type = GUI.VBoxContainer,
				name = "var_panel",
			},
		},
		{ type = GUI.LineEdit,
			
			name = "cmdline",
			
			init = function(widget)
			
				widget.text_enter_signal:connect(self, "entered_command")
			end,
		},
	}
	
	local box = load_gui(self, gui)
	
	self:set_root_frame(box)

	self.key_unhandled_signal:connect(self, "unhandled_key")
	
	self.var_buttons = {}
	self:update_var_buttons()
end

function Console:update_var_buttons()

	for count = 0,16 do

		local var = GlobalVars:get_singleton():get("console_var_"..count)
		if var then
			self.updating_vars = true
			
			if not self.var_buttons[count] then
				self.var_buttons[count] = self.var_panel:add(GUI.Button:new())
				self.var_buttons[count]:set_toggle_mode(true)
				self.var_buttons[count].toggled_signal:connect(self, "var_toggled", count)
			end
	
			self.var_buttons[count]:set_text(var)
			self.var_buttons[count]:set_pressed(GlobalVars:get_singleton():get(var))
			
			self.updating_vars = false
		else
			if self.var_buttons[count] then
				self.var_buttons[count]:delete()
				self.var_buttons[count] = nil
			end
		end
	end
end

function Console:var_toggled(on, count)
	if self.updating_vars then
		return
	end
	local var = GlobalVars:get_singleton():get("console_var_"..count)
	GlobalVars:get_singleton():set(var, on)
end

function Console:__parent_args__(window)

	return window, GUI.Window.MODE_POPUP, GUI.Window.SIZE_CENTER
end

function Console:__init__(window)

	instance = self

	self.main_window = window
	self:init_gui()
	self:hide()
	
	self.dir_access = DirAccess:create()
end

return Console
