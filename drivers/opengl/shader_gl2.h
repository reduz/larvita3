#ifdef OPENGL2_ENABLED

//
// C++ Interface: shader_gl
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SHADER_GL2_H
#define SHADER_GL2_H

#include "render/shader.h"
#include "gl_extensions.h"

/**
	@author ,,, <red@lunatea>
*/
class Shader_GL2 : public Shader {

	IAPI_TYPE(Shader_GL2,Shader);
	
friend class RasterizerForward_GL2;
friend class RasterizerDeferred_GL2;
friend class Renderer_GL2;
	
	struct ShaderType {
		
		String file;
		String code;
		GLuint shader_id;
	} vertex,fragment;
	
	StorageMode storage_mode;
	Type type;
	bool active;
	GLuint program_id;
	
	bool bind_shader_source(String p_source,GLuint p_type,GLuint& r_shader);
public:

	virtual void set(String p_path,const Variant& p_value);
	virtual Variant get(String p_path) const;
	virtual PropertyHint get_property_hint(String p_path) const;
	
	virtual void get_property_list( List<PropertyInfo> *p_list ) const;

	virtual StorageMode get_storage_mode() const;
	virtual void set_storage_mode(StorageMode p_storage);
	
	virtual Error parse_code(String p_code,Type p_type);
	virtual Error load_code(String p_file,Type p_type);
	
	virtual String get_code(Type p_type) const;
	
	virtual Error reload();
	
	Shader_GL2();	
	~Shader_GL2();

};

#endif
#endif // OPENGL2_ENABLED
