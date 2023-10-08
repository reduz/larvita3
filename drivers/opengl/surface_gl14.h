#ifdef OPENGL_ENABLED
//
// C++ Interface: surface_gl14
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SURFACE_GL14_H
#define SURFACE_GL14_H

#include "render/surface.h"
#include "drivers/opengl/material_gl.h"
#include "gl_extensions.h"

class Renderer_GL14;
	
class Surface_GL14 : public Surface {

//optimization
friend class Rasterizer_GL14;
friend class Renderer_GL14;
	
	IAPI_TYPE(Surface_GL14,Surface);
	IAPI_PERSIST_AS(Surface);
	

	struct ArrayData {
		
		Uint32 type;
		Uint32 ofs,size;
		bool configured;
		ArrayData() { type=0; ofs=0; size=0; configured=false; }	
	};
	
	IRef<Material_GL> material;
	
	// for using with regular memory arrays
	MID vertex_mem;
	MID index_mem;
	
	// for using with vbo arrays
	GLuint vertex_id; // 0 means, unconfigured
	GLuint index_id; // 0 means, unconfigured
	
	
	AABB aabb;
	
	bool use_VBO; // use vertex buffer objects if available
	
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
	
	enum ArrayTarget {
		TARGET_INDEX,
  		TARGET_VERTEX,
	};
	
	void array_lock(ArrayTarget p_target) const;
	
	void copy_to_array(ArrayTarget p_target,const void * p_src, int p_offset, int p_bytes);
	void copy_from_array(ArrayTarget p_target,void * p_dst,int p_offset, int p_bytes ) const;
	
	void array_unlock(ArrayTarget p_target) const;
	
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
	
	Surface_GL14();
	~Surface_GL14();

};

#endif
#endif // OPENGL_ENABLED
