collectgarbage("setpause", 80)
collectgarbage("setstepmul", 200)

require "loop.lua"
require "main_window.lua"

MemoryPoolStatic:get_singleton():set_debug_level(MemoryPoolStatic.LEVEL_SUMMARY)

local vm = VideoMode()
vm.width = 720
vm.height = 560
vm.fullscreen = false
vm.resizable = true

local n = 1
while arg[n] do

	if not string.match(arg[n], "^-") then
	
		n = n+1
	else

		local s = string.match(arg[n], "^%-size=(.*)")
		if s then

			s = split(s, "x")
			vm.width = tonumber(s[1])
			vm.height = tonumber(s[2])
			vm.fullscreen = false
			vm.resizable = true
		end
		
		if arg[1] == "-fs" then
			vm.fullscreen = true
		end
		table.remove(arg, n)
	end
end

local main = Main:create(vm)
local loop = Loop:new()

main:set_main_loop(loop)

local window = EditorMainWindow:new_instance(loop:get_gui_main_window(), arg)

main:run()

window._signal_handler:clear_connections()

loop:delete()
loop = nil

--main:delete()

return 0

