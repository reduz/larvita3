require "helpers.lua"

local function parseargs(s)
  local arg = {}
  string.gsub(s, "([%w_]+)=([\"'])(.-)%2", function (w, _, a) -- terminating string '
    arg[w] = a
  end)
  return arg
end

local function collect(s)
  local stack = {}
  local top = {}
  table.insert(stack, top)
  local ni,c,label,xarg, empty
  local i, j = 1, 1
  while true do
    ni,j,c,label,xarg, empty = string.find(s, "<(%/?)([%w_]+)(.-)(%/?)>", j)
    if not ni then break end
    local text = string.sub(s, i, ni-1)
    if not string.find(text, "^%s*$") then
      table.insert(top, text)
    end
    if empty == "/" then  -- empty element tag
      table.insert(top, {label=label, xarg=parseargs(xarg), empty=1})
    elseif c == "" then   -- start tag
      top = {label=label, xarg=parseargs(xarg)}
      table.insert(stack, top)   -- new level
    else  -- end tag
      local toclose = table.remove(stack)  -- remove top
      top = stack[#stack]
      if #stack < 1 then
        error("nothing to close with "..label)
      end
      if toclose.label ~= label then
        error("trying to close "..toclose.label.." with "..label)
      end
      table.insert(top, toclose)
    end
    i = j+1
  end
  local text = string.sub(s, i)
print("text is ", text)
  if not string.find(text, "^%s*$") then
    table.insert(stack[stack.n], text)
  end
  if #stack > 1 then
    error("unclosed "..stack[stack.n].label)
  end
  return stack[1]
end

local function id(s)

	return string.gsub(s, "^#", "")
end

function add_material(data, dae)

	local mat = {}
	for k,v in ipairs(dae) do
	
		if v.label == "instance_effect" then
		
			mat.effect = id(v.xarg.url)
		end
	end
	data[dae.xarg.id] = mat
end

function add_float_array(data, dae)

	if tonumber(dae.xarg.count) == 0 then
		data[dae.xarg.id] = {}
		return
	end

	local arr = split(dae[1], "%s%s*")
	for i=1,#arr do
		arr[i] = tonumber(arr[i])
	end
	data[dae.xarg.id] = arr
end

function create_vertex_array(array, render, stride)

	if array.rid then
		return array.rid
	end
	
	local rid = render:array_create(Render.ARRAY_VERTEX, #array/3)
	local vcount = 0
	for i=1,#array,3 do
	
		local vec = Vector3(array[i], array[i+1], array[i+2]) -- lua's 1-based arrays
		vec = transform_vertex(vec)
		render:array_set_vertex(rid, vcount, vec)
		vcount = vcount + 1
	end
	
	array.rid = rid
	
	return rid
end

function create_normal_array(array, render)

	if array.rid then
		return array.rid
	end
	
	local rid = render:array_create(Render.ARRAY_NORMAL, #array/3)

	local vcount = 0
	for i=1,#array,3 do
		local vec = Vector3(array[i], array[i+1], array[i+2]) -- lua's 1-based arrays
		vec = transform_vertex(vec)
		render:array_set_normal(rid, vcount, vec)
		
		vcount = vcount + 1
	end
	
	array.rid = rid
	
	return rid
end

function create_texcoord_array(array, render, stride)

	if array.rid then
		return array.rid
	end

	local stride = stride or 3
	local rid = render:array_create(Render.ARRAY_TEX_UV, #array/stride)
	
	local i=1
	local vcount = 0
	while array[i] ~= nil do
	
		local uv = Render.UV(array[i], 1.0-array[i+1])
		render:array_set_UV(rid, vcount, uv)
		
		i = i + stride
		vcount = vcount + 1
	end

	array.rid = rid
	
	return rid
end

function get_material(data, matid)

	if data[matid] then
		return data[data[matid].effect]
	end
end

function add_surface(data, dae)

	local surface = { arrays = {} }
	if data.label == "triangles" then
		surface.type = Render.SURFACE_TRIANGLES
	else
		surface.type = Render.SURFACE_QUADS
	end
	surface.material = dae.xarg.material
	local inputs = {}
	local indices = {}
	for k,v in ipairs(dae) do
	
		local array
		local max_offset
		if type(v) == 'table' then
			if v.label == 'input' then

				table.insert(inputs, { semantic = v.xarg.semantic, source = data[id(v.xarg.source)], offset = v.xarg.offset, array = {}, indices = {} })
			end
			if v.label == "p" then
			
				if not v[1] then
					return
				end
			
				local ind = split(v[1], " ")
				for i=1,#ind do

					table.insert(indices, tonumber(ind[i]))
				end
			end
		end
	end

	local index_count = #indices / #inputs
	for i=0,index_count-1 do
	
		for j=1,#inputs do

			local source = inputs[j].source
			local index = indices[(i * #inputs) + inputs[j].offset + 1]
			table.insert(inputs[j].indices, index)
			for k=0,source.stride-1 do
				table.insert(inputs[j].array, source.data[(index * source.stride) + k + 1])
			end
		end
	end

	local array
	for k,v in ipairs(inputs) do
		if v.semantic == "VERTEX" then
			array = create_vertex_array(v.array, data.global.render, v.source.stride)
			array.vertex_indices = v.indices
		end
		if v.semantic == "NORMAL" then
			array = create_normal_array(v.array, data.global.render, v.source.stride)
		end
		if v.semantic == "TEXCOORD" then
			array = create_texcoord_array(v.array, data.global.render, v.source.stride)
			array.is_uv = true
		end
		table.insert(surface.arrays, array)
	end
	
	local idarr = data.global.render:array_create(Render.ARRAY_INDEX, index_count)
	order = {[0] = 0, 1, -1}
	--order = {[0] = 0, 0, 0}
	for i=0,index_count-1 do
		data.global.render:array_set_index(idarr, i, i+order[i%3])
	end
	table.insert(surface.arrays, idarr)
	
	table.insert(data.surfaces, surface)
end

function transform_vertex(vec)

	--do return vec end

	vec.y, vec.z = vec.z, vec.y
	vec.z = -vec.z
	
	return vec
end

function transform_matrix(mat)

	--do return mat end

	for col=0,3 do
	
		local tmp = mat:get(col, 1)
		mat:set(col, 1, mat:get(col, 2))
		mat:set(col, 2, tmp)
	end

	for row=0,3 do
		local tmp = mat:get(1, row)
		mat:set(1,row, mat:get(2, row))
		mat:set(2, row, tmp)
	end


	mat:set(2, 0, -mat:get(2, 0))
	mat:set(2, 1, -mat:get(2, 1))
	mat:set(2, 3, -mat:get(2, 3))
	mat:set(0, 2, -mat:get(0, 2))
	mat:set(1, 2, -mat:get(1, 2))
	mat:set(3, 2, -mat:get(3, 2))

	--local aux = mat:get(3, 0);
	--mat:set(3, 0, mat:get(3, 2));
	--mat:set(3, 2, -aux);
	
	return mat
end

function add_subscene(data, dae)
	
	local stack = {
	
		node = add_subscene,
	}

	local global = data.global or data
	local node = {name = dae.xarg.name, id = dae.xarg.id, sid = dae.xarg.sid}
	if dae.xarg.type == 'JOINT' then
	
		node.type = "bone"
		node.name = dae.xarg.name
		if dae.__parent.label == 'node' then
			node.parent = dae.__parent.xarg.id
		end
	end
	if dae.xarg.type == 'NODE' then
	
		node.type = "node"
	end
	
	for k,v in ipairs(dae) do

		if v.label == 'instance_controller' or v.label == 'instance_geometry' then
			node.instance = global[id(v.xarg.url)]
			for kcont,vcont in ipairs(v) do
				if vcont.label == 'skeleton' then

					node.skeleton = id(vcont[1])
				end
--				if vcont.label == "bind_material" then
--print("bind material for ", vcont[1][1].symbol, vcont[1][1].target)
--					data[vcont[1][1].symbol] = data[id(vcont[1][1].target)]
--				end
			end
		end
		if v.label == 'matrix' then
		
			local mlist = split(v[1], " ")
			node.matrix = get_matrix_from_array(mlist, 0)
			local pos = node.matrix:get_translation()
			--print("******** node pos is ", pos.x, pos.y, pos.z)
		end
	end
	
	global[dae.xarg.id] = node
	if data.__parent then
		table.insert(data.__parent, node)
	end

	if node.type ~= 'bone' and not node.instance then
		node.instance = DummyNode:new()
	end
	
	local scene_data = { global = global, __parent = node }
	parse_level(scene_data, dae, stack)
end

function add_scene(data, dae)

	local stack = {
	
		node = add_subscene,
	}
	
	local instance = DummyNode:new()
	instance:set_name(dae.xarg.name)
	
	local node = { name = dae.xarg.name, instance = instance, type = "scene" }
	local scene_data = { global = data, __parent = node }
	parse_level(scene_data, dae, stack)
	
	if not data.root_scenes then
		data.root_scenes = {}
	end
	table.insert(data.root_scenes, node)
end

function add_source(data, dae)

	local sid = dae.xarg.id
	local source = {}
	for k,v in ipairs(dae) do
	
		if v.label == "float_array" then
			source.data = data[id(v.xarg.id)]
		end
		if v.label == "Name_array" then
			source.data = data[id(v.xarg.id)]
		end
		if v.label == "technique_common" then
		
			for kt, vt in ipairs(v) do
				if vt.label == "accessor" then
				
					source.stride = tonumber(vt.xarg.stride)
				end
			end
		end
	end
	data[sid] = source
end

function add_vertices(data, dae)

	local vid = dae.xarg.id
	for k,v in ipairs(dae) do
	
		if v.label == 'input' then
			if v.xarg.semantic == 'POSITION' then
				data[vid] = data[id(v.xarg.source)]
			end
		end
	end
end

function add_mesh(data, dae)

	local stack = {
	
		mesh = {

			source = {

				float_array = add_float_array,
				technique_common = {},
				add_source,
			},
			
			vertices = add_vertices,

			triangles = add_surface,
			polygons = add_surface,
		},
	}
	
	local mesh_data = {global = data, surfaces = {}}

	parse_level(mesh_data, dae, stack)
	local surface_vertex_indices = {}

	local mesh = data.render:mesh_create()

	for k,v in ipairs(mesh_data.surfaces) do
	
		local surf = data.render:mesh_add_surface(mesh)

		for arrk, arrv in ipairs(v.arrays) do
			data.render:mesh_bind_array_to_surface(mesh, surf, arrv)
			if arrv.vertex_indices then
				surface_vertex_indices[surf] = arrv.vertex_indices
			end
		end
		if v.material and get_material(data, v.material) then

			data.render:mesh_set_surface_material(mesh, surf, get_material(data, v.material))
		end
	end

	local mesh_node = MeshNode:new()
	mesh_node:set_name(dae.xarg.name)
	mesh_node:set_mesh(mesh)
	mesh_node.surface_vertex_indices = surface_vertex_indices

	data[dae.xarg.id] = mesh_node
end

function add_name_array(data, dae)

	local arr = split(dae[1], " ")
	for i=1,#arr do
		arr[i] = tostring(arr[i])
	end
	data[dae.xarg.id] = arr
end

function add_joints(data, dae)

	for k,v in ipairs(dae) do

		if v.label == 'input' then
			local source = id(v.xarg.source)
			if v.xarg.semantic == 'JOINT' then
			
				for kjoint,vjoint in ipairs(data[source].data) do
				
					if not data.joints[kjoint] then
						data.joints[kjoint] = {}
					end
					
					data.joints[kjoint].name = vjoint
				end
			end
			
			if v.xarg.semantic == 'INV_BIND_MATRIX' then
			
				local i=1
				local vals = data[source].data
				local jcount = 1
				while jcount * 16 <= #vals do
				
					local matrix = get_matrix_from_array(vals, jcount - 1)
					if not data.joints[jcount] then
						data.joints[jcount] = {}
					end
					data.joints[jcount].matrix = matrix:inverse()
				
					jcount = jcount + 1
				end
			end
		end
	end
end

function add_weights(data, dae)

	local inf_list, inf_count, bone_list, weight_list

	for k,v in ipairs(dae) do

		if v.label == "input" then
			if v.xarg.semantic == "JOINT" then
				bone_list = data[id(v.xarg.source)].data
			end
			if v.xarg.semantic == "WEIGHT" then
				weight_list = data[id(v.xarg.source)].data
			end
		end
		if v.label == 'vcount' then
			inf_count = split(v[1], " ")
		end
		if v.label == "v" then
			inf_list = split(v[1], " ")
		end
	end
	
	--local weights = data.global.render:array_create(Render.ARRAY_BONE_WEIGHTS, #inf_count)
	local weights = { }
	local inf_id = 1
	for i=1,#inf_count do
	
		local infs = tonumber(inf_count[i])
		if infs > Render.MAX_WEIGHTS_PER_VERTEX then
			infs = Render.MAX_WEIGHTS_PER_VERTEX
		end
		--local weight = Render.VertexWeight()
		local weight = { weight = {} }
		weight.weights = infs
		for j = 1,infs do
		
			--local bw = Render.BoneWeight()
			local bw = {}
			bw.bone_idx = data.joints[inf_list[inf_id]+1].name
			inf_id = inf_id + 1

			bw.weight = tonumber(weight_list[tonumber(inf_list[inf_id]+1)] * 255)
			inf_id = inf_id + 1
			
			weight.weight[j-1] = bw
		end
		
		--data.global.render:array_set_weight(weights, i-1, weight)
		table.insert(weights, weight)
	end
	
	--if data.mesh then
	--	local mesh = data.mesh:get_mesh()
	--	data.global.render:mesh_bind_array_to_surface(mesh, 0, weights)
	--end
	data.weights = weights
end

function add_shape_matrix(data, dae)

	local matlist = split(dae[1], " ")
	local mat = get_matrix_from_array(matlist, 0)

	data.shape_matrix = mat
end

function add_skeleton(data, dae)

	local stack = {
	
		source = {

			Name_array = add_name_array,
			float_array = add_float_array,
			
			add_source,
		},
		bind_shape_matrix = add_shape_matrix,		
		joints = add_joints,
		vertex_weights = add_weights,
		
	}
	
	local skel_data = { mesh = data[id(dae.xarg.source)], global = data, joints = {} }
	
	parse_level(skel_data, dae, stack)
	
	--local skel = data.render:skeleton_create(#skel_data.joints)
	local node = SkeletonNode:new()
	node.data = skel_data
	--node:set_skeleton(skel)
	--for k,v in ipairs(skel_data.joints) do
	--	node:add_bone(v.name, v.matrix or transform_matrix(Matrix4()), v.parent or -1)
	--end

	if skel_data.mesh then
		node:add_child(skel_data.mesh)
		if skel_data.shape_matrix then
			--skel_data.mesh:set_matrix(skel_data.shape_matrix)
		end
	end
	
	data[dae.__parent.xarg.id] = node
	data[skel_data.joints[1].name] = node
end

function skeleton_add_data(data, node, skeleton)

	local skel_list = {}
	local idx = 0
	local function add(bone, parent)

		skel_list[bone.sid or bone.id] = bone
		skel_list[bone.sid or bone.id].idx = idx
		table.insert(skel_list, bone.sid or bone.id)
		idx = idx+1
		for k,v in ipairs(bone) do
		
			if v.type == "bone" then
			
				add(v)
			end
		end
	end
	add(skeleton)
	node:set_matrix(skeleton.matrix)

	for k,v in ipairs(node.data.joints) do
		skel_list[v.name].matrix = v.matrix
	end

	skel_list[skeleton.sid].matrix = transform_matrix(Matrix4())


	--local skel = data.render:skeleton_create(#skel_list)
	--node:set_skeleton(skel)
	node:set_bone_count(#skel_list)

	for k,v in ipairs(skel_list) do

		local parent = -1
		if skel_list[v].parent and skel_list[skel_list[v].parent] then
			parent = skel_list[skel_list[v].parent].idx
			local aux = skel_list[skel_list[v].parent].matrix:inverse() * skel_list[v].matrix
			skel_list[v].mat_final = aux
		else
			skel_list[v].mat_final = node.data.shape_matrix:inverse() * skel_list[v].matrix
		end
		--print("adding bone ", skel_list[v].name, skel_list[v].matrix)
		local bone = node:add_bone(k-1, skel_list[v].name, skel_list[v].mat_final or transform_matrix(Matrix4()), parent)
	end
	
	local mesh_node = node.data.mesh
	--mesh_node:set_skeleton(skel)
	mesh_node:set_skeleton_path("..")
	local mesh_rid = mesh_node:get_mesh()
	local weight_list = node.data.weights

	local surf = 0
	while mesh_node.surface_vertex_indices[surf] do
	
		local arr = data.render:array_create(Render.ARRAY_BONE_WEIGHTS, #mesh_node.surface_vertex_indices[surf])

		local weight = Render.VertexWeight()
		for ki, vi in ipairs(mesh_node.surface_vertex_indices[surf]) do

			weight.weights = tonumber(weight_list[vi+1].weights)

			local total = 0.0
			for j = 0,weight.weights-1 do

				local bw = Render.BoneWeight()

				bw.bone_idx = skel_list[weight_list[vi+1].weight[j].bone_idx].idx
				bw.weight = weight_list[vi+1].weight[j].weight

				weight.weight[j] = bw
				total = total + bw.weight
			end
			data.render:array_set_weight(arr, ki-1, weight)
		end

		data.render:mesh_bind_array_to_surface(mesh_rid, surf, arr)
		
		surf = surf + 1
	end
end

function create_scene(data, dae)

	local root = { instance =  SceneNode:new() }
	root.instance:set_name("Root Scene")
	data.parent_node:add_child(root.instance)
	
	local function add(node, parent)
	
		if node.instance then
			parent.instance:add_child(node.instance)
			if node.matrix then
				node.instance:set_matrix(node.matrix)
			end
			if node.name then
				node.instance:set_name(node.name)
			end
			if node.skeleton then
				skeleton_add_data(data, node.instance, data[node.skeleton])
			end
			for k,v in ipairs(node) do
				add(v, node)
			end
		end
	end
	
	for k,v in ipairs(data.root_scenes) do
		add(v, root)
	end
	
	data.__root = root.instance
end

function get_color(r,g,b, a)
	return Color(r*255.0, g*255.0, b*255.0, a*255.0)
end

function path_is_relative(path)

	if string.match(path, "^/") or string.match(path, "^.:") then
		return false
	end
	return true
end

function add_effect(data, dae)

	local mat = data.render:material_create()
	data.render:material_set_flag(mat, Render.MATERIAL_VISIBLE, true)
	data.render:material_set_flag(mat, Render.MATERIAL_DOUBLE_SIDED, false)

	local function add_texture(data, dae)
		
		local texture_path = data.global[id(dae.xarg.texture)].filename or ""
		if path_is_relative(texture_path) then
			texture_path = data.global.dae_path .. texture_path
		end
		local file_system = GUI.FileSystem:instance_func()
		texture_path = file_system:get_abs_file_path(texture_path)
		local rel_path = strip_resource_path(texture_path, data.global.dae_path)
		file_system:delete()
		local tpath
		if rel_path then
			-- copy texture
			--print("******* running ".."cp "..texture_path.." "..GlobalVars:get_var("resource_path").."/"..rel_path)
			tpath = GlobalVars:get_var("resource_path").."/"..rel_path
			os.execute("cp "..texture_path.." "..tpath)
		end
		
		local texid = data.global.render:texture_create()
		local err = data.global.render:texture_load_image(texid, tpath or "")
		if err ~= OK then
			texid = data.global.render:test_texture_get()
		end
		data.global.render:texture_set_path(texid, rel_path or "")
		data.global.render:material_set_texture(mat, Render.MATERIAL_TEXTURE_DIFFUSE, texid)
	end

	local function add_lights(data, dae)

		local type = dae.__parent.label
	
		for k,v in ipairs(dae) do
		
			if v.label == "emission" then
				data.global.render:material_set_light_color(mat, Render.LIGHT_EMISSIVE, get_color(unpack(split(v[1][1], " "))))
			end
			if v.label == "ambient" then
				if v[1].label == color then
					data.global.render:material_set_light_color(mat, Render.LIGHT_AMBIENT, get_color(unpack(split(v[1][1], " "))))
				elseif v[1].label == 'texture' then
					add_texture(data, v[1])
				end
			end
			if v.label == "diffuse" then
				
				if v[1].label == color then
					data.global.render:material_set_light_color(mat, Render.LIGHT_DIFFUSE, get_color(unpack(split(v[1][1], " "))))
				elseif v[1].label == 'texture' then
					add_texture(data, v[1])
				end
			end
			if v.label == "reflective" then
				if type == 'lambert' then
					data.global.render:material_set_light_color(mat, Render.LIGHT_SPECULAR, get_color(unpack(split(v[1][1], " "))))
				end
			end
			
			if v.label == "specular" then
				
				--data.global.render:material_set_light_color(mat, Render.LIGHT_SPECULAR, get_color(unpack(split(v[1][1], " "))))
			end
			if v.label == 'transparency' then
			
				local t = v[1][1];
				data.global.render:material_set_var(mat, Render.MATERIAL_TRANSPARENCY, 1.0 - t)
			end
			if v.label == 'shininess' then
			
				local s = v[1][1];
				data.global.render:material_set_var(mat, Render.MATERIAL_TRANSPARENCY, s)
			end
		end
	end

	local function add_image_surf(data, dae)
	
		if dae.xarg.type == "2D" then
		
			local imgid = dae.__parent.xarg.sid
			local sourceid
			for k,v in ipairs(dae) do
			
				if v.label == "init_from" then
					sourceid = v[1]
				end
			end
			data.global[imgid] = data.global[sourceid]
		end
	end
	local function add_sampler(data, dae)
	
		local samid = dae.__parent.xarg.sid
		local image
		for k,v in ipairs(dae) do
		
			if v.label == "source" then
				image = v[1]
			end
		end
		
		data.global[samid] = { filename = data.global[image] }
	end

	local stack = {

		profile_COMMON = {
		
			technique = {
		
				lambert = add_lights,
				blinn = add_lights,
			},
			
			newparam = {
				surface = add_image_surf,
				sampler2D = add_sampler,
			},
		},
	}

	local fx_data = { global = data, lights = {} }
	parse_level(fx_data, dae, stack)

	data[dae.xarg.id] = mat
end

function parse_fname(fname)

	fname = string.gsub(fname, "^file:///", "")
	fname = string.gsub(fname, "%%20", " ")
	
	return fname
end

function add_image(data, dae)

	if (not dae[1]) then
		return
	end
	data[dae.__parent.xarg.id] = parse_fname(dae[1])
end

-- this is a stack with all the shit that comes inside the xml file
-- we register the handlers we want for each label here

local dae_images = {

	COLLADA = {

		library_images = {
		
			image = {
				init_from = add_image,
			},
		},
	},
}

local dae_effects = {

	COLLADA = {

		library_effects = {
		
			effect = {
			
				add_effect,
			},
		},
	},
}

local dae_materials = {

	COLLADA = {
		library_materials = {
		
			material = add_material,
		},
	},
}

local dae_geometries = {
	
	COLLADA = {
		library_geometries = {
		
			geometry = {
			
				add_mesh,
			},
		},
	},
}

local dae_controllers = {

	COLLADA = {
		library_controllers = {

			controller = {
				skin = add_skeleton,
			},
		},
	},
}

local dae_scenes = {

	COLLADA = {
		library_visual_scenes = {
		
			visual_scene = add_scene,
			create_scene,
		},
	},
}

local dae_stack = {

	COLLADA = {
	
		asset = {},
		library_animations = {},
		library_cameras = {},
		library_effects = {
		
			effect = {
			
				add_effect,
			},
		},
		
		library_images = {
		
			image = {
				init_from = add_image,
			},
		},
		
		library_lights = {},

		library_materials = {
		
			material = add_material,
		},
		
		library_geometries = {
		
			geometry = {
			
				add_mesh,
			},
		},
		
		library_visual_scenes = {
		
			visual_scene = add_scene,
			create_scene,
		},
		
		library_controllers = {

			controller = {
				skin = add_skeleton,
			},
		},
		
		scene = {},
	},

}

function parse_level(data, dae, stack)

	for k,v in ipairs(dae) do

		if type(v) == 'table' then
			v.__parent = dae
		
			if v.label and stack[v.label] then
			
				local val = stack[v.label]

				if type(val) == 'function' then
					val(data, v)
				end
				if type(val) == 'table' then
				
					parse_level(data, v, val)
					
					if type(val[1]) == 'function' then
					
						val[1](data, v, val)
					end
				end
			end
		end
	end
end

function import_dae(fname, render, parent)

	local f = io.open(fname)
	if not f then
		return nil
	end
	
	--if Collada:normalize_document(fname, Vector3(0, 1, 0), 0.01) ~= OK then
	--	return nil
	--end
	
	local dae_str = f:read("*a")
	
	local dae = collect(dae_str)
	
	local path = string.match(fname, "^(.*/)[^/]*$")
	
	local data = {render = render, parent_node = parent, dae_path = path}
	--parse_level(data, dae, dae_stack)
	
	parse_level(data, dae, dae_images)
	parse_level(data, dae, dae_effects)
	parse_level(data, dae, dae_materials)
	parse_level(data, dae, dae_geometries)
	parse_level(data, dae, dae_controllers)
	parse_level(data, dae, dae_scenes)

	
	local function set(node)
		if node ~= data.__root then
			node:set_owner(data.__root)
		end
		local c = node:get_child_count()
		for i=0,c-1 do
			set(node:get_child_by_index(i))
		end
	end
	set(data.__root)
	
	--[[
	local mat = Matrix4()
	mat:scale(0.001, 0.001, 0.001)
	data.__root:set_matrix(mat)
	--]]
	
	return data.__root
end

---------------------------- animation ----------------------------

function anim_add_subscene(data, dae)

	local stack = {
	
		node = anim_add_subscene,
	}
	data[dae.xarg.id] = { name = dae.xarg.name }

	for k,v in ipairs(dae) do

		if v.label == 'matrix' then
		
			local mlist = split(v[1], " ")
			data[dae.xarg.id].matrix = get_matrix_from_array(mlist, 0)
		end
	end
	
	parse_level(data, dae, stack)
end

function anim_add_scene(data, dae)

	local stack = {
	
		node = anim_add_subscene,
	}

	parse_level(data, dae, stack)
end

function add_sampler(data, dae)

	local sampler = {}
	sampler.id = dae.xarg.id
	
	for k,v in ipairs(dae) do
	
		if v.label == "input" then
		
			if v.xarg.semantic == "INPUT" then
				sampler.offset_list = data[id(v.xarg.source)]
			end
			if v.xarg.semantic == "OUTPUT" then
				sampler.matrix_list = data[id(v.xarg.source)]
			end
			if v.xarg.semantic == "INTERPOLATION" then
				sampler.interpolation_list = data[id(v.xarg.source)]
			end
		end
	end
	data[dae.xarg.id] = sampler
	table.insert(data.samplers, sampler)
end

function add_channel_target(data, dae)

	local bone_name = string.gsub(dae.xarg.target, "/.*$", "")

	data[id(dae.xarg.source)].bone_name = bone_name
end

function get_matrix_from_array(array, offset)
	local mat = Matrix4()
	local start = (offset * 16)
	local i = 1

	for row = 0,3 do
	
		for col = 0,3 do
		
			mat:set(col, row, array[start + i])
			--print("setting ", col, row, tofloat(array[start + i]), array[start + i])
			i = i+1
		end
	end
	
	return transform_matrix(mat)
end

function add_channel(data, dae)

	local stack = {
		source = {

			Name_array = add_name_array,
			float_array = add_float_array,

			add_source,
		},
		sampler = add_sampler,
		channel = add_channel_target,
	}
	
	parse_level(data, dae, stack)
end

function add_track(data, sampler, parent_sampler, skel)

	--if sampler.matrix_list.stride ~= 16 then
	--	print("invalid stride for ", data[sampler.bone_name].name)
	--	return 0
	--end

	local anim = data.animation
	local tnum = Animation:add_track(anim, data[sampler.bone_name].name)

	Animation:set_frame_count(anim, tnum, Animation.TYPE_POSITION, #sampler.offset_list.data)
	Animation:set_frame_count(anim, tnum, Animation.TYPE_ROTATION, #sampler.offset_list.data)

	local time = 0
	for k,v in ipairs(sampler.offset_list.data) do
		local mat
		if sampler.matrix_list.stride == 16 then
			mat = get_matrix_from_array(sampler.matrix_list.data, k-1)
		else
			mat = data[sampler.bone_name].matrix
		end
		local initial_mat = data[sampler.bone_name].matrix
		local restid = skel:get_bone_id(data[sampler.bone_name].name)
		local rest = skel:get_bone_rest(restid)
		mat = rest:inverse() * mat

		--if parent_sampler then
		--	local parent_mat = get_matrix_from_array(parent_sampler.matrix_list.data, k-1)
		--	local parent_initial_mat = data[parent_sampler.bone_name].matrix
		--	mat = (parent_initial_mat * parent_mat):inverse() * mat
		--end
		time = tonumber(v)

		Animation:set_pos_frame(anim, tnum, k-1, time, mat:get_translation())

		local q = Quat(mat)
		Animation:set_rot_frame(anim, tnum, k-1, time, q)
	end
	return time
end

function dae_import_animation(fname, render, skeleton)

	local f = io.open(fname)
	if not f then
		return nil
	end
	local dae_str = f:read("*a")
	
	local dae = collect(dae_str)


	local stack = {

		COLLADA = {

			library_visual_scenes = {

				visual_scene = anim_add_scene,
			},

			library_animations = {

				animation = add_channel,
			},
		},
	}
	
	local data = {render = render, samplers = {}}
	data.animation = Animation:create_rid()

	parse_level(data, dae, stack)
	
	local samplers = {}
	for k,v in ipairs(data.samplers) do
	
		samplers[data[v.bone_name].name] = v
	end
	
	local function get_sampler_parent(sampler)
	
		local parent
		local bonid = skeleton:get_bone_id(sampler)
		local bonparent = skeleton:get_bone_parent(bonid)
		if bonparent ~= -1 then
			local parent_name = skeleton:get_bone_name(bonparent)
			parent = samplers[parent_name]
			if not parent then
				return get_sampler_parent(parent_name)
			end
		else
			return nil
		end

		return parent
	end
	
	local total_time = 0
	for k,v in ipairs(data.samplers) do

		local parent = get_sampler_parent(data[v.bone_name].name)
		local time = add_track(data, v, parent, skeleton)
		if time > total_time then
			total_time = time
		end
	end

	Animation:set_length(data.animation, total_time)
	Animation:set_loops(data.animation, true)
	return data.animation
end

---------------------------- skeleton hack ----------------------------

function dae_hack_skeleton(fname, p_scene)

	
end

---------------------------- helper ----------------------------


function dump_table(name, t, tab)

	tab = tab or ""
	print(tab..name.." = {")
	local ind = tab.."\t"

	local function print_val(k,v)
		if type(v) == 'table' then
			dump_table(k, v, ind)
		else
			print(ind..k.." = "..tostring(v))
		end
	end
	
	for k,v in pairs(t) do
		if type(k) ~= 'number' then
			print_val(k,v)
		end
	end

	for k,v in ipairs(t) do
	
		print_val(k,v)
	end
	
	print(tab.."},")
end

--t = import_dae(arg[1])
--dump_table("", t)

