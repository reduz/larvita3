class("OpenSceneDialog", GUI.Window)

local SCENE_TYPE = "SceneNode"

function OpenSceneDialog:show(filter)

	filter = filter or SCENE_TYPE

	self:clear()
	
	local list = List_IAPI_DB__ListingElement_()
	
	IAPI_Persist:get_singleton():get_db():get_listing(list, IAPI_DB.LISTING_TYPE, filter)

	self.db_ids = {}
	local I = list:begin()
	while I do

		local id = I:get()
	
		self.db_ids[self.list:get_size()] = id.id
		self.list:add_string(id.id.." - "..id.path)
	
		I = I:next()
	end
	
	GUI.Window.show(self)
end

function OpenSceneDialog:clear()

	self.list:clear()
	self.db_ids = nil
end

function OpenSceneDialog:hide()

	self:clear()
	GUI.Window.hide(self)
end

function OpenSceneDialog:ok_pressed()

	local n = self.list:get_selected()
	if n < 0 then
		return
	end
	
	self.scene_selected_signal:emit(self.db_ids[n])
	self:hide()
end

function OpenSceneDialog:init_gui()

	local gui = {
		
		type = {GUI.WindowBox, "Import Scene"},

		{ type = GUI.List,
			
			name = "list",
			add_args = {1},
		},
		
		{ type = GUI.HBoxContainer,
			
			{ type = { GUI.Button, "Ok" },
				
				init = function(widget)
				
					widget.pressed_signal:connect(self, self.ok_pressed)
				end,
			},
			
			{ type = { GUI.Button, "Cancel" },

				init = function(widget)
				
					widget.pressed_signal:connect(self, self.hide)
				end,
			},
		},
	}
		
	local container = load_gui(self, gui)

	self:set_root_frame(container)

end

function OpenSceneDialog:__parent_args__(parent)

	return parent, GUI.Window.MODE_POPUP, GUI.Window.SIZE_TOPLEVEL_CENTER
end

function OpenSceneDialog:__init__(parent)

	self.scene_selected_signal = LuaSignal:new(1)
	
	self:init_gui()
end


return OpenSceneDialog
