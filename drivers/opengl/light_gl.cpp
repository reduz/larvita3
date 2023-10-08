#ifdef OPENGL_ENABLED
//
// C++ Implementation: light_gl
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "light_gl.h"

#include "texture_gl.h"

/* Light has the following IAPI properties 

	"mode":int - Light::Mode
	
	"color/ambient":Color - Ambient Color
	"color/diffuse":Color - Diffuse Color
	"color/specular":Color - Specular Color
	
	"spot_angle":real - spot angle in degrees 0 - 90
	
	"radius":real - light max radius 
	"attenuation":real attenuation
	
	"cast_shadows":bool - light cast shadows
*/

void Light_GL::set(String p_path,const Variant& p_value) {

	if (p_path=="matrix") {
	
		matrix=p_value;
		
	} else if (p_path=="mode") {
	
		set_mode((Light::Mode)p_value.get_int());
	} else if (p_path=="color/ambient") {
	
		ambient=p_value.get_color();
	} else if (p_path=="color/diffuse") {
	
		diffuse=p_value.get_color();
	} else if (p_path=="color/specular") {
	
		specular=p_value.get_color();
	} else if (p_path=="spot_angle") {
	
		spot_angle=p_value.get_real();
	} else if (p_path=="radius") {
	
		radius=p_value.get_real();
	} else if (p_path=="attenuation") {
	
		attenuation=p_value.get_real();
	} else if (p_path=="intensity") {
		
		intensity = p_value.get_real(); // get real yo!
	} else if (p_path=="spot_intensity") {
		
		spot_intensity = p_value.get_real();
	} else if (p_path=="cast_shadows") {
	
		cast_shadows=p_value.get_real();
	} else if (p_path == "shadow_texture") {
		shadow_texture = p_value.get_IOBJ();
	}
	
}
Variant Light_GL::get(String p_path) const {

	if (p_path=="matrix") {
	
		return matrix;
	} else if (p_path=="mode") {
	
		return (int)mode;
	} else if (p_path=="color/ambient") {
	
		return ambient;
	} else if (p_path=="color/diffuse") {
	
		return diffuse;
	} else if (p_path=="color/specular") {
	
		return specular;
	} else if (p_path=="spot_angle") {
	
		return (double)spot_angle;
	} else if (p_path=="radius") {
	
		return radius;
	} else if (p_path=="attenuation") {
	
		return attenuation;
	} else if (p_path=="intensity") {
		return intensity;
	} else if (p_path == "spot_intensity") {
		return spot_intensity;
	} else if (p_path=="cast_shadows") {
	
		return cast_shadows;
	} else if (p_path == "shadow_texture") {
		
		return IObjRef(shadow_texture);
	}
	
	ERR_FAIL_V(Variant());
}
void Light_GL::get_property_list( List<PropertyInfo> *p_list ) const {

	p_list->push_back( PropertyInfo( Variant::MATRIX4, "matrix" ) );
	p_list->push_back( PropertyInfo( Variant::INT, "mode" ) );
	p_list->push_back( PropertyInfo( Variant::COLOR, "color/ambient" ) );
	p_list->push_back( PropertyInfo( Variant::COLOR, "color/diffuse" ) );
	p_list->push_back( PropertyInfo( Variant::COLOR, "color/specular" ) );
	p_list->push_back( PropertyInfo( Variant::REAL, "spot_angle" ) );
	p_list->push_back( PropertyInfo( Variant::REAL, "radius" ) );
	p_list->push_back( PropertyInfo( Variant::REAL, "attenuation" ) );
	p_list->push_back( PropertyInfo( Variant::REAL, "intensity" ) );
	p_list->push_back( PropertyInfo( Variant::REAL, "spot_intensity" ) );
	p_list->push_back( PropertyInfo( Variant::BOOL, "cast_shadows" ) );
	p_list->push_back( PropertyInfo( Variant::IOBJ, "shadow_texture" ) );
}
IAPI::PropertyHint Light_GL::get_property_hint(String p_path) const {

	if (p_path=="mode") {
	
		return PropertyHint( PropertyHint::HINT_ENUM,"Directional,Spot,Point");
	} else if (p_path=="spot_angle") {
	
		return PropertyHint( PropertyHint::HINT_RANGE,"0,90,0.01");
	} else if (p_path=="attenuation") {
	
		return PropertyHint( PropertyHint::HINT_RANGE,"0,16,0.001");
	} else if (p_path == "intensity") {
		return PropertyHint( PropertyHint::HINT_RANGE,"-8,8,0.001");
	} else if (p_path == "spot_intensity") {
		return PropertyHint( PropertyHint::HINT_RANGE,"-8,8,0.001");
	} else if (p_path == "radius") {
		return PropertyHint( PropertyHint::HINT_RANGE,"0,64,0.001");
	} else if (p_path == "shadow_texture") {
		return PropertyHint( PropertyHint::HINT_IOBJ_TYPE,"Texture");
	};
	
	return PropertyHint();
}

void Light_GL::set_matrix(const Matrix4& p_matrix) {

	matrix=p_matrix;

}

Matrix4 Light_GL::get_matrix() const {

	return matrix;
}


void Light_GL::set_mode(Mode p_mode) {
	ERR_FAIL_INDEX(p_mode, MODE_MAX);
	mode=p_mode;
	property_changed_signal.call("mode");
}
Light::Mode Light_GL::get_mode() const {

	return mode;
}

void Light_GL::set_ambient(const Color& p_ambient) {

	ambient=p_ambient;
}
void Light_GL::set_diffuse(const Color& p_diffuse) {

	diffuse=p_diffuse;
}
void Light_GL::set_specular(const Color& p_specular) {

	specular=p_specular;
}

Color Light_GL::get_ambient() const {

	return ambient;
}
Color Light_GL::get_diffuse() const  {

	return diffuse;
}
Color Light_GL::get_specular() const {

	return specular;
}

void Light_GL::set_spot_angle(float p_angle) {

	spot_angle=p_angle;
}
float Light_GL::get_spot_angle() const {

	return spot_angle;
}

void Light_GL::set_radius(float p_radius) {

	property_changed_signal.call("radius");
	radius=p_radius;
}
float Light_GL::get_radius() const {

	return radius;
}

void Light_GL::set_attenuation(float p_attenuation) {

	attenuation=p_attenuation;
}
float Light_GL::get_attenuation() const {

	return attenuation;
}

void Light_GL::set_intensity(float p_intensity) {

	intensity=p_intensity;
}
float Light_GL::get_intensity() const {

	return intensity;
}

void Light_GL::set_cast_shadows(bool p_cast_shadows) {

	cast_shadows=p_cast_shadows;
}
bool Light_GL::get_cast_shadows() const {

	return cast_shadows;
}

void Light_GL::set_influence_hash(Uint64 p_hash) const {
	
	if (p_hash != _last_hash) {
		_shadow_dirty = true;
		_last_hash = p_hash;
	};
};


Light_GL::Light_GL() {

	mode=MODE_DIRECTIONAL;
	ambient=Color(20,20,20);
	diffuse=Color(50,50, 50);
	spot_angle=45;
	radius=1;
	intensity = 1;
	spot_intensity = 1;
	attenuation=0.5;
	cast_shadows=false;
	_pass_id=0xFFFFFFFF;
	_recycled=false;

	_shadow_dirty = false;
	_last_hash = 0;
}


Light_GL::~Light_GL() {

}


#endif // OPENGL_ENABLED
