class("ImportSceneDialog", GUI.Lua__Window)

function ImportSceneDialog:init_gui()

	local gui = {
	
		type = {GUI.WindowBox, "Import Scene"},
		
		{ type = GUI.HBoxContainer,
		
			{ type = GUI.List,
				name = "list",
				add_args = {1},
			},

			{ type = GUI.VBoxContainer,

				{ type = {GUI.CheckButton, "Not Visible"},
					name = "invisible",
				},
				{ type = {GUI.CheckButton, "Use as bsp volume"},
					name = "bsp",
				},


				{ type = GUI.HBoxContainer,

					{ type = { GUI.Button, "Ok"},

						name = "button_ok",
					},
					{ type = {GUI.Button, "Cancel"},

						name = "button_cancel",
					},
				},
			},
		},
	}
	
	local container = load_gui(self, gui)

	self.invisible.toggled_signal:connect(self, self.button_toggled, "invisible")
	self.bsp.toggled_signal:connect(self, self.button_toggled, "bsp")
	
	self.button_ok.pressed_signal:connect(self, self.ok_pressed)
	self.button_cancel.pressed_signal:connect(self, self.cancel_pressed)

	self.list.selected_signal:connect(self, self.node_selected)
	
	self:set_root_frame(container)
end

function ImportSceneDialog:button_toggled(toggled, name)

	local node = self.nodes[self.list:get_selected_string()]
	
	node.flags[name] = toggled
end

function ImportSceneDialog:node_selected()

	local nodestr = self.list:get_selected_string()
	local node = self.nodes[nodestr].node
	
	self.bsp:set_checked(self.nodes[nodestr].flags.bsp)
end

function ImportSceneDialog:load_scene(scene, flags)

	self:clear()

	self.scene = scene
	
	local function add(node)
		if node ~= scene and node:get_owner() ~= scene then
			return
		end
		local path = Node:get_node_path(node, scene, true)
		self.list:add_string(path)
		local rel_path = Node:get_node_path(node, scene, false)

		local n = {flags = flags[rel_path], node = node}
		self.nodes[path] = n
		if n.flags then
			for k,v in ipairs(n.flags) do
				n.flags[v] = true
			end
		else
			n.flags = {}
		end
	end
	
	traverse_scene(scene, add)
	self.list:select(0)
end

function ImportSceneDialog:clear_args()

	self.invisible:set_checked(false)
	self.bsp:set_checked(false)
end

function ImportSceneDialog:ok_pressed()

	if not self.scene then
		self:hide()
		return
	end
	
	for k,v in pairs(self.nodes) do
	
		if v.node:get_type() == TYPE_MESH and v.flags.bsp then
		
			-- make it a bsp node
			NodeHelpers:create_bsp_collision_nodes_from_visual_node( v.node );
		end
	end

	if self.scene then
		self.import_done(self.scene) -- emit signal
	end
	
	self:hide()
end

function ImportSceneDialog:cancel_pressed()

	self:clear()
	self:hide()
end

function ImportSceneDialog:clear()

	self.list:clear()
	self:clear_args()
	self.nodes = {}
	self.scene = nil
end

function ImportSceneDialog:__parent_args__(parent)

	return parent, GUI.Window.MODE_POPUP, GUI.Window.SIZE_TOPLEVEL_CENTER
end

function ImportSceneDialog:__init__(parent)

	self.import_done = LuaSignal()
	
	self:init_gui()
end

return ImportSceneDialog
