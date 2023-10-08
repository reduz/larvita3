//
// C++ Interface: light
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef LIGHT_H
#define LIGHT_H

#include "iapi.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class Light : public IAPI {

	IAPI_TYPE(Light,IAPI);
public:		

	enum Mode {
		MODE_DIRECTIONAL,
		MODE_SPOT,	
		MODE_POINT,
		MODE_MAX,
	};

	/* Light has the following IAPI properties 
	
		"matrix":matrix4 light matrix
		"mode":int - Light::Mode
		
		"color/ambient":Color - Ambient Color
		"color/diffuse":Color - Diffuse Color
		"color/specular":Color - Specular Color
		
		"spot_angle":real - spot angle in degrees 0 - 90
		
		"range":real - light max range 
		"attenuation":real attenuation
		
		"casts_shadows":bool - light casts shadows
	*/

	virtual void set_matrix(const Matrix4& p_matrix)=0;
	virtual Matrix4 get_matrix() const=0;

	virtual void set_mode(Mode p_mode)=0;
	virtual Mode get_mode() const=0;
	
	virtual void set_ambient(const Color& p_ambient)=0;
	virtual void set_diffuse(const Color& p_diffuse)=0;
	virtual void set_specular(const Color& p_specular)=0;

	virtual Color get_ambient() const=0;
	virtual Color get_diffuse() const=0;
	virtual Color get_specular() const=0;

	virtual void set_spot_angle(float p_angle)=0;
	virtual float get_spot_angle() const=0;
	
	virtual void set_radius(float p_radius)=0;
	virtual float get_radius() const=0;

	virtual void set_attenuation(float p_attenuation)=0;
	virtual float get_attenuation() const=0;

	virtual void set_intensity(float p_intensity)=0;
	virtual float get_intensity() const=0;

	virtual void set_cast_shadows(bool p_cast_shadows)=0;
	virtual bool get_cast_shadows() const=0;

	virtual void set_influence_hash(Uint64 p_hash) const=0;
	
	virtual const AABB get_AABB() const;

	Light();
	~Light();

};

#endif
