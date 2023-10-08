function traverse_scene(node, f, ...)
--print"traverse"
	f(node, ...)

	local count = node:get_child_count()
	for i=0,count-1 do
		local child = node:get_child_by_index(i)
		if child then
			traverse_scene(child, f, ...)
		end
	end
end

function strip_resource_path(path, resource_path)

	local rp = resource_path or GlobalVars:get_singleton():get("path/resources")
	rp = string.gsub(rp, "\\", "/")
	path = string.gsub(path, "\\", "/")
	if not string.match(rp, "/$") then
		rp = rp .. "/"
	end
	print("rp is ", rp, "sub is ", path:sub(1, rp:len()))
	if path:sub(1, rp:len()) == rp then
		return path:sub(rp:len()+1)
	else
		return nil
	end
end

function orthonormalize_matrix(mat)

	do return end
	local loc = mat:get_translation()
	-- This seemed to be the only way to get rid of precision issues...
	-- tried EVERYTING, and setting/getting euler worked the best.
	local _,pitch,yaw,roll=mat:get_euler_rotation(0.0,0.0,0.0);
	mat:set_euler_rotation(pitch,yaw,roll);
	mat:set_translation(loc)
	--mat:orthonormalize()
end

function orthonormalize_node(node)

	local mat = node:get_local_matrix()
	orthonormalize_matrix(mat)
	node:set_local_matrix(mat)
end


function split (s,t)
 local l = {n=0}
 local f = function (s)
  l.n = l.n + 1
  l[l.n] = s
  return ""
 end
 local p = "%s*(.-)%s*"..t.."%s*"
 s = string.gsub(s,"^%s+","")
 s = string.gsub(s,"%s+$","")
 s = string.gsub(s,p,f)
 l.n = l.n + 1
 l[l.n] = string.gsub(s,"(%s%s*)$","")
 return l
end
