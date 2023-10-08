require "helpers.lua"
require "dae_import.lua"
require "import_scene_dialog.lua"
require "iapi_editor.lua"
require "viewport.lua"
require "open_scene_dialog.lua"
require "memory_debug_window.lua"
require "anim_editor.lua"
require "indicator.lua"
require "reparent_dialog.lua"
require "cursor_node.lua"
require "spline_editor.lua"
require "instance_dialog.lua"
require "blender_import.lua"
require "console.lua"

local function tcopy(t)

	local r = {}
	for k,v in pairs(t) do
		r[k] = v
	end
	
	return r
end

local gui_reserved = { type = true, add_args = true, init = true, add = true, name = true, }




function load_gui(root, tree, parent)

	if type(tree) == 'string' then
	
		tree = { type = {GUI.Label, tree} }
	end

	local wtype = tree.type

	local obj
	if wtype then
		local args = {}
		if wtype[1] then
			for i=2,#wtype do
				args[i-1] = wtype[i]
			end
			wtype = wtype[1]
		end

		obj = wtype:new(unpack(args))

		if parent then
			local add_args = tcopy(tree.add_args or {})
			local pos = add_args.self_pos or 1
			table.insert(add_args, pos, obj)

			if type(tree.add) == "function" then
				tree.add(obj, parent, root, tree)
			else
				parent:add(unpack(add_args))
			end
		end

		if tree.name then
			if root[tree.name] then
				if type(root[tree.name]) ~= 'table' then
					root[tree.name] = {root[tree.name]}
				end
				table.insert(root[tree.name], obj)
			else
				root[tree.name] = obj
			end
		end
	else
	
		obj = parent
	end

	for k,v in pairs(tree) do
	
		if type(k) == 'string' and not gui_reserved[k] then

			obj[k] = v
		end
	end

	for k,v in ipairs(tree) do
	
		load_gui(root, v, obj)
	end
	
	if type(tree.init) == 'function' then
	
		tree.init(obj, parent, root, tree)
	end
	
	return obj
end

---------------------------------------------------------------------------------------------------------------------

class("RootBox", GUI.Lua__VBoxContainer)

function RootBox:stylebox(t)

	if t == GUI.SB_ROOT_CONTAINER then
		return GUI.StyleBox()
	else
	
		return GUI.Frame.stylebox(self, t)
	end
end


---------------------------------------------------------------------------------------------------------------------

local function print_scene(scene)

	print(Node:get_node_path(scene))
	
	local count = scene:get_child_count()
	for i=0,count-1 do
	
		local child = scene:get_child_by_index(i)
		print_scene(child)
	end
end

---------------------------------------------------------------------------------------------------------------------

class "EditorMainWindow"

function EditorMainWindow:enter_rotate_mode()

	if self.move_mode == "rotate_object" then
		return
	end
	if self.move_mode ~= "free" then
		self:leave_current_mode(true)
	end

	self.view.rotate_list = self:compute_transform_list()
	
	self.move_mode = "rotate_object"
end

function EditorMainWindow:restore_selection_cache()

	if self.move_mode == "rotate_object" or self.move_mode == "move_object" then
		for k,v in ipairs(self.view.selection) do
	
			if v.matrix_cache then
				v.node:set("matrix/local", v.matrix_cache)
				--v.node:set_local_matrix(v.matrix_cache)
			end
		end
	elseif self.move_mode == "cursor_helper_move" then
	
		self.view.current_cursor:set_global_matrix(self.view.current_cursor_matrix_cache)
	end
end

function EditorMainWindow:leave_rotate_mode(cancel)

	if cancel then
		self:restore_selection_cache()
	end

	self.view.rotate_list = nil
	self.move_mode = "free"
end

function EditorMainWindow:enter_move_mode()

	if self.move_mode == "move_object" then
		return
	end
	if self.move_mode ~= "free" then
		self:leave_current_mode(true)
	end

	self.view.move_list = self:compute_transform_list()
	
	self.move_mode = "move_object"
end

function EditorMainWindow:leave_move_mode(cancel)

	if cancel then
		self:restore_selection_cache()
	end
	
	self.view.move_list = nil
	self.move_mode = "free"
end

function EditorMainWindow:leave_current_mode(p_cancel)

	if self.move_mode == "move_object" then
	
		self:leave_move_mode(p_cancel)
	end
	
	if self.move_mode == "rotate_object" then
		self:leave_rotate_mode(p_cancel)
	end
	
	if self.move_mode == "cursor_helper_move" then
	
		self:leave_cursor_helper_mode(p_cancel)
	end
end

function EditorMainWindow:compute_transform_list(mouse_pos)

	local list = {}
	local center = Vector3()
	for k,v in ipairs(self.view.selection) do

		if self:owned(v.node) then
	
			v.matrix_cache = Matrix4(v.node:get_local_matrix())
			v.global_matrix_cache = Matrix4(v.node:get_global_matrix())
			local parent = v.node:get_parent()
			local add = true
			while parent do
			
				if self:node_is_selected(parent) then
					add = false
					parent = nil
				else
					parent = parent:get_parent()
				end
			end
			
			if add then
				table.insert(list, v)
				center = center + v.node:get_global_matrix():get_translation()
			end
		end
	end
	
	if #list > 0 then
		center = center / #list
	end
	
	list.center = center
	
	list.mouse_pos = mouse_pos

	return list
end

function EditorMainWindow:node_rotate(node, center, mouse_x, mouse_y, camera, plane, matrix)

				
	--local global = Matrix4( node:get_global_matrix() )
	local global = Matrix4()
	global:set_translation(center)
	local center_orig = global:inverse()

	if plane == "camera" then
		local vec_from = global:get_translation()
		local vec_to = camera:get_global_matrix():get_translation()
		local vec=vec_to-vec_from;
		vec:normalize();
	
		local rot_mat = Matrix4()
	
		rot_mat:load_rotate(vec,mouse_x/50.0)
	
		global:clear_translation()
		global = rot_mat * global
		global:set_translation(vec_from)
	end
	
	if plane == "xy" then
	
		global:rotate_z(mouse_x/50.0)
	end

	if plane == "xz" then
	
		global:rotate_y(mouse_x/50.0)
	end
	if plane == "zy" then
	
		global:rotate_x(mouse_x/50.0)
	end

	local nodemat = matrix or node:get_global_matrix()
	
	local matlocal = center_orig * nodemat
	matlocal = global * matlocal
	
	--node:set_global_matrix(matlocal)
	node:set("matrix/global", matlocal)
	--orthonormalize_node(node)	
end

function EditorMainWindow:node_move(node, vec, camera, plane, matrix)

	local mat = Matrix4(matrix or node:get_global_matrix())

	mat:set_translation(mat:get_translation() + vec)
	node:set("matrix/global", mat)

	do return end

	
	local cam = camera:get_global_matrix()
	local distance = mat:get_translation():distance_to(cam:get_translation())

	if plane == "camera" then
	
		mat = cam:inverse() * mat
		mat:set_translation(mat:get_translation() + vec * distance)
		
		--local final_pos = (cam * mat):get_translation()
		--locmat:set_translation(final_pos)
	
		node:set("matrix/global", cam * mat)
		--node:set_global_matrix(cam * mat)
	else
	
		local loc = mat:get_translation()
		vec = vec * distance
		
		if plane == "xy" then
		
			loc = loc + vec
		end
		if plane == "xz" then
		
			loc.x = loc.x + vec.x
			loc.z = loc.z - vec.y
		end
		
		if plane == "zy" then
		
			loc.z = loc.z + vec.x
			loc.y = loc.y + vec.y
		end
		
		mat:set_translation(loc)
		
		node:set("matrix/global", mat)
		--node:set_global_matrix(mat)
	end

	--orthonormalize_node(node)
end

function EditorMainWindow:get_move_plane(center)

	local cam = self.view.current_camera:get_global_matrix():get_translation()
	local norm = self.view.cursor:get_global_matrix():get_translation() - cam
	norm:normalize()
	
	if self.move_mode_plane == "camera" then

		return Plane(center, norm)
	else
	
		local pnorm
	
		-- y, z plane
		local yznorm = Vector3(1, 0, 0)
		local dot = math.abs(norm:dot(yznorm))
		pnorm = yznorm
		-- x, z plane
		local xznorm = Vector3(0, 1, 0)
		if math.abs(norm:dot(xznorm)) > dot then
		
			dot = math.abs(norm:dot(xznorm))
			pnorm = xznorm
		end

		-- x, y plane
		local xynorm = Vector3(0, 0, 1)
		if math.abs(norm:dot(xynorm)) > dot then
		
			dot = norm:dot(xynorm)
			pnorm = xynorm
		end

		return Plane(center, pnorm)
	end
end

function EditorMainWindow:get_move_vector(event, pos, center)

	local x1, x2, y1, y2
	if event.pos then
		x2, y2 = event.pos.x, event.pos.y
		x1, y1 = x2 - event.rel.x, y2 - event.rel.y
	else
		x1, y1 = event[1], event[2]
		x2, y2 = pos[1], pos[2]
	end

	local cmat = self.view.current_camera:get_global_matrix()
	local cam = cmat:get_translation()

	local dist = cam:distance_to(center) * 2

	local mstart = self:get_ray(x1, y1, dist)
	local mend = self:get_ray(x2, y2, dist)
	cmat:transform(mstart)
	cmat:transform(mend)

	local plane = self:get_move_plane(center)
	
	local p1 = Vector3()
	plane:get_intersection_with_segment(cam, mstart, p1)
	
	local p2 = Vector3()
	plane:get_intersection_with_segment(cam, mend, p2)
	
	local trans = p2 - p1

	return trans
end

function EditorMainWindow:mouse_motion(p_pos, p_rel, p_button_mask)

	p_pos = p_pos + self.viewport:get_global_pos()

	if self.move_mode == "cursor_rotate" then

		self.view.cam_rot_y = self.view.cam_rot_y - (p_rel.x * (math.pi / 180.0))
		self.view.cam_rot_x = self.view.cam_rot_x - (p_rel.y * (math.pi / 180.0))
		self:update_cursor_camera()
	end
	if self.move_mode == "cursor_move" then
	
		local move = self:get_move_vector( { pos = p_pos, rel = p_rel}, nil, self.view.cursor:get_global_matrix():get_translation() )
		move = move * -1
		self:node_move(self.view.cursor, move, self.view.current_camera, "camera")
	end
	
	if self.move_mode == "move_object" then

		if not self.view.move_list.mouse_pos then
			self.view.move_list.mouse_pos = {p_pos.x - p_rel.x, p_pos.y - p_rel.y}
		end

		local move = self:get_move_vector(self.view.move_list.mouse_pos, {p_pos.x, p_pos.y}, self.view.move_list.center)
		for k,v in ipairs(self.view.move_list) do
			self:node_move(v.node, move, self.view.current_camera, self.move_mode_plane, v.global_matrix_cache)
		end
	end
	
	if self.move_mode == "rotate_object" then
	
		if not self.view.rotate_list.mouse_pos then
			self.view.rotate_list.mouse_pos = {p_pos.x - p_rel.x, p_pos.y - p_rel.y}
		end

		local relx = p_pos.x - self.view.rotate_list.mouse_pos[1]
		local rely = p_pos.y - self.view.rotate_list.mouse_pos[2]
		
		for k,v in ipairs(self.view.rotate_list) do
			self:node_rotate(v.node, self.view.rotate_list.center, relx, rely, self.view.current_camera, self.move_mode_plane, v.global_matrix_cache)
		end
	end
	
	if self.move_mode == "cursor_helper_move" then
	
		local move = self:get_move_vector( { pos = p_pos, rel = p_rel}, nil, self.view.current_cursor:get_global_matrix():get_translation() )
		self:node_move(self.view.current_cursor, move, self.view.current_camera, self.move_mode_plane)
	end
end

function EditorMainWindow:mouse_button(p_pos, p_button, p_press, p_modifier_mask)

	p_pos = p_pos + self.viewport:get_global_pos()

	local shift = bw_and(p_modifier_mask, KEY_MASK_SHIFT) ~= 0

	if p_button == GUI.BUTTON_WHEEL_UP and p_press then
	
		self.view.cursor_distance = (self.view.cursor_distance ) / 1.1
		self:update_cursor_camera()
	end
	if p_button == GUI.BUTTON_WHEEL_DOWN and p_press then
	
		self.view.cursor_distance = (self.view.cursor_distance ) * 1.1
		self:update_cursor_camera()
	end
	
	if p_button == GUI.BUTTON_MIDDLE then
	
		if p_press then
			if self.move_mode == "free" then
			
				if shift then
					self.move_mode = "cursor_move"
				else
					self.move_mode = "cursor_rotate"
				end
			else
				self:toggle_move_mode_plane()
			end
		else
			if self.move_mode == "cursor_move" or self.move_mode == "cursor_rotate" then
				self.move_mode = "free"
			end
		end
	end
	
	if p_button == GUI.BUTTON_RIGHT then

		if p_press then
			if self.move_mode == "free" then

				-- selection
				self:pick_object(p_pos, shift)
			else

				self:leave_current_mode(true)
			end
		end
	end

	if p_button == GUI.BUTTON_LEFT then

		if p_press then

			if self.move_mode ~= "free" then

				self:leave_current_mode(false)
			else

				self:enter_cursor_helper_mode(p_pos)
			end
		else
		
			if self.move_mode == "cursor_helper_move" then
			
				self:leave_cursor_helper_mode(false)
			end
		end
	end
end

function EditorMainWindow:enter_cursor_helper_mode(p_pos)

	local list = self:get_pick_list(p_pos)
	if not list then
	
		return
	end
	local cursor
	local dist
	local cam_pos = self.view.current_camera:get_global_matrix():get_translation()

	for k,v in ipairs(list) do

		if v:get_type() == "CursorNode" then

			if not cursor or v:get_global_matrix():get_translation():distance_to(cam_pos) < dist then
				cursor = v
				dist = v:get_global_matrix():get_translation():distance_to(cam_pos)
			end
		end
	end

	if cursor then
		self.view.current_cursor = cursor
		self.view.current_cursor_matrix_cache = cursor:get_global_matrix()
		self.move_mode = "cursor_helper_move"
		
		self.cursor_selected:emit(cursor)
	end
end

function EditorMainWindow:leave_cursor_helper_mode(p_cancel)

	if p_cancel then
		self:restore_selection_cache()
	end

	self.cursor_released:emit(self.view.current_cursor, p_cancel)
	self.view.current_cursor = nil
	self.move_mode = "free"
	
end


function EditorMainWindow:process_match_list(list, camera_pos)

	local cache = {}
	-- sort
	local function comp(l, r)
	
		local ldist = cache[l] or l:get_global_matrix():get_translation():distance_to(camera_pos)
		local rdist = cache[r] or r:get_global_matrix():get_translation():distance_to(camera_pos)

		cache[l] = ldist
		cache[r] = rdist
		
		return ldist < rdist
	end
	table.sort(list, comp)

	-- remove duplicates and nodes that don't belong to our scene
	cache = {}
	for i=#list, 1, -1 do

		local obj = list[i]

		if not obj:get_owner() then
			table.remove(list, i)
		else
			if obj:get_owner() ~= self.scene then

				local parent = obj:get_parent()
				while parent do
					if parent:get_type() == "SceneNode" and parent:get_owner() == self.scene then
						break
					else
						parent = parent:get_parent()
					end
				end
				list[i] = parent
			end

			if (not list[i]) or cache[list[i]] then
				table.remove(list, i)
			else
				cache[list[i]] = true
			end
		end
	end

	-- look for last selected	
	local next
	if self.view.selection.last_selected then
	
		for k,v in ipairs(list) do

			if v == self.view.selection.last_selected then
				next = k+1
				break
			end
		end
		if next==#list then
			next = 1
		end
	end

	return list[next or 1]
end

function EditorMainWindow:add_ray(rbegin, rend)

	local prim = PrimitiveListNode.Primitive()
	prim.points = 2
	prim.vertices[0] = rbegin
	prim.vertices[1] = rend
	prim.use_colors = true
	prim.colors[0] = Color(255, 0, 0)
	prim.colors[1] = Color(255, 0, 0)
	prim.use_normals = false
	self.indicator:add_primitive(prim)
end

function EditorMainWindow:get_ray(x, y, distance)

	local size = self:get_painter():get_display_size()
	local w = size.x
	local h = size.y

	x = x / w
	y = y / h
	
	local proj = self.view.current_camera:get_projection_matrix()

	local ray = Vector3()
	ray.x = ( ( 2.0 * x ) - 1 ) / proj:get(0,0)
	ray.y = -( ( 2.0 * y ) - 1 ) / proj:get(1,1)

	ray.z = -1
	ray:normalize()

	return ray * distance
end

function EditorMainWindow:get_pick_list(p_pos)

	local pos = self.view.current_camera:get_global_matrix():get_translation()

	local dir = self:get_ray(p_pos.x, p_pos.y, 500)
	self.view.current_camera:get_global_matrix():transform(dir)

	local spatial_indexer = Renderer:get_singleton():get_spatial_indexer()

	local match_list = List_IAPIRef_()
	spatial_indexer:get_objects_in_segment(pos, dir, match_list)

	--self:add_ray(pos, dir)
	
	if match_list:size() == 0 then return end
	
	local tlist = {}
	local I = match_list:begin()
	while I do
	
		table.insert(tlist, I:get())
		I = I:next()
	end

	return tlist
end

function EditorMainWindow:pick_object(p_pos, shift)

	local pos = self.view.current_camera:get_global_matrix():get_translation()
	local tlist = self:get_pick_list(p_pos)
	
	if not tlist then
		return
	end

	obj = self:process_match_list(tlist, pos)
	if not obj then
		return
	end

	local selected = self:node_is_selected(obj)

	if not shift then
		self:remove_selection()
		if not selected then
			self:add_selection(obj)
		end
	else
		self:add_selection(obj)
	end
end


function EditorMainWindow:key(p_unicode, p_scan_code, p_press, p_repeat,p_modifier_mask)

	self.buttons[p_scan_code] = p_press

	local shift = not (p_modifier_mask == 0)

	if p_scan_code == KEY_m then

		if p_press then
			self:enter_move_mode()
		end
	end
	
	if p_scan_code == KEY_r then
	
		if p_press then
			self:enter_rotate_mode()
		end
	end
	
	if p_scan_code == KEY_ESCAPE then

		if p_press then
			self:leave_current_mode(true)
		end
	end

	if p_press then	
	
		if p_scan_code == KEY_DELETE then

			if shift then
				self:edit_skip_selected()
			else
				self:edit_delete_selected()
			end
		end
		
		if p_scan_code == KEY_F2 then
		
			self:edit_rename_node()
		end
	
		if p_scan_code == KEY_F8 then
			self:view_preview()
		end
		
		if p_scan_code == KEY_KP7 then -- arriba
			-- do something
			if shift then
				self.view.cam_rot_x = -math.pi / 2.0
				self.view.cam_rot_y = 0.0
			else
				self.view.cam_rot_x = math.pi / 2.0
				self.view.cam_rot_y = 0.0
			end
			self:update_cursor_camera()
		end
		
		if p_scan_code == KEY_KP3 then -- costados
		
			if shift then
				self.view.cam_rot_x = 0
				self.view.cam_rot_y = math.pi / 2.0
			else
				self.view.cam_rot_x = 0
				self.view.cam_rot_y = - math.pi / 2.0
			end
			self:update_cursor_camera()
		end
		
		if p_scan_code == KEY_KP1 then
		
			if shift then -- frente
				self.view.cam_rot_x = 0.0
				self.view.cam_rot_y = 0.0
			else
				self.view.cam_rot_x = 0.0
				self.view.cam_rot_y = math.pi
			end
			self:update_cursor_camera()
		end
		
		if p_scan_code == KEY_c and not p_repeat then
			print "collect!"
			collectgarbage("collect")
			collectgarbage("collect")
			collectgarbage("collect")
		end
		
		if p_scan_code == KEY_BACKQUOTE then
			Console:toggle()
		end
	end
end

local move_mode_planes = { "camera", "xy", "xz", "zy" }

function EditorMainWindow:toggle_move_mode_plane()

	if not self.move_mode_plane then
		self.move_mode_plane = move_mode_planes[#move_mode_planes]
	end

	for k,v in ipairs(move_mode_planes) do
	
		if v == self.move_mode_plane then
		
			if k == #move_mode_planes then
				k = 0
			end
			self.move_mode_plane = move_mode_planes[k+1]
			break
		end
	end

	self:restore_selection_cache()
	self.plane_display:set_text(self.move_mode_plane)

	--if self.view.move_list then self.view.move_list.mouse_pos = nil end
	--if self.view.rotate_list then self.view.rotate_list.mouse_pos = nil end
end


function EditorMainWindow:new_scene()

	local scene = SceneNode:new()
	scene:set_name("New Scene")
	self.scene_name = nil
	
	--[[
	local mesh = MeshNode:new()
	mesh:set_name("Cubito")
	local mrid = self.loop:get_renderer():test_cube_get_mesh()
	mesh:set_mesh(mrid)
	scene:add_child(mesh)
	mesh:set_foreign_parent(scene)
	--]]
	
	self:set_scene(scene)
	self:create_cubito()
end

function EditorMainWindow:node_is_selected(node)

	for k,v in ipairs(self.view.selection) do
	
		if node == v.node then
			return true
		end
	end
	
	return false
end

function EditorMainWindow:owned(node)

	if node == self.scene or node:get_owner() == self.scene then
	
		return true
	end
end

function EditorMainWindow:get_volume_size(list)

	if not list[1] then
		return Rect3()
	end
	
	local min
	local max
	
	local function add(p)
	
		if not min then min = Vector3(p) end
		if not max then max = Vector3(p) end

		if p.x > max.x then max.x = p.x end
		if p.y > max.y then max.y = p.y end
		if p.z > max.z then max.z = p.z end

		if p.x < min.x then min.x = p.x end
		if p.y < min.y then min.y = p.y end
		if p.z < min.z then min.z = p.z end
	end

	local function add_aabb(aabb, mat)

		for x = 0,1 do
		
			for y = 0,1 do
			
				for z = 0,1 do
				
					local vec = Vector3(aabb.pos)
					if x==1 then vec.x = vec.x + aabb.size.x end
					if y==1 then vec.y = vec.y + aabb.size.y end
					if z==1 then vec.z = vec.z + aabb.size.z end
					mat:transform(vec)
					add(vec)
				end
			end
		end
	end
	
	for k,v in ipairs(list) do
	
		local mat = v[2]
		local aabb = v[1]

		add_aabb(aabb, mat)
		--add(v.pos)
		--add(v.pos + v.size)
	end

	local ret = Rect3()
	ret.pos = min
	ret.size = max - min
	
	return ret
end

function EditorMainWindow:get_last_selected()

	return self.view.selection.last_selected
end


function EditorMainWindow:get_scene_volume(node)

	local volume_list = {}

	local function add(n, root)

		if n == root then return end
	
		local relmat = root:get_global_matrix():inverse() * n:get_global_matrix()
		
		if n:get_owner() == node and n:is_type("MeshNode") and n:get_mesh() then

			local vol = n:get_mesh():get_AABB()
			table.insert(volume_list, { vol, relmat })
		end
		if n:get_type() == "SceneNode" then
			table.insert(volume_list, { self:get_scene_volume(n), relmat } )
		end
	end
			
	traverse_scene(node, add, node)

	return self:get_volume_size(volume_list)
end


function EditorMainWindow:add_selection(node)

	if not node then return end
	
	self.view.selection.last_selected = node
	
	if self:node_is_selected(node) then

		return
	end
	
	-- adding selection mesh
	local highlight = MeshNode:new()
	highlight:set_mesh(self.view.selected_mesh)
	
	highlight:set_name(node:get_name().." Selected")

	local aabb
	
	local volume_list = {}
	if node:is_type("VisualNode") then
		aabb = node:get_local_AABB()
	else
		--if node:get_type() == TYPE_SCENE then

			aabb = self:get_scene_volume(node)
		--end
	end

	local mat = Matrix4()

	mat:set_translation(aabb.pos)
	mat:scale(aabb.size)

	node:add_child(highlight)
	
	highlight:set_local_matrix(mat)
	
	table.insert(self.view.selection, {node = node, highlight = highlight})

	node.history = node.history or { node }
	node.history.current = 1

	self.iapi_editor:load_iapi(self.view.selection.last_selected, self.icons[self.view.selection.last_selected:get_type()])

	local pressed = self.view.selection.last_selected.history[node.history.current].custom_editor
	self.scene_button_edit:set_pressed(pressed)
	self:editor_mode_toggle(pressed)
	
	self:update_tree_selection()
end

function EditorMainWindow:update_cursor_camera()

	local mat = Matrix4()
	mat:set_translation(self.view.cursor:get_global_matrix():get_translation())
	mat:rotate_y(self.view.cam_rot_y)
	mat:rotate_x(self.view.cam_rot_x)
	self.view.cursor:set_local_matrix(mat)
	orthonormalize_node(self.view.cursor)

	if self.view.cursor_distance < 0.0 then
		self.view.cursor_distance = 0.0
	end
	local mat = Matrix4()
	mat:translate(0, 0, self.view.cursor_distance)
	self.view.current_camera:set_local_matrix(mat)
end


function EditorMainWindow:remove_selection(node, no_emit)
	
	if not node then
	
		for i=#self.view.selection,1, -1 do
			self:remove_selection(i, true)
			self:update_tree_selection()
		end
		return
	end

	if type(mode) == 'number' then
		if not self.view.selection[node] then
			return
		end
	end

	for k,v in ipairs(self.view.selection) do

		if node == v.node then
			node = k
			break
		end
	end
	
	if type(node) == 'number' then
		-- remove bounding box thing
		self.view.selection[node].highlight:get_parent():remove_child(self.view.selection[node].highlight)
		if self.view.selection.last_selected == self.view.selection[node].node then
			self.view.selection.last_selected = nil
			self.iapi_editor:load_iapi(nil)
		end
		table.remove(self.view.selection, node)
	end
	
	if not no_emit then -- doble_negative because parameter defaults to false
	
		self:update_tree_selection()
	end
end



function EditorMainWindow:set_scene(scene)

	self:reset_view_nodes()
	if self.scene then
		self.view.root:remove_child(self.scene)
		self.scene:set("script", nil)
		self.scene = nil
	end
	self.scene = scene

	if not scene:get_parent() then
		self.view.root:add_child(scene)
	end

	--print_scene(scene)

	self.move_mode = "free"
	self:toggle_move_mode_plane()

	self:remove_selection()
	self:add_selection(scene)

	self:update_tree()
end

function EditorMainWindow:update_tree()

	local show_whole_tree = false

	self.update_tree_queued = false

	self.tree:clear()
	if not self.scene then return end

	local function traverse(node, parent_item)

		if not show_whole_tree and (node ~= self.scene and node:get_owner() == nil) then
			return
		end

		local it = self.tree:create_item(parent_item)
		it:set_text(0, node:get_name())
		if self.icons[node:get_type()] then
			it:set_bitmap(0, self.icons[node:get_type()])
		end

		-- the node and the item are buddies
		it.node = node
		node.tree_item = it
		
		if not self:owned(node) then
			it:set_custom_color(0, GUI.Color(255, 0, 0))
		end

		it.selected_signal:connect(self, self.tree_item_selected, it)
		local childs = node:get_child_count()

		for i=0,childs-1 do
		
			traverse(node:get_child_by_index(i), it)
		end
	end

	if show_whole_tree then
		traverse(self.view.root)
	else
		traverse(self.scene)
	end
	
	self:update_tree_selection()
end

function EditorMainWindow:update_tree_selection()

	self.updating_tree = true

	local function check(it)

		if not it then
			return
		end
		
		if self:node_is_selected(it.node) then
		
			it:select(0)
		else
			it:deselect(0)
		end
		
		local child = it:get_childs()
		while child do
		
			check(child)
			child = child:get_next()
		end
	end	

	check(self.tree:get_root_item())
	
	self.updating_tree = false
end

function EditorMainWindow:tree_item_selected(col, item)

	if self.updating_tree then return end

	self.updating_tree = true
	
	self:remove_selection()
	self:add_selection(item.node)

	self.updating_tree = false
end

function EditorMainWindow:load_scene(dbid)

	local scene
	scene = self:open_scene(dbid)
	if not scene then return end
	
	self:set_scene(scene)
	self.scene_name = IAPI_DB:get_singleton():get_path(scene:get_ID())
end

function EditorMainWindow:file_open_scene(dbid)

	if not dbid then
		self.question_pending = self.file_open_scene
		self.question_dialog:show("Current scene will be lost. Are you sure?")
		return
	end

	if self.question_pending == self.file_open_scene and type(dbid) == "number" then
	
		if dbid == 1 then
	
			self.open_scene_dialog:show()
			self.question_pending = nil
			return
		end
	end

	if type(dbid) == "number" then

		self:load_scene(dbid)
	end
end

function EditorMainWindow:file_import_scene(fname)

	if type(fname) == 'number' and fname == 1 then
		self.import_file_dialog:show(GUI.FileDialog.MODE_OPEN)
	elseif type(fname) == 'string' then
		self:import_scene(fname)
	else
		self.question_pending = self.file_import_scene
		self.question_dialog:show("Current scene will be lost. Are you sure?")
	end
end

function EditorMainWindow:import_scene(fname)

	local scene
	local flags = {}

	if string.find(string.lower(fname), "%.dae$") then

		--scene = import_dae(fname, self.loop:get_render(), self.view.root)

		---[[

		scene = Collada:import_scene(fname, self.scene)
		--]]
	end
	
	if string.find(string.lower(fname), "%.lua$") then
		scene = lua_import_scene(fname, self.scene)
	end

	if not scene then
		print("unable to open scene", fname)
		return
	end

	self:update_tree()
	
	if not self.scene then
		self:set_scene(scene)
	end
	--self.scene_name = nil
	--self.import_dialog:show()
	--self.import_dialog:load_scene(scene, flags)
end

function EditorMainWindow:open_scene(dbid)

	local scene = IAPI_Persist:get_singleton():load(dbid)

	if not scene then
		print("error loading scene "..dbid)
		self.question_pending = nil
		self.question_dialog:show("Error loading scene "..tostring(dbid))
		return nil
	end
	
	return scene
end

function EditorMainWindow:create_scene_instance(dbid)

	if dbid then

		local parent = self:get_last_selected() or self.scene
		local scene = self:open_scene(dbid)
		if not scene then return end
		scene:set_scene_instance(true)
		parent:add_child(scene)
		scene:set_owner(self.scene)

		local mat = Matrix4()
		mat:set_translation(self.view.cursor:get_global_matrix():get_translation())
		scene:set_global_matrix(mat)
		
		self:remove_selection()
		self:add_selection(scene)
	
		self:update_tree()
	else
		self.instance_scene_dialog:show()
	end
end

function EditorMainWindow:file_save_as()

	self:save()
end

function EditorMainWindow:file_save_scene(fname)

	self:save(self.scene_name)
end

function EditorMainWindow:save(sname, temporary)

	if not self.scene then return end

	if not sname then
		self.save_scene_dialog:show("Scene name", self.scene:get_name())
	else
	
		-- save
		--fname = fname or self.scene:get_resource_path()
		
		if not temporary then
		
			self.scene_name = sname
			local name = self.scene_name
			if string.find(name, "/") then
				name = string.gsub(name, "/*%s*$", "")
				name = string.match(name, "/(.*)%s*$")
			end
			self.scene:set_name(name)
			self:update_tree()
		end

		self.scene:presave_owned_shared_data(self.scene_name)
		
		local err = IAPI_Persist:get_singleton():save(self.scene, self.scene_name)
		if err ~= OK then
		
			print("error saving scene", err)
			self.question_pending = nil
			self.question_dialog:show("Error "..tostring(err).." saving scene")
		else
		
			print("saved scene with id ", self.scene:get_ID())
		end
	end
end


function EditorMainWindow:import_scene_done(scene)

	--self:set_scene(scene)
	self.scene_name = nil
	self:update_tree()
end

function EditorMainWindow:menu_slot(item, menu)

	print("selected "..menu[item][1])
	menu[item][2](self, unpack(menu[item], 3, #menu[item]))
end

function EditorMainWindow:view_node_properties()

	self.asset_property_stack:raise_frame(self.iapi_editor)
end

function EditorMainWindow:view_memory()

	collectgarbage('collect')
	print("lua: ", collectgarbage('count'))
	print("total: ", Memory:get_static_mem_usage())
	self.memory_debug_window:show()
end

function EditorMainWindow:view_preview()

	if not self.scene_name then
		return
	end

	local b = 1
	while arg[b-1] do
		b = b - 1
	end

	local cmd = arg[b].." "..self.project_path.." "..self.scene_name

	print("running command ", cmd)
	os.execute(cmd)	
end

function EditorMainWindow:file_new_scene(ans)

	if not ans then
		self.question_pending = self.file_new_scene
		self.question_dialog:show("Current scene will be lost. Are you sure?")
		return
	end
	
	if ans ~= 1 then
		return
	end

	self:new_scene()
end

function EditorMainWindow:edit_rename_node(new_name)

	local node = self:get_last_selected()
	if not node then return end

	if not new_name then
		self.rename_dialog:show("Rename node", node:get_name())
		return
	end
	
	if new_name == "" then
		return
	end
	
	if node == self.scene then

		self.scene:set_name(new_name)
		self:update_tree()
		return
	end
	
	local parent = node:get_parent()
	if parent:has_child(new_name) and parent:get_child(new_name) ~= node then
	
		local b,e,n = string.find(new_name, "%.(%d+)$")
		n = tonumber(n) or 0
		
		local base = new_name
		if b then
			base = new_name:sub(1, b-1)
		end
		
		while parent:has_child(base.."."..n) do
		
			n = n+1
		end
		
		self.rename_dialog:show("Name Already exists", base.."."..n)
		return
	end

	-- name is valid

	node:set_name(new_name)
	self:update_tree()
end

function EditorMainWindow:file_quit(ans)

	if not ans then

		self.question_pending = self.file_quit
		self.question_dialog:show("Current scene will be lost. Are you sure?")
	else
		if ans == 1 then
			self.loop:request_quit(true)
		end
	end
end

function EditorMainWindow:edit_skip_selected(ans)

	if not ans then
	
		self.question_pending = self.edit_skip_selected
		self.question_dialog:show("Skip node: are you sure?")
		return
	end
	
	if ans ~= 1 then
		return
	end

	local list = self:compute_transform_list()
	
	for k,v in ipairs(list) do
		if self:owned(v.node) then
			self:remove_selection(v.node)
			
			self:skip_node(v.node)
		end
	end
	
	self:update_tree()
end

function EditorMainWindow:skip_node(node)

	local parent = node:get_parent()
	if not parent then
		ERROR("Skipping node without parent")
		return
	end

	local cc = node:get_child_count()

	for i=cc-1,0,-1 do
	
		local c = node:get_child_by_index(i)
		local mat = c:get_global_matrix()
		local owner = c:get_owner()
		c = node:take_child(c)
		parent:add_child(c)
		c:set_global_matrix(mat)
		c:set_owner(owner)
	end
	parent:take_child(node)
end

function EditorMainWindow:edit_delete_selected(ans)

	if not ans then
	
		self.question_pending = self.edit_delete_selected
		self.question_dialog:show("Delete node: are you sure?")
		return
	end
	
	if ans ~= 1 then
		return
	end

	local list = self:compute_transform_list()
	
	for k,v in ipairs(list) do
		if self:owned(v.node) then
			self:remove_selection(v.node)
			v.node:get_parent():remove_child(v.node)
		end
	end
	
	self:update_tree()
end

function EditorMainWindow:edit_duplicate_selected()

	local node = self:get_last_selected()
	
	if node == self.scene then
		return
	end
	
	if not self:owned(node) then
	
		return
	end
	
	if node:get_type() ~= "SceneNode" then
		return
	end

	local id = node:get_instanced_from_ID()
	if id == OBJ_INVALID_ID then
		return
	end
	
	local scene = IAPI_Persist:get_singleton():load(id)
	if not scene then
		print("error loading ", fname)
		return
	end
	scene:set_scene_instance(true)
	scene:set_owner(self.scene)
	node:get_parent():add_child(scene)
	scene:set_global_matrix(node:get_global_matrix())
	self:remove_selection()
	self:add_selection(scene)
	self:update_tree()
end

function EditorMainWindow:edit_reparent_node(new_parent)

	local child = self:get_last_selected()
	if not child then return end
	
	if new_parent then

		local old_parent = child:get_parent()
		local old_owner = child:get_owner()
		local mat = Matrix4(child:get_global_matrix())
		old_parent:take_child(child)
		child:set_owner(old_owner)
		
		local ret = new_parent:add_child(child)
		if ret then
			print "error, unable to reparent"
			old_parent:add_child(child)
			return
		end
		
		child:set_global_matrix(mat)

		
		self:update_tree()
	else
		self.reparent_dialog:show(self.scene, child)
	end
end

function EditorMainWindow:create_bsp_collision()

	if not self.scene then
		return
	end

	local function bsp_gen(node)
	
		if node:get_type() == "MeshNode" and self:owned(node) then

			print("creating bsp for ", node:get_name())		
			NodeHelpers:create_bsp_collision_nodes_from_visual_node(node)
		end
	end

	traverse_scene(self.scene, bsp_gen)

	self:update_tree()	
end

function EditorMainWindow:create_cubito()

	local node = MeshNode:new()
	node:set_mesh(Renderer:get_singleton():get_test_cube())
	node:set_name("Cubito")

	local mat = Matrix4()
	mat:scale(0.3, 1.2, 3)
	node:set_local_matrix(mat)
	
	self.scene:add_child(node)
	node:set_owner(self.scene)
	
	self:update_tree()
end

function EditorMainWindow:create_room()
	
	local node = RoomNode:new()
	
	local p = self:get_last_selected() or self.scene
					
	p:add_child(node)
	node:set_owner(self.scene)
	
	self:update_tree()
end

function EditorMainWindow:create_particle_system()
	
	local node = ParticleSystemNode:new()
	node:set_name("Particles")

	local p = self:get_last_selected() or self.scene
	p:add_child(node)
	node:set_owner(self.scene)

	self:update_tree()
end

function EditorMainWindow:create_spline_node()

	local node = SplineNode:new()
	node:set_name("Spline")

	local p = self:get_last_selected() or self.scene
	p:add_child(node)
	node:set_owner(self.scene)

	self:update_tree()
end

function EditorMainWindow:instance_type_selected(p_type, at_cursor)

	local ip = IAPI_Persist:get_singleton()
	local node = ip:instance_type(p_type)
	
	if not node or not node:is_type("Node") then
	
		print("error! type "..p_type.." invalid")
		return
	end
	
	local p = self:get_last_selected() or self.scene

	if at_cursor then
		node:set_global_matrix(self.view.cursor:get_global_matrix())
	end

	node:set_name("New "..p_type)
	p:add_child(node)
	node:set_owner(self.scene)

	self:update_tree()
end

function EditorMainWindow:create_cursor_node()

	local node = CursorNode:new()
	node:set_name("new cursor")
	node:set_global_matrix(self.view.cursor:get_global_matrix())

	SceneMainLoop:get_singleton():get_scene_tree():add_child(node)
end

function EditorMainWindow:create_animation_node()

	local node = AnimationNode:new()
	node:set_name("Animation")
	
	local p = self:get_last_selected() or self.scene
	p:add_child(node)
	node:set_owner(self.scene)
	
	self:update_tree()
end

function EditorMainWindow:init_menues()

	local sep = true

	local function init_menu(mbox, menu)

		mbox.item_selected_signal:connect(self, self.menu_slot, menu)
		for k,v in ipairs(menu) do
		
			if v == sep then
				mbox:add_separator()
			else
				mbox:add_item(v[1], k)
			end
		end
	end
	
	local file = {
		{"&New", self.file_new_scene},
		{"&Open", self.file_open_scene},
		{"&Save", self.file_save_scene},
		{"Save &As...", self.file_save_as},
		sep,
		{"&Import Scene...", self.file_import_scene},
		{"&Quit", self.file_quit},
	}

	init_menu(self.file_menu, file)

	local create = {
		{"&Instance...", self.create_instance},
		{"&Mesh node", self.create_mesh_node},
		{"&Light", self.create_light},
		{"&Camera", self.create_camera},
		{"&Dummy", self.create_dummy},
		{"&AutoTransform", self.create_auto_transform},
		{"&InfluenceZone", self.create_influence_zone},
		{"&SceneInstance", self.create_scene_instance},
  		{"&BSPColision from VisualNode", self.create_bsp_collision},
		{"Cu&bito", self.create_cubito },
		{"Room", self.create_room},
		{"&Particle System", self.create_particle_system },
		{"A&nimation Node", self.create_animation_node },
		{"Cursor Node", self.create_cursor_node },
		{"Spline Node", self.create_spline_node },
	}
	
	init_menu(self.create_menu, create)

	local edit = {
	
		{"Select &All", self.edit_select_all},
		{"Du&plicate Selected", self.edit_duplicate_selected},
		{"De&lete Selected", self.edit_delete_selected},
		{"S&kip Selected", self.edit_skip_selected},
		{"&Deselect All", self.edit_deselect_all},
		sep,
		{"&Rename...", self.edit_rename_node},
		{"Re&parent...", self.edit_reparent_node},
		{"&Translate Selected", self.edit_translate_selected},
		{"&Rotate Selected around center of mass", self.edit_rotate_center},
		{"Rotate Selected around &cursor", self.edit_rotate_cursor},
		{"Trans&form Menu", self.edit_transform_menu},
		{"&Scale Selected", self.edit_scale_selected},
		{"&Mirror Selected", self.edit_mirror_selected},
	}
	
	init_menu(self.edit_menu, edit)
	
	local view = {
		
		{"Scene &Tree", self.view_scene_tree},
		{"Node &Properties", self.view_node_properties},
		sep,
		{"Align to &Axis", self.view_align_to_axis},
		{"&Cursor to Selection", self.view_cursor_to_selection},
		{"&Toggle Orthogonal/Perspective", self.view_toggle_perspective},
		sep,
		{"&Scene Script", self.view_scene_script},
		sep,
		{"Pre&view", self.view_preview},
		{"Memory", self.view_memory},
	}
	
	init_menu(self.view_menu, view)
	
end

function EditorMainWindow:create_instance()

	self.instance_dialog:show()
end

function EditorMainWindow:question_answered(answer)

	if self.question_pending then
		self:question_pending(answer)
		self.question_pending = nil
	end
end

function EditorMainWindow:hide_show_panel(button, panel)

	local dir = button:get_dir()
	if dir == GUI.DOWN then
		dir = GUI.UP
	else
		dir = GUI.DOWN
	end

	if ( panel:is_visible() ) then
	
		panel:hide()
	else
		panel:show()
	end
	button:set_dir( dir )
end

function EditorMainWindow:iapi_selected(iapi)

	local sel = self:get_last_selected()
	if not sel then
		return
	end

	if not sel.history then
		ERROR("Invalid history on node?")
		return
	end
	
	sel.history.current = sel.history.current + 1
	sel.history[sel.history.current] = iapi
	
	while #sel.history > sel.history.current do
	
		table.remove(sel.history)
	end
end

function EditorMainWindow:editor_mode_toggle(toggled)

	local sel = self:get_last_selected()
	if toggled then

		if not sel then
			return
		end
		local type = sel:get_type()
		local ed = self.custom_editors[type]
		self.asset_property_stack:raise_frame(ed or self.invalid_editor)
		if ed then
			ed:load(sel)
		end
	else
	
		self.asset_property_stack:raise_frame(self.iapi_editor)
	end
	
	if sel.history[sel.history.current] then
		sel.history[sel.history.current].custom_editor = toggled
	end
end

function EditorMainWindow:light_toggled(toggled)

	if toggled then
		self.render:get_spatial_indexer():add_global_light(self.light)
	else
		self.render:get_spatial_indexer():remove_global_light(self.light)
	end
end

function EditorMainWindow:history(dir)

	local sel = self:get_last_selected()
	if not sel or not sel.history then
		return
	end
	
	local cur = sel.history.current + dir
	if not sel.history[cur] then
		return
	end
	
	sel.history.current = cur
	self.iapi_editor:load_iapi(sel.history[cur], self.icons[sel.history[cur]:get_type()])
	self.scene_button_edit:set_pressed(sel.history[sel.history.current].custom_editor)
end

function EditorMainWindow:init_gui()

	local gui =
		{ type = RootBox,
	
			{ type = GUI.HBoxContainer,

				{ type = {GUI.MenuBox, "&File"},
					name="file_menu",
				},

				{ type = {GUI.MenuBox, "&Create"},
					name="create_menu",
				},

				{ type = {GUI.MenuBox, "&Edit"},
					name="edit_menu",
				},

				{ type = {GUI.MenuBox, "&View"},
					name="view_menu",
				},

				{ type = {GUI.MenuBox, "&Settings"},
					name="settings_menu",
				},

				{ type = {GUI.Label, "Camera:"},
					name="camera_pos",
				},
				
				"Transform plane:",
				{ type = GUI.Label,
					
					name = "plane_display",
					add_args = {1},
				},
				
				{ type = { GUI.ArrowButton, GUI.DOWN },
				
					name = "right_panel_arrow",
					
					
				},
			
				init = function(widget)
					
					widget:set_stylebox_override(self:get_skin():get_stylebox(GUI.SB_ROOT_CONTAINER));

					--widget:set_style(self:get_skin():get_stylebox(GUI.SB_WINDOWBUTTON_NORMAL), true);
				end,
			},
			
			{ type = GUI.HBoxContainer,
				
				{ type = ViewPort,
				
					name = "viewport",
					add_args = {2},
					init = function(widget)
					
						widget:set_event_reciever(self)
					end
				},
					
				{ type = GUI.VBoxContainer ,
				
					name = "property_box",
					
					{ type = GUI.MarginGroup,
					
						name = "mgscene",
						init = function(widget)
							widget:set_label_text("Scene Tree:");
						end,
					
						{ type = GUI.ScrollBox,
		
							name = "tree_scroll_box",
							
							{ type = { GUI.Tree, 1 },
								name = "tree",
								init = function(self)
									if self.set_multi_mode then
										self:set_multi_mode(GUI.Tree.SELECT_ROW)
										self:set_minimum_size(GUI.Point(200,0))
									end
								end,
								add = function(widget, parent)
								
									parent:set_frame(widget)
								end,
							},
							
							add_args = {1},
							init = function(self)
								self:set_expand_h(true)
								self:set_expand_v(true)
							end,
						},
						add_args = {1},
					},
					
					{ type = GUI.MarginGroup,
					
						name = "mgprop",
						init = function(self)
							self:set_label_text("Properties:");
						end,
					
						{ type = GUI.StackContainer,
						
							name = "asset_property_stack",
							
							add_args = {1},
							
							{ type = { IAPIEditor, false, true },
								
								name = "iapi_editor",
								init = function(widget)
								
									widget.iapi_selected_signal:connect(self, self.iapi_selected)
								end,
							},
							
							{ type = { GUI.Label, "No editor available" },
								
								name = "invalid_editor",
							},
							
							{ type = { AnimEditor, self },
								name = "anim_editor",
							},
							
							{ type = { SplineEditor, self },
								
								name = "spline_editor",
							},
							
							add_args = {1},
						},
						
						add_args = {1},
					},
					add_args = {1},
					
				},				
				
				add_args = {1},
			},
		}
	
	local box = load_gui(self, gui)


	local rp = "res:/"
	self.scene_button_file = self.mgscene:get_label_hb():add( GUI.Widget:new(), 1) -- expand
	self.scene_button_light = self.mgscene:get_label_hb():add( GUI.Button:new( self:get_painter():load_bitmap(rp.."/icons/icon_light_toggle.png") ))
	self.scene_button_light:set_toggle_mode(true)
	self.scene_button_light.toggled_signal:connect(self, self.light_toggled)
	
	self.mgscene:get_label_hb():add( GUI.VSeparator:new()) -- expand
	
	self.scene_button_file = self.mgprop:get_label_hb():add( GUI.Widget:new(), 1) -- expand

	self.scene_button_edit = self.mgprop:get_label_hb():add( GUI.Button:new( self:get_painter():load_bitmap(rp .. "/icons/icon_edit.png") ))
	self.scene_button_edit:set_toggle_mode(true)
	self.scene_button_edit.toggled_signal:connect(self, self.editor_mode_toggle)

	self.iapi_button_back = self.mgprop:get_label_hb():add( GUI.MenuButton:new("", self:get_painter():load_bitmap(rp .. "/icons/icon_back.png") ))
	self.iapi_button_back.pressed_signal:connect(self, self.history, -1)
	self.iapi_button_forward = self.mgprop:get_label_hb():add( GUI.MenuButton:new("", self:get_painter():load_bitmap(rp .. "/icons/icon_forward.png") ))
	self.iapi_button_forward.pressed_signal:connect(self, self.history, 1)

	--self:set_no_stretch_root_frames(false)
	self:set_root_frame(box)
	
	self:init_menues()

	self.import_file_dialog = GUI.FileDialog:new(self)
	self.import_file_dialog.file_activated_signal:connect(self, self.import_scene)
	self.import_file_dialog:add_filter("Collada", ".dae")
	self.import_file_dialog:add_filter("Blender", ".lua")
	
	self.instance_scene_dialog = OpenSceneDialog:new(self)
	self.instance_scene_dialog.scene_selected_signal:connect(self, self.create_scene_instance)

	self.open_scene_dialog = OpenSceneDialog:new(self)
	self.open_scene_dialog.scene_selected_signal:connect(self, self.file_open_scene)

	self.save_scene_dialog = GUI.StringInputDialog:new(self)
	self.save_scene_dialog.entered_string_signal:connect( self, self.save )
	
	--self.tree_scroll_box:set_minimum_size( GUI.Point(200,0) )
	
	
	self.import_dialog = ImportSceneDialog:new(self)
	self.import_dialog.import_done:connect(self, self.import_scene_done)
	
	self.instance_dialog = InstanceDialog:new(self)
	self.instance_dialog.instance_type_selected:connect(self, self.instance_type_selected)

	self.rename_dialog = GUI.StringInputDialog:new(self)
	self.rename_dialog.entered_string_signal:connect(self, self.edit_rename_node)
	
	self.question_dialog = GUI.QuestionInputDialog:new(self)
	self.question_dialog:add_button(1, "Yes")
	self.question_dialog:add_button(0, "No")
	self.question_dialog.button_pressed_signal:connect(self, self.question_answered)

	self.reparent_dialog = ReparentDialog:new(self)
	self.reparent_dialog.new_parent_selected:connect(self, self.edit_reparent_node)
	
	self.right_panel_arrow.pressed_signal:connect( self, self.hide_show_panel, self.right_panel_arrow, self.property_box )
	--self.asset_property_stack:hide()
	--self.asset_property_stack:set_minimum_size( GUI.Point(200,0) )
	self.property_box:set_stylebox_override(self.property_box:get_window():get_skin():get_stylebox(GUI.SB_ROOT_CONTAINER));
	
	self.icons = {
		--icon_light.png
		MeshNode = self:get_painter():load_bitmap(rp .. "/icons/icon_mesh_old.png"),
		--icon_portal.png
		SceneNode = self:get_painter():load_bitmap(rp .. "/icons/icon_scene.png"),
	}
	self.memory_debug_window = MemoryDebugWindow:new(self)

	self.console = Console:new(self)

	self:add_custom_editor("AnimationNode", self.anim_editor)
	self:add_custom_editor("SplineNode", self.spline_editor)
end

function EditorMainWindow:add_custom_editor(type, editor)

	self.custom_editors[type] = editor
end

function EditorMainWindow:set_loop(loop)

	self.loop = loop
end

function EditorMainWindow:set_resource_path(rp)

	local fs = GUI.FileSystem:instance_func()
	rp = fs:get_abs_dir_path(rp)

	self.resource_path = rp
	
	local gb = GlobalVars:get_singleton()
	gb:set("path/resources", rp)

	gb:load_from_file("res://game.cfg")
	gb:load_from_file("res://game_override.cfg")
	IAPI_Persist:get_singleton():load_db()
	
	self.game_loop = require "init.lua"
	self.loop:set_game_loop( self.game_loop )
	self.game_loop = self.loop:get_game_loop()
end

function EditorMainWindow:create_selected_mesh()

	local render = Renderer:get_singleton()
	local mesh = render:create_mesh()
	--mesh:set_name("Scene selected mesh")

	local vertex_count = 48	
	
	local surface = render:create_surface()
	surface:create(Surface.PRIMITIVE_LINES, Surface.FORMAT_ARRAY_VERTEX, 0, vertex_count)

	local array = Variant(Variant.REAL_ARRAY, vertex_count * 3)

	local lines = {
		{0, 0, 0, 1, 0, 0},
		{0, 0, 0, 0, 1, 0},
		{0, 0, 0, 0, 0, 1},
		
		{1, 1, 0, 1, 0, 0},
		{1, 1, 0, 0, 1, 0},
		{1, 1, 0, 1, 1, 1},
		
		{1, 0, 1, 0, 0, 1},
		{1, 0, 1, 1, 1, 1},
		{1, 0, 1, 1, 0, 0},
		
		{0, 1, 1, 0, 0, 1},
		{0, 1, 1, 1, 1, 1},
		{0, 1, 1, 0, 1, 0},
	}
	
	local vid = 0
	for k,v in ipairs(lines) do
	
		local from = Vector3(v[1], v[2], v[3])
		local to = Vector3(v[4], v[5], v[6])

		local seg = (to - from) / 4
		local fcut = from + seg
		local tcut = to - seg

		array:array_set_real(vid*3+0, from.x)
		array:array_set_real(vid*3+1, from.y)
		array:array_set_real(vid*3+2, from.z)
		vid = vid+1

		array:array_set_real(vid*3+0, fcut.x)
		array:array_set_real(vid*3+1, fcut.y)
		array:array_set_real(vid*3+2, fcut.z)
		vid = vid+1
		
		array:array_set_real(vid*3+0, tcut.x)
		array:array_set_real(vid*3+1, tcut.y)
		array:array_set_real(vid*3+2, tcut.z)
		vid = vid+1

		array:array_set_real(vid*3+0, to.x)
		array:array_set_real(vid*3+1, to.y)
		array:array_set_real(vid*3+2, to.z)
		vid = vid+1
		-- maybe add colors?
	end

	surface:set_array(Surface.ARRAY_VERTEX, array)
	surface:set_material(render:get_indicator_material())
	mesh:add_surface(surface)

	return mesh
end

function EditorMainWindow:reset_view_nodes()

	self.view.current_camera = self.view.cursor_camera
	self.view.cursor:set_local_matrix(Matrix4()) -- identity
	self.view.cursor_distance = 10.0
	self.view.cam_rot_x = 0.0
	self.view.cam_rot_y = 0.0
	self:update_cursor_camera()
	self:remove_selection()
	self.view.current_camera:set_as_current()
end

function EditorMainWindow:create_view()

	local root = SceneNode:new()
	self.game_loop:set_scene_tree(root)
	
	local view = {}
	view.root = root

	view.selected_mesh = self:create_selected_mesh()

	view.cursor = DummyNode:new()
	view.cursor:set_name("Cursor")

	--[[
	view.cursor_mesh = MeshNode:new()
	view.cursor_mesh:set_name("Cursor Mesh")
	view.cursor_mesh:set_mesh(view.selected_mesh)
	view.cursor:add_child(view.cursor_mesh)
	--]]
	
	view.cursor_camera = CameraNode:new()
	view.cursor_camera:set_name("Editor Camera")
	
	local proj = Matrix4()
	local mode = Main:get_singleton():get_video_mode()
	proj:set_projection( 45.0, mode.width/mode.height, 1, 500.0 )

	--proj:set_frustum(-0.05, 0.05, -0.05, 0.05, 0.05, 100)
	view.cursor_camera:set_projection_matrix(proj)

	view.cursor:add_child(view.cursor_camera)
	
	view.current_camera = view.cursor_camera
	
	root:add_child(view.cursor)

	---[[
	self.light = self.render:create_light()
	self.light:set_mode(Light.MODE_DIRECTIONAL)
	self.light:set_ambient(Color(130, 130, 130))
	self.light:set_diffuse( Color(190,190,190) )
	self.light:set_specular( Color(200,200,200) )
	local lightm = Matrix4()
	lightm:rotate_y(math.pi/5);
	lightm:rotate_x(-(math.pi/2 + math.pi/6));
	self.light:set_matrix(lightm)
	--]]

	view.selection = {}
	
	self.view = view
	view.current_camera:set_as_current()
	
	local ind = Indicator:new()
	root:add_child(ind)
	self.indicator = ind
end

function EditorMainWindow:queue_update_tree()

	if self.update_tree_queued then
		return
	end

	self.update_tree_queued = true	
	self:get_root().update_signal:connect(self, self.update_tree)
end

function EditorMainWindow:scene_changed(node)

	if node:get_owner() then
		self:queue_update_tree()
	end
end

function EditorMainWindow:init()

	--self.anim_iapi = AnimEditor:new(self.scene_main)
	--self.anim_editor:load_iapi(self.anim_iapi)

	self.render = Renderer:get_singleton()
	--self.render:set_default_clear_color( Color(80,80,90,255) )

	--self.game_loop.node_enter_scene_signal:connect(self, self.scene_changed);
	--self.game_loop.node_exit_scene_signal:connect(self, self.scene_changed);
	
	self:create_view()	

	--[[
	local db = require "db_custom.lua"
	if type(db) == "string" then
		db = require "db.lua"
	end
	IAPI_Persist:get_singleton():set_db(db)
	--]]

	local cmd_file
	if arg[1] then
		if string.find(arg[1], "/$") then
			cmd_file = arg[2]
		else
			cmd_file = arg[1]
		end
	end

	if cmd_file then
		if string.find(string.lower(cmd_file), ".dae$") or string.find(string.lower(cmd_file), ".lua$") then
			local abs = self.file_system:get_abs_file_path(cmd_file)
			self:import_scene(abs)
		else
			self:load_scene(cmd_file)
		end
	else
		self:new_scene()
	end
	
	self.key_unhandled_signal:connect(self, self.key)
end

function EditorMainWindow:__init__(arg, resource_path, loop)

	self.cursor_selected = LuaSignal(1)
	self.cursor_released = LuaSignal(2)

	self.buttons = {}
	
	self.custom_editors = {}
	self:init_gui()
	
	self.resource_path = GlobalVars:get_singleton():get("path/resources")
	self.file_system = GUI.FileSystem:instance_func()

	self.loop = Main:get_singleton():get_main_loop()

	local cmd_file
	if arg[1] and string.find(arg[1], "/$") then
		self.project_path = arg[1]
	end
	
	self.project_path = self.project_path or os.getenv("PROJECT_PATH")
	
	if self.project_path then
		self:set_resource_path(self.project_path)
	else
		self:set_resource_path("testdata")
	end
end

return EditorMainWindow

