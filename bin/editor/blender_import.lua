local texture_cache = {}

local function load_table(t)
	local ret = {}
	for k,v in ipairs(t) do
		if v.type == "dictionary" then
			ret[v.name] = load_table(v)
		elseif v.type == "resource" then
			ret[v.name] = v
		else
			ret[v.name] = v.value
		end
	end
	
	return ret
end

local function load_matrix(t)
	local mat = Matrix4()
	local idx = 1
	for i=0,2 do
		for j=0,2 do
		 	mat:set(j, i, t[idx])
			idx = idx + 1
		end
	end
	for i=0,2 do
		mat:set(3, i, t[idx])
		idx = idx + 1
	end

	print("loaded matrix with position", mat:get_translation())

	return mat
end

local function load_quaternion(q)
	return Quat(q[1], q[2], q[3], q[4])
	--return Quat(unpack(q, 1, 4))
end

local function load_texture(t, scene)

	if not t.path then
		return
	end
	if texture_cache[t.path] then
		return texture_cache[t.path]
	end

	local fname = t.path
	if string.find(t.path, "/") then
		fname = string.match(t.path, "/([^/]*)$")
	end
	if not string.match(string.lower(fname), "%.png$") then
		print("invalid texture "..fname)
		return
	end
	
	local db = IAPI_DB:get_singleton()
	if db and db:get_ID("textures/"..fname) ~= OBJ_INVALID_ID then
		return IAPI_Persist:get_singleton():load("textures/"..fname)
	end

	local tex = Renderer:get_singleton():create_texture()
	--print("base path is ", scene.base_path)
	local err = tex:load(scene.base_path.."/"..t.path)
	if err == OK then
		texture_cache[t.path] = tex
	end

	return tex
end

local function load_material(t, scene)

	local mat = Renderer:get_singleton():create_material()
	local vars = load_table(t)

	mat:set("flags/shaded", not vars["vars/unshaded"])
	mat:set("flags/visible", true)
	mat:set("flags/double_sided", false)
	mat:set("flags/wireframe", vars["vars/wireframe"])
	
	mat:set("vars/transparency", vars['vars/transparency'])
	--mat:set("vars/shininess", vars['vars/specular_exp'])
	local c = vars['color']
	mat:set("color/diffuse", Color(c[1] * 255, c[2] * 255, c[3] * 255, c[4] * 255))
	local spec = vars['vars/specular'] * 255
	mat:set("color/specular", Color(spec, spec, spec, 255))

	if vars['layers/base/texture'] then
		local tex = load_texture(vars['layers/base/texture'], scene)
		mat:set("textures/diffuse", tex)
	end
	if vars['layers/normal/texture'] then
		local tex = load_texture(vars['layers/normal/texture'], scene)
		mat:set("textures/normalmap", tex)
	end
	if vars['layers/specular/texture'] then
		local tex = load_texture(vars['layers/specular/texture'], scene)
		mat:set("textures/specular", tex)
	end
	
	return mat
end

local function convert_primitive(p)
	return p
end

local function load_mesh(t, scene)
	local r = Renderer:get_singleton()
	local mesh = r:create_mesh()
	local props = {}
	local max = 0
	local has_skel = false
	for k,v in ipairs(t.value) do
		if v.type == "dictionary" then
			props[v.name] = load_table(v)
		else
			props[v.name] = v.value
		end
		local surfn = string.match(v.name, "^surfaces/(%d+)/.*")
		if surfn and tonumber(surfn) > max then
			max = tonumber(surfn)
		end
	end
	for i=1,max do
		local f = props["surfaces/"..i.."/format"]
		local primitive = convert_primitive(f.primitive)
		local index_len = f.index_array_len or -1
		local array_len = f.array_len
		local surf = r:create_surface()
		local format = 0
		local arrays = {}
		if props["surfaces/"..i.."/vertex_array"] then
			local va = Variant(Variant.REAL_ARRAY, array_len * 3)
			for k,v in ipairs(props["surfaces/"..i.."/vertex_array"]) do
				va:array_set_real((k-1) * 3 + 0, v[1])
				va:array_set_real((k-1) * 3 + 1, v[2])
				va:array_set_real((k-1) * 3 + 2, v[3])
			end
			format = format + Surface.FORMAT_ARRAY_VERTEX
			arrays[Surface.ARRAY_VERTEX] = va
		end
		
		---[[
		if props["surfaces/"..i.."/normal_array"] then
			local na = Variant(Variant.REAL_ARRAY, array_len * 3)
			for k,v in ipairs(props["surfaces/"..i.."/normal_array"]) do
				local n = Vector3(v[1], v[2], v[3])
				n:normalize()
				na:array_set_real((k-1) * 3 + 0, n.x)
				na:array_set_real((k-1) * 3 + 1, n.y)
				na:array_set_real((k-1) * 3 + 2, n.z)
			end
			format = format + Surface.FORMAT_ARRAY_NORMAL
			arrays[Surface.ARRAY_NORMAL] = na
		end
		--]]

		---[[
		local layer = "surfaces/"..i.."/tex_uv_array"
		if props[layer] then
			local uva = Variant(Variant.REAL_ARRAY, array_len * 2)
			for k,v in ipairs(props[layer]) do
				uva:array_set_real((k-1) * 2 + 0, v[1])
				uva:array_set_real((k-1) * 2 + 1, v[2])
			end
			format = format + (2 ^ (Surface.ARRAY_TEX_UV_0))
			arrays[Surface.ARRAY_TEX_UV_0] = uva
		end
		--]]

		--[[
		if props["surfaces/"..i.."/color_array"] then
		
			local ca = Variant(Variant.INT_ARRAY, array_len)
			local c = Color()
			for k,v in ipairs(props["surfaces/"..i.."/color_array"]) do
				c.r = v[1] * 255
				c.g = v[2] * 255
				c.b = v[3] * 255
				c.a = v[4] * 255
				ca:array_set_int(k-1, c:to_32())
			end
			format = format + Surface.FORMAT_ARRAY_COLOR
			arrays[Surface.ARRAY_COLOR] = ca
		end
		--]]
		
		--[[
		if props["surfaces/"..i.."/tangent_array"] then
			local ta = Variant(Variant.REAL_ARRAY, array_len * 4)
			for k,v in ipairs(props["surfaces/"..i.."/tangent_array"]) do
				ta:array_set_real(k-1, v)
			end
			format = format + Surface.FORMAT_ARRAY_TANGENT
			arrays[Surface.ARRAY_TANGENT] = ta
		end
		--]]
		
		---[[
		if props["surfaces/"..i.."/weights_array"] and props["surfaces/"..i.."/bone_array"] then
			has_skel = true
			local wa = Variant(Variant.BYTE_ARRAY, array_len * 8)
			local weights = props["surfaces/"..i.."/weights_array"]
			local bones = props["surfaces/"..i.."/bone_array"]
			local total = 0
			for k,v in ipairs(weights) do

				wa:array_set_byte((k-1)*2 + 0, bones[k])
				wa:array_set_byte((k-1)*2 + 1, math.max(math.min(v * 255, 255), 0))
			end
			format = format + Surface.FORMAT_ARRAY_WEIGHTS
			arrays[Surface.ARRAY_WEIGHTS] = wa
		end
		--]]
		
		---[[
		if props["surfaces/"..i.."/index_array"] then
			local ia = Variant(Variant.INT_ARRAY, index_len)
			for k,v in ipairs(props["surfaces/"..i.."/index_array"]) do
				ia:array_set_int(k-1, v)
			end
			format = format + Surface.FORMAT_ARRAY_INDEX
			arrays[Surface.ARRAY_INDEX] = ia
		end
		--]]

		surf:create(primitive, format, 0, array_len, index_len)
		local mat = load_material(props["surfaces/"..i.."/material"], scene)
		surf:set_material(mat)
		
		for k,v in pairs(arrays) do
			surf:set_array(k, v)
		end
		
		mesh:add_surface(surf)
	end
	
	return mesh, has_skel
end

local function load_node_base(node, t)

	for k,v in ipairs(t.properties) do
		if v.name == "transform/local" then
			node:set_local_matrix(load_matrix(v.value))
		end
	end
end

local function load_mesh_node(scene, t)

	local node = MeshNode:new()
	load_node_base(node, t)
	
	for k,v in ipairs(t.properties) do
	
		if v.name == "mesh" then
			local mesh, has_skeleton = load_mesh(v, scene)
			node:set_mesh(mesh)
			if has_skeleton then
				node:set("use_skeleton_node", true)
			end
		end
	end
	
	return node
end

local function load_scene(scene, t)

	local node = SceneNode:new()
	load_node_base(node, t)
	return node
end

local function load_skeleton(scene, t)
	
	local node = SkeletonNode:new()
	load_node_base(node, t)
	
	local bones = {}
	local max = -1
	for k,v in ipairs(t.properties) do
	
		local idx,prop = string.match(v.name, "^bones/(%d+)/(.*)$")
		if idx then
			bones[v.name] = v.value
			if tonumber(idx) > max then
				max = tonumber(idx)
			end
		end
	end
	
	node:set_bone_count(max + 1)
	for i=0,max do
		local name = bones["bones/"..i.."/name"]
		local mat = load_matrix(bones["bones/"..i.."/rest"])
		local parent = bones["bones/"..i.."/parent"]
		node:add_bone(i, name, mat, parent)
	end
	
	return node
end

local _node_types = {
	Spatial = load_scene,
	Skeleton = load_skeleton,
	MeshInstance = load_mesh_node,
}

local function import_node(scene, t)
	
	local meta = load_table(t.meta)
	local node = _node_types[t.type](scene, t)
	node:set_name(meta.name)
	if meta.path then
		scene.root:get_node(meta.path):add_child(node)
		node:set_owner(scene.root)
	else
		scene.root = node
	end
end

local function import_scene(t, fname, root)

	if t[1].type ~= "Spatial" then
		return nil
	end
	local scene = {}
	
	scene.base_path = "."
	if string.find(fname, "/") then
		scene.base_path = string.gsub(fname, "/[^/]*$", "")
	end
	
	for k,v in ipairs(t) do
		if k==1 and root then
			scene.root = root
		else
			import_node(scene, v)
		end
	end
	
	return scene.root
end

function lua_import_scene(file, scene)

	-- clear cache?
	texture_cache = {}
	local t = require(file)
	if not t then
		return
	end
	if t.magic ~= "SCENE" then
		return
	end
	
	return import_scene(t, file, scene)
end

---------------------------------- animation ----------------------------------

local function add_key(anim, track, t)
	
	local type, value, offset, channel
	for k,v in ipairs(t) do
		if v.name == "value" then
			type = v.type
			if v.type == "vector3" then
				value = Vector3(unpack(v.value, 1, 3))
			elseif v.type == "quaternion" then
				value = load_quaternion(v.value)
			end
		end
		if v.name == "time" then
			offset = v.value
		end
		if v.name == "chan" then
			channel = v.value
		end
	end
	if channel == 1 then
		anim:add_location_key(track, offset, value)
	elseif channel == 2 then
		anim:add_rotation_key(track, offset, value)
	elseif channel == 4 then
		anim:add_scale_key(track, offset, value)
	end
end

local function import_animation(t)
	if not t[1] or t[1].type ~= "Animation" or not t[1].properties then
		return
	end
	
	local animation = Animation:new()
	local prop = load_table(t[1].properties)
	animation:set_loop(prop.loop ~= 0)
	animation:set_length(prop.length)

	local track = 1
	while prop["track/"..track.."/type"] do
		
		local tidx = animation:add_track(prop["track/"..track.."/path"])
		for k,v in ipairs(prop["track/"..track.."/keys"]) do
			add_key(animation, tidx, v)
		end
		track = track + 1
	end
	
	return animation
end

function lua_import_animation(file)

	local t = require(file)
	if not t then
		return
	end
	if t.magic ~= "SCENE" then
		return
	end
	
	return import_animation(t)
end

----------------------------- animation object -----------------------------

class("LuaAnimation", Lua__AnimationFormatLoader)

function LuaAnimation:load_animation(p_path)

	if not string.match(string.lower(p_path), "%.lua$") then
		return
	end
	return lua_import_animation(p_path)
end

AnimationLoader:add_animation_format_loader(LuaAnimation:new())

