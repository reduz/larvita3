#ifdef OPENGL_ENABLED
//
// C++ Implementation: material_gl
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "material_gl.h"
#include "renderer_gl2.h"

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
	"flags/test_z":bool - test against z buffer
	"flags/wireframe":bool - draw as wireframe
	"flags/glow":bool - send to glow buffer (if glow exists)
		
	"vars/transparency":real - transparency 0 -> 1 (invisible)
	"vars/shininess":real - shininess 0->1
	"vars/line_thickness":real line thickness 0->1	
	"vars/normalmap_depth":real normalmap depth 0->1
	
	textures/diffuse:iobj(Texure) diffuse texture (overrides diffuse color)
	textures/reflection:iobj(Texure) reflection texture
	textures/normalmap:iobj(Texure) normalmap texture (surface must has binormal and tangent arrays also)
	textures/specular:iobj(Texure) 	- specularity texture (overrides specular color)
	
	next_pass:IOBJ(Material) - material for next pass
*/		

#define _VAR_SET(m_path,m_var)\
	if (p_path==m_path) {\
		m_var=p_value;\
		return;\
	}

#define _VAR_SET_V(m_path,m_var,m_val)\
	if (p_path==m_path) {\
		m_var=m_val;\
		return;\
	}

#define _VAR_SETT(m_path,m_var)\
	if (p_path==m_path) {\
		m_var=IObjRef(p_value);\
		return;\
	}
		
		
		
#define _VAR_GET(m_path,m_var)\
	if (p_path==m_path)\
		return m_var;
		
#define _VAR_GETT(m_path,m_var)\
	if (p_path==m_path)\
		return IObjRef(m_var);

#define CLAMP(val, min, max) \
	(val < min ? min : val) > max ? max : val

void Material_GL::set(String p_path,const Variant& p_value) {

	_VAR_SET("layer", layer);
	
	_VAR_SET("color/ambient",color_ambient);
	_VAR_SET("color/diffuse",color_diffuse);
	_VAR_SET("color/specular",color_specular);
	_VAR_SET("color/emission",color_emission);
	
	_VAR_SET("flags/visible",flags.visible);
	_VAR_SET("flags/double_sided",flags.double_sided);
	_VAR_SET("flags/invert_normals",flags.invert_normals);
	_VAR_SET("flags/shaded",flags.shaded);
	_VAR_SET("flags/draw_z",flags.draw_z);
	_VAR_SET("flags/test_z",flags.test_z);
	_VAR_SET("flags/wireframe",flags.wireframe);
	_VAR_SET("flags/scissor_alpha",flags.scissor_alpha);
	_VAR_SET("flags/emission_spherical",flags.emission_spherical);
	_VAR_SET("flags/glow_spherical",flags.glow_spherical);
	
	if (p_path=="vars/transparency") {
		vars.transparency=p_value;
		return;		
	}

	_VAR_SET("vars/transparency",vars.transparency);
	
	
	_VAR_SET_V("vars/shininess", vars.shininess, CLAMP(float(p_value), 0, 1));
	_VAR_SET("vars/line_thickness",vars.line_thickness);
	_VAR_SET("vars/normalmap_depth",vars.normalmap_depth);
	_VAR_SET("vars/scissor_alpha_treshold",vars.scissor_alpha_treshold);
	_VAR_SET("vars/glow", vars.glow);
	
	_VAR_SETT("textures/diffuse",textures.diffuse);
	_VAR_SETT("textures/specular",textures.specular);
	_VAR_SETT("textures/emission",textures.emission);
	_VAR_SETT("textures/reflection",textures.reflection);
	_VAR_SETT("textures/refmask",textures.refmask);
	_VAR_SETT("textures/normalmap",textures.normalmap);	
	_VAR_SETT("textures/glowmap",textures.glowmap);
	
	_VAR_SETT("shader",shader);	
	
	_VAR_SETT("next_pass",next_pass);	
	
	ERR_PRINT(String("Path: "+p_path+" not found").ascii().get_data());

	ERR_FAIL();
}

Variant Material_GL::get(String p_path) const {

	_VAR_GET("layer", layer);
	
	_VAR_GET("color/ambient",color_ambient);
	_VAR_GET("color/diffuse",color_diffuse);
	_VAR_GET("color/specular",color_specular);
	_VAR_GET("color/emission",color_emission);
	
	_VAR_GET("flags/visible",flags.visible);
	_VAR_GET("flags/double_sided",flags.double_sided);
	_VAR_GET("flags/invert_normals",flags.invert_normals);
	_VAR_GET("flags/shaded",flags.shaded);
	_VAR_GET("flags/draw_z",flags.draw_z);
	_VAR_GET("flags/test_z",flags.test_z);
	_VAR_GET("flags/wireframe",flags.wireframe);
	_VAR_GET("flags/scissor_alpha",flags.scissor_alpha);
	_VAR_GET("flags/glow_spherical",flags.glow_spherical);
	_VAR_GET("flags/emission_spherical",flags.emission_spherical);
	
	_VAR_GET("vars/transparency",vars.transparency);
	_VAR_GET("vars/shininess",vars.shininess);
	_VAR_GET("vars/line_thickness",vars.line_thickness);
	_VAR_GET("vars/normalmap_depth",vars.normalmap_depth);
	_VAR_GET("vars/scissor_alpha_treshold",vars.scissor_alpha_treshold);
	_VAR_GET("vars/glow", vars.glow);
	
	_VAR_GETT("textures/diffuse",textures.diffuse);
	_VAR_GETT("textures/specular",textures.specular);
	_VAR_GETT("textures/emission",textures.emission);	
	_VAR_GETT("textures/reflection",textures.reflection);
	_VAR_GETT("textures/refmask",textures.refmask);
	_VAR_GETT("textures/normalmap",textures.normalmap);	
	_VAR_GETT("textures/glowmap",textures.glowmap);	
	
	_VAR_GETT("shader",shader);	
	
	_VAR_GETT("next_pass",next_pass);	
	
	ERR_PRINT(String("Unexisting property "+p_path).ascii().get_data());
	ERR_FAIL_V(Variant());
	return Variant();
}

void Material_GL::get_property_list( List<PropertyInfo> *p_list ) const {

	p_list->push_back( PropertyInfo( Variant::INT, "layer" ) );
	
	p_list->push_back( PropertyInfo( Variant::COLOR, "color/ambient" ) );
	p_list->push_back( PropertyInfo( Variant::COLOR, "color/diffuse" ) );
	p_list->push_back( PropertyInfo( Variant::COLOR, "color/specular" ) );
	p_list->push_back( PropertyInfo( Variant::COLOR, "color/emission" ) );
	
	p_list->push_back( PropertyInfo( Variant::BOOL, "flags/visible" ) );
	p_list->push_back( PropertyInfo( Variant::BOOL, "flags/double_sided" ) );
	p_list->push_back( PropertyInfo( Variant::BOOL, "flags/invert_normals" ) );
	p_list->push_back( PropertyInfo( Variant::BOOL, "flags/shaded" ) );
	p_list->push_back( PropertyInfo( Variant::BOOL, "flags/draw_z" ) );
	p_list->push_back( PropertyInfo( Variant::INT, "flags/test_z" ) );
	p_list->push_back( PropertyInfo( Variant::BOOL, "flags/wireframe" ) );
	p_list->push_back( PropertyInfo( Variant::BOOL, "flags/scissor_alpha" ) );
	p_list->push_back( PropertyInfo( Variant::BOOL, "flags/emission_spherical" ) );
	p_list->push_back( PropertyInfo( Variant::BOOL, "flags/glow_spherical" ) );
	
	p_list->push_back( PropertyInfo( Variant::REAL, "vars/transparency" ) );
	p_list->push_back( PropertyInfo( Variant::REAL, "vars/shininess" ) );
	p_list->push_back( PropertyInfo( Variant::REAL, "vars/line_thickness" ) );
	p_list->push_back( PropertyInfo( Variant::REAL, "vars/normalmap_depth" ) );
	p_list->push_back( PropertyInfo( Variant::REAL, "vars/scissor_alpha_treshold" ) );
	p_list->push_back( PropertyInfo( Variant::REAL, "vars/glow" ) );
	
	p_list->push_back( PropertyInfo( Variant::IOBJ, "textures/diffuse" ) );
	p_list->push_back( PropertyInfo( Variant::IOBJ, "textures/specular" ) );
	p_list->push_back( PropertyInfo( Variant::IOBJ, "textures/emission" ) );
	p_list->push_back( PropertyInfo( Variant::IOBJ, "textures/reflection" ) );
	p_list->push_back( PropertyInfo( Variant::IOBJ, "textures/refmask" ) );
	p_list->push_back( PropertyInfo( Variant::IOBJ, "textures/normalmap" ) );
	p_list->push_back( PropertyInfo( Variant::IOBJ, "textures/glowmap" ) );
		
	p_list->push_back( PropertyInfo( Variant::IOBJ, "next_pass" ) );
	
}

IAPI::PropertyHint Material_GL::get_property_hint(String p_path) const {
	
	if (p_path == "layer")
		return PropertyHint(PropertyHint::HINT_RANGE,"-1000,1000,1");
	else if (p_path.find("vars/")==0)
		return PropertyHint(PropertyHint::HINT_RANGE,"0,1,0.0001");
	else if (p_path.find("textures/")==0)
		return PropertyHint(PropertyHint::HINT_IOBJ_TYPE,"Texture");
	else if (p_path=="next_pass")
		return PropertyHint(PropertyHint::HINT_IOBJ_TYPE,"Material");
	else if (p_path=="flags/test_z")
		return PropertyHint(PropertyHint::HINT_ENUM, "None,Never,Less,Equal,Less or equal,Greater,Not equal,Greater or equal,Always");
	else
		return PropertyHint();
}


Material_GL::Material_GL() {
	
	layer = 0;
	
	flags.visible=true;
	flags.double_sided=false;
	flags.invert_normals=false;
	flags.shaded=true;
	flags.draw_z=true;
	flags.test_z=Renderer::DEPTH_TEST_LESS;
	flags.wireframe=false;
	flags.scissor_alpha=false;
	flags.emission_spherical = false;
	flags.glow_spherical = false;
	
	vars.transparency=0.0;
	vars.shininess=0.2;
	vars.line_thickness=1.0;
	vars.normalmap_depth=0.7;
	vars.scissor_alpha_treshold=0;
	vars.glow = 0.0;
	
	color_ambient=Color(40,40,40,255);
	color_diffuse=Color(180,180,180,255);
	color_specular=Color(40,40,40,255);
	color_emission=Color(0,0,0,255);	
	
}

int Material_GL::get_layer() const {
	
	return layer;	
}

bool Material_GL::has_scissor_enabled() const {

	return flags.scissor_alpha;
}

bool Material_GL::has_alpha() const {

	if (!flags.shaded)
		return true;
	
	if (flags.scissor_alpha)
		return false;

	if (vars.transparency>0.0) {
		
		return true;
	}
	
	if (!textures.diffuse.is_null() && textures.diffuse->has_alpha())
		return true;
	return false;
}

Material_GL::~Material_GL() {
}


#endif // OPENGL_ENABLED
