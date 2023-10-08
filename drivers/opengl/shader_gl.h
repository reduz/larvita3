//
// C++ Interface: shader_gl
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SHADER_GL_H
#define SHADER_GL_H

#ifdef OPENGL_ENABLED

#include "gl_extensions.h"
#include "rstring.h"
#include "table.h"
#include "dvector.h"
#include "variant.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/


class ShaderGL {
	
	struct Version {
		
		GLuint id;
		GLuint vert_id;
		GLuint frag_id;					
		GLint *uniform_location;
	};
	
	Table<Uint64, Version> version_table;
	Uint64 conditional_version;
	virtual String get_shader_name() const=0;
	DVector<Variant> uniforms;
protected:	

	int uniform_count;
	int conditional_count;

	const char** conditional_strings;
	const char** uniform_strings;
	const char* vertex_code;
	const char* fragment_code;

	void compile_version(Uint64 p_vers);

	void setup(const char** p_conditional_defines, int p_conditional_count,const char** p_uniform_names,int p_uniform_count, const char*p_vertex_code, const char *p_fragment_code);
	ShaderGL();				

	void load_uniforms();

public:

	Uint64 get_version();
	bool test(); // returns true if shaders are supported

	void set_conditional(int p_which, bool p_value);
	void bind();
	
	int get_uniform(int p_which);
	void set_uniform(int p_which, Variant p_value);
	
	void unbind();

	virtual String get_name()=0;	
	virtual void init()=0;
	void finish();

	virtual ~ShaderGL();

};


#endif
#endif
