//
// C++ Interface: material
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MATERIAL_H
#define MATERIAL_H

#include "iapi.h"
#include "render/texture.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class Material : public IAPI {
	
	IAPI_TYPE(Material,IAPI);	
public:
	
	/* Material IAPI must implement the following properties:
	
		"type":String - type of material. "default" is the cross-platform portable material. Other implementations may add other types of materials with extended properties.
		
		platform independed properties: (default)

		"layer":int - drawing order (< 0 draws first)
		
		"color/ambient":Color - ambient color
		"color/diffuse":Color - diffuse color
		"color/specular":Color - specular color
		"color/emission":Color - emission color
		
		"flags/visible":bool - is visible
		"flags/double_sided":bool - is double sided (front&back)
		"flags/invert_normals":bool - swap front/back
		"flags/shaded":bool - affected by light
		"flags/draw_z":bool - write to z buffer
		"flags/test_z":int - test_z mode
		"flags/wireframe":bool - draw as wireframe
		
		"vars/transparency":real - transparency 0 -> 1 (invisible)
		"vars/shininess":real - shininess 0->1
		"vars/line_thickness":real line thickness 0->1	
		"vars/normalmap_depth":real normalmap depth 0->1
		"vars/glow":real - glow level
	
		textures/diffuse:iobj(Texure) diffuse texture (overrides diffuse color)
		textures/reflection:iobj(Texure) reflection texture
		textures/normalmap:iobj(Texure) normalmap texture (surface must has binormal and tangent arrays also)
		textures/specular:iobj(Texure) 	- specularity texture (overrides specular color)
		shader:IOBJ(Shader) - a shader can be added to a material, which will also be applied to the surface it's been assigned to
		next_pass:IOBJ(Material) - material for next pass
	*/	
	
	virtual int get_layer() const=0;
	virtual bool has_alpha() const=0;
	virtual bool has_scissor_enabled() const=0;
	
	Material();
	~Material();

};

#endif
