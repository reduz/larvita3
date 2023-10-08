#ifdef OPENGL_ENABLED
//
// C++ Interface: light_gl
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef LIGHT_GL_H
#define LIGHT_GL_H

#include "render/light.h"

class Texture_GL;

/**
	@author ,,, <red@lunatea>
*/
class Light_GL : public Light {
friend class Rasterizer_GL14;	
friend class RasterizerForward_GL2;	
friend class RasterizerDeferred_GL2;	
friend class Renderer_GL;

	IAPI_TYPE(Light_GL,Light);
	IAPI_INSTANCE_FUNC(Light_GL);
	IAPI_PERSIST_AS(Light);
	
	Matrix4 matrix;
	Mode mode;
	Color ambient,diffuse,specular;
	float spot_angle;
	float spot_intensity;
	float radius;
	float intensity;
	float attenuation;
	bool cast_shadows;
	
	// used by Render_GL
	unsigned int _pass_id;
	bool _recycled;
	mutable bool _shadow_dirty;
	mutable Uint64 _last_hash;
	// @OPTIM possible optimization, store a transformed vector and cache it when projetion matrix doesn't change
	IRef<Texture_GL> shadow_texture;
public:


	virtual void set(String p_path,const Variant& p_value);
	virtual Variant get(String p_path) const;
	virtual void get_property_list( List<PropertyInfo> *p_list ) const;
	virtual PropertyHint get_property_hint(String p_path) const;

	virtual void set_matrix(const Matrix4& p_matrix);
	virtual Matrix4 get_matrix() const;

	virtual void set_mode(Mode p_mode);
	virtual Mode get_mode() const;
	
	virtual void set_ambient(const Color& p_ambient);
	virtual void set_diffuse(const Color& p_diffuse);
	virtual void set_specular(const Color& p_specular);

	virtual Color get_ambient() const ;
	virtual Color get_diffuse() const ;
	virtual Color get_specular() const ;

	virtual void set_spot_angle(float p_angle);
	virtual float get_spot_angle() const;

	virtual void set_radius(float p_radius);
	virtual float get_radius() const;

	virtual void set_attenuation(float p_attenuation);
	virtual float get_attenuation() const;

	virtual void set_intensity(float p_intensity);
	virtual float get_intensity() const;
	
	virtual void set_cast_shadows(bool p_cast_shadows);
	virtual bool get_cast_shadows() const;

	virtual void set_influence_hash(Uint64 p_hash) const;

	Light_GL();
	~Light_GL();

};

#endif
#endif // OPENGL_ENABLED
