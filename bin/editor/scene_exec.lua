collectgarbage("setpause", 95)

require "run_loop.lua"

if not arg[1] then
	print("Usage: "..arg[0].." scene")
	return 1
end

MemoryPoolStatic:get_singleton():set_debug_level(MemoryPoolStatic.LEVEL_SUMMARY)

local main = Main:create()
loop = RunLoop:new(arg[1])

main:set_loop(loop)

main:run()

main:delete()
loop:delete()

return 0

