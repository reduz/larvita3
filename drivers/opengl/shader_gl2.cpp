#ifdef OPENGL2_ENABLED
//
// C++ Implementation: shader_gl
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "shader_gl2.h"
#include "os/file_access.h"

void Shader_GL2::set(String p_path,const Variant& p_value) {


}
Variant Shader_GL2::get(String p_path) const {

	return Variant();
}
IAPI::PropertyHint Shader_GL2::get_property_hint(String p_path) const {

	return PropertyHint();
}

void Shader_GL2::get_property_list( List<PropertyInfo> *p_list ) const {

	
}



Shader::StorageMode Shader_GL2::get_storage_mode() const {

	return storage_mode;
}
void Shader_GL2::set_storage_mode(StorageMode p_storage) {

	storage_mode=p_storage;
}

Error Shader_GL2::parse_code(String p_code,Type p_type) {

	switch(p_type) {
		
		case TYPE_FRAGMENT: {
			
			fragment.code=p_code;
			fragment.file="";
		} break;
		case TYPE_VERTEX: {
			
			vertex.code=p_code;
			vertex.file="";
		} break;			
	}
	
	
	
	return reload();
}

Error Shader_GL2::load_code(String p_file,Type p_type) {

	FileAccess *f = FileAccess::create();
	
	Error err = f->open(p_file,FileAccess::READ);
	
	if (err) {
		
		memdelete(f);
		ERR_FAIL_COND_V( err, err );
	}
	
	String code;
	
	while (true) {
		
		Uint8 data=f->get_8();
		if (f->eof_reached())
			break;
		code+=(String::CharType)data;
	}
	
	if (p_type==TYPE_FRAGMENT)
		printf("loading fragment: %s\n",p_file.ascii().get_data());
	else
		printf("loading vertex: %s\n",p_file.ascii().get_data());
	
	f->close();
	memdelete(f);
	err = parse_code( code, p_type );
	
	ERR_FAIL_COND_V( err, err );
	
	switch(p_type) {
		
		case TYPE_FRAGMENT: {
			
			fragment.file=p_file;
		} break;
		case TYPE_VERTEX: {
			
			vertex.file=p_file;
		} break;			
	}
		

	return OK;
}

String Shader_GL2::get_code(Type p_type) const {
	
	switch( p_type ) {
		
		case TYPE_VERTEX: {
			
			return vertex.code;
		} break;
		case TYPE_FRAGMENT: {
			
			return fragment.code;
		} break;
	}
	
	return "";
}


bool Shader_GL2::bind_shader_source(String p_source,GLuint p_type,GLuint& r_shader) {
	
	r_shader=glCreateShader(p_type);
	
	if (r_shader==0) {
		
		return false;
	}
	
	CharString charstr=p_source.ascii();
	const char *src_cstr=charstr.get_data();
	glShaderSource(r_shader,1,&src_cstr,NULL);
	glCompileShader(r_shader);
	
	GLint status;
	
	glGetShaderiv(r_shader,GL_COMPILE_STATUS,&status);
	
	if (status==GL_FALSE) {
		// error compiling
		GLsizei iloglen;
		glGetShaderiv(r_shader,GL_INFO_LOG_LENGTH,&iloglen);
		
		if (iloglen<=0) {
			
			glDeleteShader(r_shader);
		}
		ERR_FAIL_COND_V(iloglen<=0,false);
		
		char *ilogmem = (char*)Memory::alloc_static(iloglen+1);
		ilogmem[iloglen]=0;
		glGetShaderInfoLog(r_shader, iloglen, &iloglen, ilogmem); 	
		
		String err_string=(p_type==GL_VERTEX_SHADER)?"VertexShader Compilation Failed:\n":"FragmentShader Compilation Failed:\n";
		
		err_string+=ilogmem;
		ERR_PRINT(err_string.ascii().get_data());
		Memory::free_static(ilogmem);
		glDeleteShader(r_shader);
		return false;
	}
		
	return true;
}

Error Shader_GL2::reload() {

	
	if (program_id!=0) {
		
		glDeleteShader(fragment.shader_id);
		glDeleteShader(vertex.shader_id);
		glDeleteProgram(program_id);
		active=false;
	}
	
	if (fragment.code=="" || vertex.code=="")
		return OK;
	/* Create Progam */
	program_id=glCreateProgram();
	ERR_FAIL_COND_V(program_id==0,ERR_COMPILATION_FAILED); // create program failed
	
	/* Create Vertex Shader */
	
	if (!bind_shader_source(vertex.code,GL_VERTEX_SHADER,vertex.shader_id)) {
		
		glDeleteProgram(program_id);
		program_id=0;
		ERR_FAIL_V(ERR_COMPILATION_FAILED);		
	}

	if (!bind_shader_source(fragment.code,GL_FRAGMENT_SHADER,fragment.shader_id)) {
		
		glDeleteShader(vertex.shader_id);
		glDeleteProgram(program_id);
		program_id=0;
		ERR_FAIL_V(ERR_COMPILATION_FAILED);		
	}

	glAttachShader(program_id,vertex.shader_id);
	glAttachShader(program_id,fragment.shader_id);
	
	/* Link */
	
	glLinkProgram(program_id);
	
	GLint status;
	
	glGetProgramiv(program_id, GL_LINK_STATUS, &status);
	
	if (status==GL_FALSE) {
		// error linking
		GLsizei iloglen;
		glGetProgramiv(program_id,GL_INFO_LOG_LENGTH,&iloglen);
		
		if (iloglen<=0) {
			
			glDeleteShader(vertex.shader_id);
			glDeleteShader(fragment.shader_id);
			glDeleteProgram(program_id);
			program_id=0;
			ERR_FAIL_COND_V(iloglen<=0,ERR_LINK_FAILED);		
		}
		
		char *ilogmem = (char*)Memory::alloc_static(iloglen+1);
		ilogmem[iloglen]=0;
		glGetProgramInfoLog(program_id, iloglen, &iloglen, ilogmem); 	
		
		String err_string="Shader Program LINK FAILED:\n";
		
		err_string+=ilogmem;
		ERR_PRINT(err_string.ascii().get_data());
		Memory::free_static(ilogmem);
		glDeleteShader(vertex.shader_id);
		glDeleteShader(fragment.shader_id);
		glDeleteProgram(program_id);
		program_id=0;
		
		ERR_FAIL_V(ERR_LINK_FAILED);
	}

	active=true;
	return OK;
}


Shader_GL2::Shader_GL2() {

	storage_mode=STORAGE_BUILT_IN;
	active=false;
	program_id=0;
	vertex.shader_id=0;
	fragment.shader_id=0;
}


Shader_GL2::~Shader_GL2()
{
}


#endif // OPENGL2_ENABLED
