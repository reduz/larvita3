#ifdef OPENGL2_ENABLED
//
// C++ Interface: surface_gl
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SURFACE_GL2_H
#define SURFACE_GL2_H

/**
	@author ,,, <red@lunatea>
*/

#include "render/surface.h"
#include "drivers/opengl/material_gl.h"
#include "gl_extensions.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

class Renderer_GL2;
	
class Surface_GL2 : public Surface{

//optimization
friend class RasterizerForward_GL2;
friend class RasterizerDeferred_GL2;
friend class Renderer_GL2;
	
	IAPI_TYPE(Surface_GL2,Surface);
	IAPI_PERSIST_AS(Surface);
	

	struct ArrayData {
		
		Uint32 type;
		Uint32 ofs,size;
		bool configured;
		ArrayData() { type=0; ofs=0; size=0; configured=false; }	
	};
	
	IRef<Material_GL> material;
	
	GLuint vertex_id; // 0 means, unconfigured
	GLuint index_id; // 0 means, unconfigured
	
	AABB aabb;
	
	int array_len;
	int index_array_len;
	
	PrimitiveType primitive;
	
	int format;
	int flags;
	
	bool configured;
	ArrayData *array;
	int array_count;
	int stride;
	
	bool has_alpha_cache;
	
	int _find_array(ArrayType p_type) const;
public:

	static IRef<IAPI> IAPI_DEFAULT_INSTANCE_FUNC(const String& p_type,const  IAPI::CreationParams& p_params);
	virtual void set(String p_path,const Variant& p_value);
	virtual Variant get(String p_path) const;
	virtual void get_property_list( List<PropertyInfo> *p_list ) const;
	virtual PropertyHint get_property_hint(String p_path) const;

	virtual void set_material(IRef<Material> p_material);
	virtual IRef<Material> get_material() const;
		
	virtual void set_array(ArrayType p_type,const Variant& p_array);
	virtual Variant get_array(ArrayType p_type) const;
	
	virtual int get_array_len() const;
	virtual int get_index_array_len() const;
		
	virtual int get_format() const;
	virtual int get_flags() const;
	virtual PrimitiveType get_primitive_type() const;
	virtual void get_custom_array_format_flags(Uint8*) const;
		
	virtual void create(PrimitiveType p_primitive,int p_format,int p_flags,int p_array_len,int p_index_array_len=NO_INDEX_ARRAY,const Uint8* p_custom_array_fmt=NULL);
	
	virtual bool has_alpha() const;
		
	AABB get_AABB() const;
	
	Surface_GL2();
	~Surface_GL2();

};

#endif
#endif // OPENGL2_ENABLED
