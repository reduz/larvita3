class("KeyDialog", GUI.Window)

function KeyDialog:ok_pressed()

	if self.name:get_text() ~= "" then
	
		self.edited_signal:emit(self.name:get_text(), self.variant:get_value())
	end
	self:hide()
end

function KeyDialog:show(name, val)

	self.name:set_text(name or "")
	self.variant:edit(val, nil, nil, true)
	
	GUI.Window.show(self)
end

function KeyDialog:init_gui()

	local gui = {
		
		type = GUI.VBoxContainer,

		{ type = GUI.HBoxContainer,
			"Name:",
			{ type = GUI.LineEdit,
				
				name = "name",
			},
			"Value:",
			{ type = VariantEditor,
	
				name = "variant",
			},
		},
		
		{ type = GUI.HBoxContainer,
			
			{ type = { GUI.Button, "Ok" },
				name = "ok_button",
				
				init = function(widget)
				
					widget.pressed_signal:connect(self, self.ok_pressed)
				end,
			},
			{ type = { GUI.Button, "Cancel" },
				name = "cancel_button",
				
				init = function(widget)
				
					widget.pressed_signal:connect(self, self.hide)
				end,
			},
		},
	}
	
	local box = load_gui(self, gui)
	self:set_root_frame(box)
end

function KeyDialog:__parent_args__(parent)

	return parent, GUI.Window.MODE_POPUP, GUI.Window.SIZE_TOPLEVEL_CENTER
end

function KeyDialog:__init__(parent)

	self.edited_signal = LuaSignal:new(2)
	
	self:init_gui()
end

-----------------------------------------------------------------------------------------


class("AnimEditor", GUI.VBoxContainer)

function AnimEditor:clear()

	self.anim_combo:clear()
	self.track_combo:clear()
	self.key_list:get_list():clear()
	self.animation = nil
end

function AnimEditor:load(node)

	self:clear()

	self.node = node
	local set = node:get_animation_set()
	if not set then
		return
	end

	self.anim_combo:clear()
	self.anim_list = {}
	local list = List_IAPI__PropertyInfo_()
	set:get_property_list(list)
	local I = list:begin()
	while I do

		local name = string.match(I:get().path, "^set/(.*)$")
		if name then
			table.insert(self.anim_list, name)
			self.anim_combo:add_string(name)
		end
		I = I:next()
	end
end

function AnimEditor:animation_selected(n)

	local anim = self.node:get_animation_set():get_animation(self.anim_list[n+1])
	if not anim then
		return
	end
	self.animation = anim
	self.animation_name = self.anim_list[n+1]

	self:reload_tracks()
	
	self.time_slider:get_range():set_min(0)
	self.time_slider:get_range():set_max(anim:get_length())
	self.time_slider:get_range():set_step(0.001)
	self.time_slider:get_range():set(0)
	
	self.node:force_animation(self.anim_list[n+1])
end

function AnimEditor:reload_tracks(sel)

	self.track_combo:clear()
	self.track_list = {}
	for i=0, self.animation:get_track_count()-1 do
		self.track_combo:add_string(self.animation:get_track_name(i))
		if i == sel then
		
			self.track_combo:select(i)
		end
	end
end

function AnimEditor:track_selected(n)

	self.key_list:get_list():clear()
	self.frames = {}
	self.current_track = n
	self.current_value = nil

	self:reload_keys()
end

function AnimEditor:reload_keys(sel)

	self.key_list:get_list():clear()

	local keys = self.animation:get("tracks/"..self.current_track.."/value_keys")

	if not keys then
		return
	end

	if sel then
		if sel < 0 then
			sel = 0
		elseif sel >= keys:size()/3 then
			sel = (keys:size()/3) - 1
		end
	end
	
	local count = 0
	for i=0, keys:size() - 1, 3 do
	
		local key = { offset = keys:array_get(i), name = keys:array_get(i+1), value = keys:array_get(i+2) }
		self.key_list:get_list():add_string(key.offset.." - "..key.name..": "..tostring(key.value))
		self.frames[i] = key

		if count == sel then
		
			self.key_list:get_list():select(count, true)
		end
		count = count + 1
	end
end


function AnimEditor:value_selected(n)

	self.current_value = n
end

function AnimEditor:add_track(n)

	if not self.animation then
		return
	end

	if not n then
		self.string_pending = self.add_track
		self.string_dialog:show("Track name", "")
		return
	end
	if n == "" then
		return
	end
	
	local t = self.animation:add_track(n)
	self.node:redo_animation_link(self.animation_name)
	self:reload_tracks(t)
	
end

function AnimEditor:string_input(s)

	if self.string_pending then
		self.string_pending(self, s)
		self.string_pending = nil
	end
end

function AnimEditor:remove_keyframe(r)

	if not self.current_value or not self.animation then
		return
	end

	if r and r ~= "" and string.lower(r) ~= "no" then
		
		self.animation:remove_value_key(self.current_track, self.current_value)
		self:reload_keys(self.current_value)
	else

		self.string_pending = self.remove_keyframe
		self.string_dialog:show("Remove frame "..self.current_value.."?", "Yes")
		return
	end
end

function AnimEditor:add_keyframe()

	self.edit_mode = "add"
	self.key_dialog:show("", nil)
end

function AnimEditor:edit_keyframe()

	if not self.current_value then
		return
	end
	self.edit_mode = "edit"
	self.key_dialog:show(self.frames[self.current_value].name, self.frames[self.current_value].value)
end

function AnimEditor:key_edited(name, value)

	local frame = self.frames[self.current_value]
	if self.edit_mode == "add" then
	
		local key = self.animation:add_value_key(self.current_track, self.time_slider:get_range():get(), name, value)
		self:reload_keys(key)
	
	elseif self.edit_mode == "edit" then

		local key = self.animation:add_value_key(self.current_track, frame.offset, name, value, true)
		self:reload_keys(key)
	end
end

function AnimEditor:position_changed(n)

	self.node:set_current_animation_time(n)
end

function AnimEditor:init_gui()

	local gui = {
		
		type = false,
		
		{ type = GUI.HBoxContainer,
			
			"Animation:",
			{ type = GUI.ComboBox,
				
				name = "anim_combo",
				add_args = {1},
				init = function(widget)
				
					widget.selected_signal:connect(self, self.animation_selected)
				end,
			},
		},
		
		{ type = GUI.ScrollBar,
			
			name = "time_slider",
			init = function(widget)
			
				widget:get_range().value_changed_signal:connect(self, self.position_changed)
			end,
		},
		
		{ type = GUI.HBoxContainer,
			
			"Time:",
			{ type = GUI.SpinBox,
				
				name = "time_spin",
				init = function(widget)
				
					widget:set_range(self.time_slider:get_range())
				end,
			},
		},
		
		{ type = GUI.HBoxContainer,
			
			"Track:",
			{ type = GUI.ComboBox,
				name = "track_combo",
				add_args = {1},
				
				init = function(widget)
				
					widget.selected_signal:connect(self, self.track_selected)
				end,
			},
			
			{ type = { GUI.Button, "Add" },
				name = "add_track_button",
				
				init = function(widget)
				
					widget.pressed_signal:connect(self, self.add_track)
				end,
			},
		},
		
		{ type = GUI.ListBox,
			
			name = "key_list",
			add_args = {1},
			
			init = function(widget)
			
				widget:get_list().selected_signal:connect(self, self.value_selected)
			end,
		},
		
		{ type = GUI.HBoxContainer,

			{ type = { GUI.Button, "Remove" },

				name = "remove_keyframe_button",
				init = function(widget)
				
					widget.pressed_signal:connect(self, self.remove_keyframe)
				end,
			},
			
			{ type = { GUI.Button, "Edit" },

				name = "edit_keyframe_button",
				init = function(widget)
				
					widget.pressed_signal:connect(self, self.edit_keyframe)
				end,
			},
			
			{ type = { GUI.Button, "Add" },

				name = "add_keyframe_button",
				init = function(widget)
				
					widget.pressed_signal:connect(self, self.add_keyframe)
				end,
			},
		},
	}
	
	load_gui(self, gui, self)
	
	self.string_dialog = GUI.StringInputDialog:new(self.editor)
	self.string_dialog.entered_string_signal:connect(self, self.string_input)
	
	self.key_dialog = KeyDialog:new(Main:get_singleton():get_main_loop():get_gui_main_window())
	self.key_dialog.edited_signal:connect(self, self.key_edited)
end

function AnimEditor:__parent_args__()

end

function AnimEditor:__init__(editor)

	self.editor = editor
	
	self:init_gui()
end

return AnimEditor

