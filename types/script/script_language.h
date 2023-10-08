//
// C++ Interface: script_language
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SCRIPT_LANGUAGE_H
#define SCRIPT_LANGUAGE_H


#include "script.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class ScriptLanguage;

class ScriptServer {
friend class ScriptLanguage;	
	enum {
		
		MAX_LANGUAGES=4
	};
	
	static ScriptLanguage *_languages[MAX_LANGUAGES];
public:	
	
	static IRef<Script> create(String p_type);
	static IRef<Script> load(String p_path);
	static IRef<Script> parse(String p_text,String p_type);
	
	static Error load_code(String p_path);
	static Error parse_code(String p_text,String p_type);
	
};

class ScriptLanguage {
public:
		
	virtual IRef<Script> create_script()=0; ///< Create a script object for instance
	
	virtual Error parse_code(String p_text)=0; ///< Parse code from text
	virtual Error load_code(String p_path)=0; ///M Parse code from a file
	virtual String get_type() const=0; ///< "lua"/"python"/"brainfuck"/etc
	virtual String get_extension() const=0; ///< ".lua"/".py"/".bf"
		
	ScriptLanguage();	
	virtual ~ScriptLanguage();

};



#endif
