class("MemoryDebugWindow", GUI.Window)

function MemoryDebugWindow:init_gui()

	local gui = {
	
		type = { GUI.WindowBox, "Memory Debug" },
		
		{ type = GUI.MemoryDebugWidget,
		
			name = "memory_debug",
			
			add_args = { 1 },
		},
	}
	
	local box = load_gui(self, gui)
	self:set_root_frame(box)
end

function MemoryDebugWindow:__parent_args__(parent)
	
	return parent, GUI.Window.MODE_POPUP, GUI.Window.SIZE_TOPLEVEL_CENTER
end

function MemoryDebugWindow:__init__(parent)

	self:init_gui()
end

return MemoryDebugWindow

