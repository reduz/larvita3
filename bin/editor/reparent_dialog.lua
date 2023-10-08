class("ReparentDialog", GUI.Lua__Window)

function ReparentDialog:get_node_selected()

	local function traverse(it)
	
		if it:is_selected(0) then

			if not it.node then print("OH, NOH, BUGH!!!! it.node NOT DERE") end
			return it.node
		end
		
		local child = it:get_childs()
		while child do
		
			local sel = traverse(child)
			if sel then
				return sel
			else
				child = child:get_next()
			end
		end
		
		return nil
	end
	
	return traverse(self.tree:get_root_item())
	
end

function ReparentDialog:node_selected()

	local node = self:get_node_selected()

	self:hide()
	if not node then
		print "OH, NOH; BUGH!!! NO NODE SELECTED!"
		return
	end
	
	self.new_parent_selected:emit(node)
end

function ReparentDialog:load_scene(scene, exclude_node)

	self.its = {}
	self.tree:clear()
	
	local function traverse(node, parent_item)
	
		if node == exclude_node then
			return
		end
		
		local it = self.tree:create_item(parent_item)
		it:set_text(0,node:get_name())
		it.node = node
		table.insert(self.its,it)
		
		if node:get_owner() ~= scene and node ~= scene then
			it:set_custom_color(0,GUI.Color(255, 0, 0))
		end

		local childs = node:get_child_count()
		for i=0,childs-1 do
		
			traverse(node:get_child_by_index(i), it)
		end
	end
	
	traverse(scene)
end

function ReparentDialog:show(scene, node)

	self:load_scene(scene, node)
	
	GUI.Window.show(self)
end

function ReparentDialog:init_gui()

	local gui = {
	
		type = { GUI.WindowBox, "Pick new parent" },

		{ type = GUI.ScrollBox,
		
			init = function(self)
				self:set_expand_h(true)
				self:set_expand_v(true)
			end,

			add_args = {1},
			
			{ type = GUI.Tree,
				
				name = "tree",

				add = function(widget, parent)
					parent:set_frame(widget)
				end,
			},
		},
		
		{ type = { GUI.Button, "OK" },
			
			name = "button_ok",
		},
	}
	
	local box = load_gui(self, gui)
	self:set_root_frame(box)
	
	self.button_ok.pressed_signal:connect(self, self.node_selected)
end

function ReparentDialog:__parent_args__(parent)

	return parent, GUI.Window.MODE_POPUP, GUI.Window.SIZE_TOPLEVEL_CENTER
end

function ReparentDialog:__init__(parent)

	self.main_window = parent

	self:init_gui()

	self.new_parent_selected = LuaSignal:new()
end


