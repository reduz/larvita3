#ifdef OPENGL_ENABLED
//
// C++ Interface: material_gl
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MATERIAL_GL_H
#define MATERIAL_GL_H

#include "render/material.h"
#include "texture_gl.h"
#include "shader_gl2.h"
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

class Renderer_GL;

class Material_GL : public Material {
//optimization
friend class Rasterizer_GL14;
friend class RasterizerForward_GL2;
friend class RasterizerDeferred_GL;
friend class Renderer_GL;

	IAPI_TYPE(Material_GL,Material);
	IAPI_INSTANCE_FUNC(Material_GL);
	IAPI_PERSIST_AS(Material);

	int layer;
	
	Color color_ambient;
	Color color_diffuse;
	Color color_specular;
	Color color_emission;
	
	struct Flags {
		bool visible;
		bool double_sided;
		bool invert_normals;
		bool shaded;
		bool draw_z;
		int test_z;
		bool wireframe;
		bool scissor_alpha;
		bool emission_spherical;
		bool glow_spherical;
	} flags;
	
	struct Variables {
		
		float transparency;
		float shininess;
		float line_thickness;
		float normalmap_depth;
		float scissor_alpha_treshold;
		float glow;
	} vars;
	
	struct Textures {
		
		IRef<Texture_GL> diffuse;
		IRef<Texture_GL> reflection;
		IRef<Texture_GL> emission;
		IRef<Texture_GL> refmask;
		IRef<Texture_GL> specular;
		IRef<Texture_GL> normalmap;		
		IRef<Texture_GL> glowmap;
	} textures;
	
	IRef<Material_GL> next_pass;
	IRef<Shader_GL2> shader;
	

	
public:
		
	virtual void set(String p_path,const Variant& p_value);
	virtual Variant get(String p_path) const;
	virtual void get_property_list( List<PropertyInfo> *p_list ) const;
	virtual PropertyHint get_property_hint(String p_path) const;
	
	virtual int get_layer() const;
	virtual bool has_alpha() const;
	virtual bool has_scissor_enabled() const;	
	Material_GL();
	~Material_GL();

};

#endif
#endif // OPENGL_ENABLED
