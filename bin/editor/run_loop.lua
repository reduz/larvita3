require "helpers.lua"

local JOY_DEAD_ZONE = 64

class("CameraMotion",Lua__MotionObject)

function CameraMotion:get_type()

	return MotionObject.TYPE_SPHERE;
end

function CameraMotion:get_sphere()

	return 0.3
end

function CameraMotion:get_ellipsoid()

	return Vector3()
end

function CameraMotion:get_desired_position()

	return self.camera:get_matrix():get_translation()
end


function CameraMotion:get_desired_motion_vector()

	return self.motion_vector;
end

function CameraMotion:motion(new_pos,collided,result)

	local m = self.camera:get_matrix()
	m:set_translation( new_pos )
	self.camera:set_matrix(m)

	if (collided) then
		local p = PrimitiveListNode.Primitive()
		p.vertices[0]=result.contact_point
		p.vertices[1]=result.contact_point+result.contact_plane:get_normal() * 0.5
		p.colors[0]=Color(255,0,255)
		p.colors[1]=Color(255,255,255)

		self.primitives:add_primitive( p )
	end

	return Vector3()
end

function CameraMotion:__parent_args__()

end

function CameraMotion:__init__(camera)

	self.motion_vector=Vector3()
	self.camera=camera
end


class("RunLoop", Lua__MainLoop)

local max_mem = 1024.0 -- 1mb

function RunLoop:iteration(elapsed)

	local ev = InputEvent()
	-- event loop
	while self:events_pending() do

		self:pop_event(ev)

		if ev.type == InputEvent.KEY then
			-- nothing
			self.key_status[ev.key.scancode] = ev.key.pressed
		end
		if ev.type == InputEvent.MOUSE_BUTTON then

			if ev.mouse_button.button_index == GUI.BUTTON_LEFT then

				self.camera_grab = ev.mouse_button.pressed
			end
		end

		if ev.type == InputEvent.MOUSE_MOTION then

			if self.camera_grab then

				local mat = self.current_camera:get_matrix()
				self.cam_rot_y = self.cam_rot_y - (ev.mouse_motion.relative_x * (math.pi / 180.0))
				self.cam_rot_x = self.cam_rot_x - (ev.mouse_motion.relative_y * (math.pi / 180.0))
			end
		end

		if ev.type == InputEvent.JOYSTICK_BUTTON then

			self.pad_status[ev.joy_button.button_index] = ev.joy_button.pressed
		end
		if ev.type == InputEvent.JOYSTICK_MOTION then

			self.joy_status[ev.joy_motion.axis] = ev.joy_motion.axis_value
		end

		if self.event_recipient then
			self.event_recipient:input_event(ev)
		end
	end

	self:update_keys(elapsed)
	--orthonormalize_node(self.current_camera)

	self:update_camera()
	self.current_camera:set_as_current()

	self.scene_main:process(elapsed)


	local usage = collectgarbage("count")
	--pause = max_mem / usage
	--collectgarbage("setpause", pause * 100)

--	self.mem_timer = self.mem_timer - elapsed
--	if self.mem_timer <= 0 then
--		collectgarbage("collect")
--		--print("lua mem usage is "..collectgarbage("count").."\r")
--		self.mem_timer = 5000
--	end

	return self.quit
end

function RunLoop:update_camera()

	local mat = Matrix4()
	mat:set_translation(self.current_camera:get_global_matrix():get_translation())

	mat:rotate_y(self.cam_rot_y)
	mat:rotate_x(self.cam_rot_x)

	self.current_camera:set_global_matrix(mat)

end

local pad_a = 0
local pad_b = 1
local pad_l = 2
local pad_r = 3
local pad_x = 4
local pad_y = 5



function RunLoop:update_keys(elapsed)

	elapsed = elapsed / 1000.0

	local speed = 0.005

	local mat = self.current_camera:get_matrix()

	local old_translation = mat:get_translation();

	if self.key_status[KEY_w] or (self.joy_status[5] and self.joy_status[5] < 0) then
		mat:translate(0, 0, -speed * elapsed)
		self.current_camera:set_matrix(mat)
	end

	if self.key_status[KEY_s] or (self.joy_status[5] and self.joy_status[5] > 0) then
		mat:translate(0, 0, speed * elapsed)
		self.current_camera:set_matrix(mat)
	end

	if self.key_status[KEY_a] or (self.joy_status[4] and self.joy_status[4] < 0) then
		mat:translate(-speed * elapsed, 0, 0)
		self.current_camera:set_matrix(mat)
	end

	if self.key_status[KEY_d] or (self.joy_status[4] and self.joy_status[4] > 0) then

		mat:translate(speed * elapsed, 0, 0)
		self.current_camera:set_matrix(mat)
	end

	--[[
	if (self.joy_status[0] ~= 0 or self.joy_status[1] ~= 0) then
 		if self.joy_status[0] and self.joy_status[0] ~= 0 and Math:absf(self.joy_status[0]) > JOY_DEAD_ZONE then
		
			mat:translate(0, 0, speed * (tofloat(self.joy_status[0]) / JOY_AXIS_MAX) * elapsed)
		end
		if self.joy_status[1] and self.joy_status[1] ~= 0 and Math:absf(self.joy_status[1]) > JOY_DEAD_ZONE then

			mat:translate(speed * (tofloat(self.joy_status[1]) / JOY_AXIS_MAX) * elapsed, 0, 0)
		end
		self.current_camera:set_matrix(mat)
	end
	--]]

	if self.pad_status[pad_a] then
		self.cam_rot_y = self.cam_rot_y - ( (elapsed/1000.0) * math.pi )
	end
	if self.pad_status[pad_l] or self.pad_status[pad_y] then
		self.cam_rot_y = self.cam_rot_y + ( (elapsed/1000.0) * math.pi )
	end

	if self.pad_status[pad_b] then
		self.cam_rot_x = self.cam_rot_x - ( (elapsed/1000.0) * math.pi )
	end
	if self.pad_status[pad_r] or self.pad_status[pad_x] then
		self.cam_rot_x = self.cam_rot_x + ( (elapsed/1000.0) * math.pi )
	end

	local motion_vector = self.current_camera:get_matrix():get_translation()-old_translation;
	local mat = self.current_camera:get_matrix();
	mat:set_translation( old_translation )
	self.current_camera:set_matrix(mat)
end


function RunLoop:load_scene()

	local scene = SceneNode:new()
	scene:set_database_ID(self.scene_id)

	--local rp = strip_resource_path(self.scene_fname)
	--if not rp then
	--	print ("invalid path: "..self.scene_fname.." is outside resource path")
	--	return
	--end

	print("instance scene", self.scene_id)

	scene:set_name("new scene")
	self.scene_main:get_scene_root():add_child(scene)

	if self.scene_main:get_scene_store():load_scene(scene) ~= OK then
		print("error loading scene "..self.scene_id)
	end

	self.scene = scene
end

function RunLoop:setup_camera()

	self.camera = CameraNode:new()
	local cmat = Matrix4()
	cmat:translate(0, 0.5, 3)
	self.camera:set_matrix(cmat)

	local proj = Matrix4()
	proj:set_projection( 45.0, 4.0/3.0, 0.001, 100.0 )
	--proj:set_frustum(-0.05, 0.05, -0.05, 0.05, 0.05, 100)
	self.camera:set_projection_matrix(proj)

	self.scene_main:get_scene_root():add_child(self.camera)

	self.current_camera = self.camera

	---[[
	self.light = self:get_render():light_create(Render.LIGHT_SUN)
	self:get_render():light_set_color( self.light, Render.LIGHT_DIFFUSE, Color(255, 255, 255))
	self.scene_main:get_global_lights():add_light(self.light)

	local lightm = Matrix4()
	lightm:rotate_z(math.pi*1.5);
	self.scene_main:get_global_lights():set_light_matrix(self.light,lightm)
	--]]
end

function RunLoop:request_quit()

	print "quit"
	self.quit = true
end

function RunLoop:finish()

	print "finish"
end

function RunLoop:register_for_events(obj)

	self.event_recipient = obj
end

function RunLoop:init()

	self.vi_visual = BasicIndexer()
	self.vi_collision = BasicIndexer()
	
	self.script_manager = ScriptManagerLua()

	self.database = DataBase_FS()
	self.database:set_db_path(GlobalVars:get_var("resource_path").."/db/")
	Main:get_singleton():get_loop():get_asset_manager():set_database(self.database)
	self.scene_store = SceneStore(self.database)

	self.scene_main = SceneMain(self.vi_visual, self.vi_collision, Main:get_singleton():get_renderer(), self.script_manager, self.scene_store)
	
	self:setup_camera()

	self:load_scene()
	print("lua mem usage is "..collectgarbage("count").."\r")
end

function RunLoop:__parent_args__()

end

function RunLoop:__init__(scene)

	self.scene_id = scene

	self.key_status = {}
	self.pad_status = {}
	self.joy_status = {}

	self.cam_rot_x = 0.0
	self.cam_rot_y = 0.0

	self.mem_timer = 5000

	self.project_path = os.getenv("PROJECT_PATH") or "testdata"
	GlobalVars:add_var("paths/resource", self.project_path)
end

return RunLoop

