#ifdef OPENGL_ENABLED
//
// C++ Interface: texture_gl
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TEXTURE_GL_H
#define TEXTURE_GL_H

#include "render/texture.h"
#include "gl_extensions.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

class Renderer_GL;

class Texture_GL : public Texture {	
	
	IAPI_TYPE(Texture_GL,Texture);
	IAPI_PERSIST_AS(Texture);
	
	
	void get_gl_format(Image::Format p_format, Uint32 p_flags,GLenum& p_gl_format,int &r_gl_components,bool &r_has_alpha);
		
	int gl_components_cache;
	GLenum gl_format_cache;
	bool has_alpha_cache;
	bool active;
	Uint32 flags;
	String file;
	int width,height;
	GLuint tex_id;
	StorageMode storage_mode;
	Image::Format format;
	
	
public:
	
	inline bool is_active() const { return active; }
	inline GLuint get_gl_tex_id() const { return tex_id; }
	
	static IRef<IAPI> IAPI_DEFAULT_INSTANCE_FUNC(const String& p_type,const  IAPI::CreationParams& p_params);
	
	virtual void set(String p_path,const Variant& p_value);
	virtual Variant get(String p_path) const;
	virtual PropertyHint get_property_hint(String p_path) const;
	
	virtual void get_property_list( List<PropertyInfo> *p_list ) const;
	Variant call(String p_method, const List<Variant>& p_params=List<Variant>());
	virtual void get_method_list( List<MethodInfo> *p_list ) const;
		
	virtual Error load(String p_file,Uint32 p_flags=FLAGS_DEFAULT);
	virtual Error load(const Image& p_image,Uint32 p_flags=FLAGS_DEFAULT);
	
	virtual Error create(int p_width,int p_height,Image::Format p_format,Uint32 p_flags=FLAGS_DEFAULT);
	virtual Error paste_area(int p_x,int p_y,const Image& p_image);
	virtual Error copy_area(int p_x,int p_y,int p_width,int p_height,Image& r_image) const;
	
	virtual String get_file() const;
	virtual Uint32 get_flags() const;	
	
	virtual int get_width() const;
	virtual int get_height() const;
	virtual Image::Format get_format() const;
	
	virtual void set_storage_mode(StorageMode p_mode);
	virtual StorageMode get_storage_mode() const;
	virtual void set_storage_file(String p_file);
	
	virtual bool has_alpha() const;
	
	Texture_GL();
	~Texture_GL();

};

#endif
#endif // OPENGL_ENABLED
