//
// C++ Interface: shader
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SHADER_H
#define SHADER_H

#include "iapi.h"

/**
	@author ,,, <red@lunatea>
*/
class Shader : public IAPI {

	IAPI_TYPE(Shader,IAPI);
	String text;
public:

	enum Type {
		TYPE_VERTEX,
		TYPE_FRAGMENT
	};

	enum StorageMode {
		
		STORAGE_FILE,
  		STORAGE_BUILT_IN
	};
	
	/* 
		creation params:
		file:String: load from file if to be loaded from a file
		type:String: "vertex"|"fragment"
		params:
		code:String (code, if storage mode is builtin)
		built_in_storage:bool (true, save code, false, save file reference
		shader/<params> may contain parameters exported by the shader
	*/
		
	virtual StorageMode get_storage_mode() const=0;
	virtual void set_storage_mode(StorageMode p_storage)=0;
	
	virtual Error parse_code(String p_code,Type p_type)=0;
	virtual Error load_code(String p_file,Type p_type)=0;
	
	virtual String get_code(Type p_type) const=0;
	
	virtual Error reload()=0;

	Shader();
	~Shader();

};

#endif
