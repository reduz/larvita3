class ("SplineEditor", GUI.VBoxContainer)

function SplineEditor:clear()

	for k,v in pairs(self.cursors) do
	
		local p = k:get_parent()
		if k then
			k:get_parent():remove_child(k)
		end
	end
	self.cursors = {}
end

function SplineEditor:hide()

	
end

function SplineEditor:cursor_changed(ev, cursor)

	if ev ~= Node.EVENT_MATRIX_CHANGED then
		return
	end
	
	local pos = cursor:get_local_matrix():get_translation()
	self.node:get_spline():set_point(self.cursors[cursor], pos)
end

function SplineEditor:add_cursor(i)

	local spline = self.node:get_spline()
	i = i or spline:get_point_count() - 1

	local cursor = CursorNode:new()
	local mat = Matrix4()
	mat:set_translation(spline:get_point(i))
	cursor:set_local_matrix(mat)
	self.node:add_child(cursor)
	self.cursors[cursor] = i

	cursor.event_signal:connect(self, self.cursor_changed, cursor)
end

function SplineEditor:update()

	self:clear()

	local spline = self.node:get_spline()
	
	for i=0,spline:get_point_count() - 1 do

		self:add_cursor(i)
	end
	
	self.node:set("display", true)
end

function SplineEditor:load(node)

	self.node = node

	self:update()	
end

function SplineEditor:add_point_pressed()

	local mat = self.editor.view.cursor:get_global_matrix()
	mat = self.node:get_global_matrix():inverse() * mat
	local p = self.node:get_spline():add_point(mat:get_translation())
	
	self:add_cursor(p)
end

function SplineEditor:close_spline()

	local spline = self.node:get_spline()
	if spline:get_point_count() < 3 then
		return
	end
	
	spline:add_point(spline:get_point(0))
	self:add_cursor()
end

function SplineEditor:cursor_selected(cursor)

	if not self.cursors[cursor] then
		return
	end
end

function SplineEditor:cursor_released(cursor, cancel)

	if not self.cursors[cursor] then
		return
	end

end

function SplineEditor:init_gui()

	local gui = {
		type = false,
	
		{ type = { GUI.Button, "add_point" },
			
			name = "add_button",
			init = function(widget)
			
				widget.pressed_signal:connect(self, self.add_point_pressed)
			end,
		},
		
		{ type = { GUI.Button, "Close" },
			
			name = "close_button",
			init = function(widget)
			
				widget.pressed_signal:connect(self, self.close_spline)
			end,
		},
	}
	
	load_gui(self, gui, self)
end

function SplineEditor:__parent_args__()

end

function SplineEditor:__init__(editor)

	self.editor = editor
	self.cursors = {}
	
	self.editor.cursor_selected:connect(self, self.cursor_selected)
	self.editor.cursor_released:connect(self, self.cursor_released)
	
	self:init_gui()
end

return SplineEditor
