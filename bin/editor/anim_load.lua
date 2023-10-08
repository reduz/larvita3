class "AnimTest"

require "editor/dae_import.lua"

function AnimTest:process()

	self.timer = self.timer - self:get_scene_main():get_process_interval()
	if self.timer <= 0 then
		self.fps:set_text("fps: "..self.frame_count)
		--print("fps: "..self.frame_count)
		self.frame_count = 0
		self.timer = self.timer + 1000000
	end
	self.frame_count = self.frame_count + 1

	local elapsed = self:get_scene_main():get_process_interval()
	self.player:seek_relative(elapsed)

	for i=0,self.anim:get_track_count()-1 do
	
		local state = self.player:get_track_state(i)
		local mat = Matrix4()
		mat:load_rotate(state.rot)
		mat:set_translation(state.pos)
		--self.channels[name]:set_pose_matrix(mat)
		self.skeleton:set_bone_pose(self.bones[i], mat)
	end
end

function AnimTest:load_cubito_skeleton()

	local skel = {}
	self.channels = {}
	skel[-1] = DummyNode:new()
	self.skeleton:get_child_by_index(0):add_child(skel[-1])
	print("type is ", self.skeleton:get_child_by_index(0):get_type())
	for i=0,self.skeleton:get_bone_count()-1 do
	
		local p = self.skeleton:get_bone_parent(i)
		local dummy = DummyNode:new()
		self.channels[self.skeleton:get_bone_name(i)] = dummy
		skel[i] = dummy
		skel[p]:add_child(dummy)
		dummy:set_matrix(self.skeleton:get_bone_rest(i))
		local mesh = MeshNode:new()
		dummy:add_child(mesh)
		--mesh:set_mesh(self.cubito)
		mesh:set_matrix(Matrix4():scale(0.05, 0.05, 0.05))
		local pos = dummy:get_global_matrix():get_translation()
	end
	skel[-1]:set_matrix(Matrix4():scale(1, 1, 1))
end

function AnimTest:export_pressed(fname)

	if not fname then
		
		self.export_dialog:show(GUI.FileDialog.MODE_SAVE)
		return
	end
	
	local tree = TreeFactory:get_singleton():get_saver_tree(fname)
	if not tree then
		-- show message
		self.error_dialog:show("Error: file format not recognized (use .xml or .cxt)")
		return
	end
	
	AnimationLoader:save(self.anim, tree)
	tree:close()
	tree:delete()
end

function AnimTest:load_gui()

	local gui = 
		{ type = GUI.VBoxContainer,
			{ type = GUI.HBoxContainer,
			
				{ type = GUI.Button,
					name = "export",
					init = function(widget, parent, root)
					
						widget:set_text("Export")
						widget.pressed_signal:connect(self, self.export_pressed)
					end,
				},
				
				{ type = GUI.Label,
				
					name = "fps",
				}
			}
		}
	local box = load_gui(self, gui)
	local loop = Main:get_singleton():get_loop()
	loop:get_gui_main_window():set_root_frame(box)
	
	self.export_dialog = GUI.FileDialog:new(loop:get_gui_main_window())
	self.export_dialog.file_activated_signal:connect(self, self.export_pressed)
	
	self.error_dialog = GUI.QuestionInputDialog:new(loop:get_gui_main_window())
	self.error_dialog:add_button(0, "Ok")
end

function AnimTest:load_node(path)

	local tree = TreeFactory:get_singleton():get_tree(path)
	local scene = SceneNode:new()
	scene:set_resource_path(path)
	self:get_scene_main():get_scene_store():load(scene, tree)

	return scene
end

function AnimTest:__init__()

	local render = self:get_scene_main():get_renderer()

	local node
	local nodepath = arg[3] or arg[2]
	if string.find(string.lower(nodepath), "dae$") then
	
		node = import_dae(nodepath, render)
	else
		node = self:load_node(nodepath)
	end
	self:add_child(node)

	local function find_skel(node)
		if node:get_type() == TYPE_SKELETON then
			return node
		end
		for i=0,node:get_child_count()-1 do
		
			local c = node:get_child_by_index(i)
			c = find_skel(c)
			if c then
				return c
			end
		end
		
		return nil
	end
	self.skeleton = find_skel(node)
	if not self.skeleton then
		print("error: skeleton not found")
	end

	self.player = AnimationPlayer()
	self.anim = dae_import_animation(arg[2], render, self.skeleton)
	self.anim:set_loops(true)

	self.bones = {}
	for i=0,self.anim:get_track_count()-1 do
	
		self.bones[i] = self.skeleton:get_bone_id(self.anim:get_track_name(i))
	end

	self.player:load(self.anim)

	self:load_gui()
	
	self.timer = 1000000 -- 1 second
	self.frame_count = 0
end


return AnimTest
