//
// C++ Implementation: shader_gl
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifdef OPENGL_ENABLED

#include "shader_gl.h"

#include "os/memory.h"
#include "print_string.h"

void ShaderGL::set_conditional(int p_which, bool p_value) {
	
	ERR_FAIL_INDEX(p_which,conditional_count);
	if (p_value)
		conditional_version|=(1L<<p_which);
	else
		conditional_version&=~(1L<<p_which);
}

void ShaderGL::load_uniforms() {
	
	GLint* loc = version_table[ conditional_version ].uniform_location;
	if (!loc) return;
	for (int i=0; i<uniform_count; i++) {
		if (loc[i] == -1) {
			continue;
		};
		const Variant& var = uniforms[i];
		switch(var.get_type()) {
			
		case Variant::INT:
			glUniform1i( loc[i], (int)var);
			break;
		case Variant::REAL:
			glUniform1f( loc[i], (float)var);
			break;
		case Variant::VECTOR3: {
			Vector3 vec = var;
			glUniform3f(loc[i], vec.x, vec.y, vec.z);
			break;
		};
		case Variant::COLOR: {
			Color col = var;
			glUniform4f(loc[i], (float)col.r/255.0, (float)col.g/255.0, (float)col.b/255.0, (float)col.a/255.0);
			break;
		};
		case Variant::MATRIX4: {
			Matrix4 mat = var;
			if (sizeof(real_t) == 4) {
				glUniformMatrix4fv(loc[i], 1, false, (float*)mat.elements);
			} else {
				float elems[16];
				for (int j=0; i<16; j++) {
					elems[j] = ((double*)mat.elements)[j];
				};
				glUniformMatrix4fv(loc[i], 1, false, elems);
			};
			break;
		};

		default:
			; //nothing
		};
	};
};

void ShaderGL::bind() {
	
	if (!version_table.has(conditional_version)) {
		compile_version(conditional_version);
	};

	glUseProgram( version_table[ conditional_version ].id );
	load_uniforms();
}

void ShaderGL::set_uniform(int p_which, Variant p_value) {

	ERR_FAIL_INDEX( p_which, uniform_count );
	uniforms.set(p_which, p_value);
};

int ShaderGL::get_uniform(int p_which) {
	
	ERR_FAIL_INDEX_V( p_which, uniform_count,-1 );
	
	return version_table[ conditional_version ].uniform_location[p_which];
}


void ShaderGL::unbind() {
	
	glUseProgram(0);
}

Uint64 ShaderGL::get_version() {
	
	return conditional_version;
};

void ShaderGL::compile_version(Uint64 p_vers) {

	Version&v = version_table[p_vers];
	
	/* SETUP CONDITIONALS */
	
	const char** strings = memnew_arr(const char*,conditional_count+1);
	
	for(int j=0;j<conditional_count;j++) {
		strings[j]=((1L<<j)&p_vers)?conditional_strings[j]:"";
	}
				
	/* CREATE PROGRAM */
	
	v.id = glCreateProgram();
	
	ERR_FAIL_COND(v.id==0); 
	
	
	/* VERTEX SHADER */
	
	strings[conditional_count]=vertex_code;

	//*
	printf("%ls compiling vertex %li\n", get_name().c_str(), p_vers);
	for (int i=0; i<conditional_count; i++) {
		printf("%s", strings[i]);
	};
	printf("%i conditionals\n", conditional_count);
	//	*/
	
	v.vert_id = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(v.vert_id,conditional_count+1,strings,NULL);
	glCompileShader(v.vert_id);
	
	GLint status;

	glGetShaderiv(v.vert_id,GL_COMPILE_STATUS,&status);			

	if (status==GL_FALSE) {
	// error compiling
		GLsizei iloglen;
		glGetShaderiv(v.vert_id,GL_INFO_LOG_LENGTH,&iloglen);
		ERR_PRINT("Error compiling vertex program.");
		
		if (iloglen<=0) {
			
			glDeleteShader(v.vert_id);
			glDeleteProgram( v.id );
			v.id=0;
			ERR_PRINT("NO LOG, WTF");
		} else {
			
			
			char *ilogmem = (char*)Memory::alloc_static(iloglen+1);
			ilogmem[iloglen]=0;
			glGetShaderInfoLog(v.vert_id, iloglen, &iloglen, ilogmem); 	
			
			String err_string=get_name() + ": Fragment Program Compilation Failed:\n";
			
			err_string+=ilogmem;
			ERR_PRINT(err_string.ascii().get_data());
			Memory::free_static(ilogmem);
			glDeleteShader(v.vert_id);
			glDeleteProgram( v.id );
			v.id=0;
			
		}
		
		ERR_FAIL( );
	}		
	
	
	/* FRAGMENT SHADER */
	
	strings[conditional_count]=fragment_code;
	v.frag_id = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(v.frag_id,conditional_count+1,strings,NULL);
	glCompileShader(v.frag_id);

	/*
	printf("%ls compiling fragment %i\n", get_name().c_str(), p_vers);
	for (int i=0; i<conditional_count; i++) {
		printf("%s", strings[i]);
	};
	*/
	
	glGetShaderiv(v.frag_id,GL_COMPILE_STATUS,&status);			
	if (status==GL_FALSE) {
	// error compiling
		GLsizei iloglen;
		glGetShaderiv(v.frag_id,GL_INFO_LOG_LENGTH,&iloglen);
		ERR_PRINT("Error compiling fragment program.");
		
		if (iloglen<=0) {
			
			glDeleteShader(v.frag_id);
			glDeleteShader(v.vert_id);
			glDeleteProgram( v.id );
			v.id=0;
			ERR_PRINT("NO LOG, WTF");
		} else {
			
			
			char *ilogmem = (char*)Memory::alloc_static(iloglen+1);
			ilogmem[iloglen]=0;
			glGetShaderInfoLog(v.frag_id, iloglen, &iloglen, ilogmem); 	
			
			String err_string=get_name() + ": Fragment Program Compilation Failed:\n";
			
			err_string+=ilogmem;
			ERR_PRINT(err_string.ascii().get_data());
			Memory::free_static(ilogmem);
			glDeleteShader(v.frag_id);
			glDeleteShader(v.vert_id);
			glDeleteProgram( v.id );
			v.id=0;
			
		}
		
		ERR_FAIL( );
	}		
	
	glAttachShader(v.id,v.vert_id);
	glAttachShader(v.id,v.frag_id);
	
	glLinkProgram(v.id);
	
	glGetProgramiv(v.id, GL_LINK_STATUS, &status);
	
	if (status==GL_FALSE) {
	// error linking
		GLsizei iloglen;
		glGetProgramiv(v.id,GL_INFO_LOG_LENGTH,&iloglen);
		
		if (iloglen<=0) {
			
			glDeleteShader(v.frag_id);
			glDeleteShader(v.vert_id);
			glDeleteProgram( v.id );
			v.id=0;
			ERR_FAIL_COND(iloglen<=0);		
		}
		
		char *ilogmem = (char*)Memory::alloc_static(iloglen+1);
		ilogmem[iloglen]=0;
		glGetProgramInfoLog(v.id, iloglen, &iloglen, ilogmem); 	
		
		String err_string=get_name() + ": Program LINK FAILED:\n";
		
		err_string+=ilogmem;
		ERR_PRINT(err_string.ascii().get_data());
		Memory::free_static(ilogmem);
		glDeleteShader(v.frag_id);
		glDeleteShader(v.vert_id);
		glDeleteProgram( v.id );
		v.id=0;
		
		ERR_FAIL();
	}
	
	/* UNIFORMS */		
	
	glUseProgram(v.id);
	v.uniform_location = memnew_arr( GLint, uniform_count );
	
	for(int j=0;j<uniform_count;j++) {
		v.uniform_location[j]=glGetUniformLocation(v.id,uniform_strings[j]);
	}
	glUseProgram(0);
	
	memdelete_arr(strings);
};

void ShaderGL::setup(const char** p_conditional_defines, int p_conditional_count,const char** p_uniform_names,int p_uniform_count, const char*p_vertex_code, const char *p_fragment_code) {

	uniform_count = p_uniform_count;
	uniforms.resize(uniform_count);
	uniform_strings = p_uniform_names;

	conditional_count = p_conditional_count;
	conditional_strings = p_conditional_defines;

	vertex_code = p_vertex_code;
	fragment_code = p_fragment_code;
};

bool ShaderGL::test() {
	
	if (!glCreateProgram || !glUseProgram) {
		return false;
	};
	
	int id = glCreateProgram();
	if (id) {
		glDeleteProgram(id);
	};
	
	return id != 0;
};

void ShaderGL::finish() {

	const Uint64* key = NULL;
	while ( (key = version_table.next(key)) ) {
		glDeleteShader( version_table[*key].vert_id );
		glDeleteShader( version_table[*key].frag_id );
		glDeleteProgram( version_table[*key].id );
		memdelete_arr( version_table[*key].uniform_location );
	};
}


ShaderGL::ShaderGL() {

	conditional_version=0;
}


ShaderGL::~ShaderGL() {
	
	finish();
}


#endif
