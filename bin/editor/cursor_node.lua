class ("CursorNode", Lua__MeshNode)

function CursorNode:get_type()

	return "CursorNode"
end

function CursorNode:set_edit_target(p_target)

	self.target = p_target
end

function CursorNode:get_edit_target()

	return self.target	
end

function CursorNode:get_cursor_mesh()

	local size = 0.1
	if not self.cursor_mesh then
	
		local r = Renderer:get_singleton()
		local m = r:create_mesh()
		local s = r:create_surface()
		
		s:create(Surface.PRIMITIVE_LINES, bw_or(Surface.FORMAT_ARRAY_VERTEX, Surface.FORMAT_ARRAY_COLOR), 0, 6);
		
		local va = Variant(Variant.REAL_ARRAY, 6 * 3)
		local ca = Variant(Variant.INT_ARRAY, 6)

		local vlist = { size, 0, 0,	-size, 0, 0,
						0, size, 0,	0, -size, 0,
						0, 0, size,	0, 0, -size,
					}
		local clist = { Color(255, 0, 0), Color(255, 0, 0),
						Color(0, 255, 0), Color(0, 255, 0),
						Color(0, 0, 255), Color(0, 0, 255),
					}
		for k,v in ipairs(vlist) do
			va:array_set_real(k-1, v)
		end
		
		for k,v in ipairs(clist) do
			ca:array_set_int(k-1, v:to_32())
		end
		
		s:set_array(Surface.ARRAY_VERTEX, va)
		s:set_array(Surface.ARRAY_COLOR, ca)
	
		s:set_material(r:get_indicator_material())
		
		m:add_surface(s)
		
		self.cursor_mesh = m
	end
	
	return self.cursor_mesh
end

function CursorNode:__init__()

	self:set_mesh(CursorNode:get_cursor_mesh())
end

return CursorNode

