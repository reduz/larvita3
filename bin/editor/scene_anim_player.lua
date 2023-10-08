class "SceneAnimPlayer"

function SceneAnimPlayer:add_skeleton(skel)

	for i=0,skel:get_bone_count()-1 do
		self.bones[skel:get_bone_name(i)] = { type = "bone", skel = skel, id = i, parent = skel:get_bone_parent(i) }
	end
	table.insert(self.skels, skel)
end

function SceneAnimPlayer:get_current_anim()

	return self.animation
end

function SceneAnimPlayer:current_anim_done()

	if not self.animation then
		return true
	end
	return self.player:done()
end

function SceneAnimPlayer:load(anim, blend_time)

	blend_time = blend_time or 2
	if self.animation == anim then return end
	local blend
	if self.animation then blend = true end
	
	self.animation = anim
	if not anim then return end
	
	self.player:load(anim, blend, blend_time)
end

function SceneAnimPlayer:restart()

	self.player:seek(0)
end

function SceneAnimPlayer:seek_relative(time)

	if not self.animation then return end
	self.player:seek_relative(time)

	for k,v in pairs(self.bones) do
	
		if v.type == "bone" then
			v.skel:set_bone_pose(v.id, Matrix4.IDENTITY)
		end
	end
	
	local mat = Matrix4()
	for i=0, Animation:get_track_count(self.animation)-1 do
	
		local state = self.player:get_track_state(i)
		mat:load_rotate(state.rot)
		mat:set_translation(state.pos)

		local name = Animation:get_track_name(self.animation, i)
		local bone = self.bones[name]
		if bone then
			if bone.type == "bone" then

				bone.skel:set_bone_pose(bone.id, mat)
			end
			if bone.type == "node" then

				bone.node:set_pose_matrix(mat)			
			end
		else
			--print("bone not found", name)
		end
	end
	
	for k,v in ipairs(self.skels) do
	
		v:process()
	end
end

function SceneAnimPlayer:clear()

	self.bones = {}
	self.skels = {}
end

function SceneAnimPlayer:__init__()

	self:clear()
	self.player = AnimationPlayer()
end

return SceneAnimPlayer
