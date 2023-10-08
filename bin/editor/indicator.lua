class("Indicator", PrimitiveListNode)

function Indicator:__init__()

	local prim = PrimitiveListNode.Primitive()
	prim.points = 2
	prim.vertices[0] = Vector3(100, 0, 0)
	prim.vertices[1] = Vector3(-100, 0, 0)
	prim.use_colors = true
	prim.colors[0] = Color(255, 0, 0)
	prim.colors[1] = Color(255, 0, 0)
	prim.use_normals = false
	self:add_primitive(prim)

	local prim = PrimitiveListNode.Primitive()
	prim.points = 2
	prim.vertices[0] = Vector3(0, 100, 0)
	prim.vertices[1] = Vector3(0, -100, 0)
	prim.use_colors = true
	prim.colors[0] = Color(0, 255, 0)
	prim.colors[1] = Color(0, 255, 0)
	prim.use_normals = false
	self:add_primitive(prim)
	
	local prim = PrimitiveListNode.Primitive()
	prim.points = 2
	prim.vertices[0] = Vector3(0, 0, 100)
	prim.vertices[1] = Vector3(0, 0, -100)
	prim.use_colors = true
	prim.colors[0] = Color(0, 0, 255)
	prim.colors[1] = Color(0, 0, 255)
	prim.use_normals = false
	self:add_primitive(prim)

	self:set_material(Renderer:get_singleton():get_indicator_material())
end
