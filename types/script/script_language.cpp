
//
// C++ Implementation: script_language
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "script_language.h"


ScriptLanguage *ScriptServer::_languages[MAX_LANGUAGES]={NULL,NULL,NULL,NULL};


IRef<Script> ScriptServer::create(String p_type) {
	
	for (int i=0;i<MAX_LANGUAGES;i++) {
		
		if (_languages[i] && _languages[i]->get_type()==p_type) {
			
			
			return _languages[i]->create_script();			
		}
	}
	
	ERR_FAIL_V( IRef<Script>() );
}


IRef<Script> ScriptServer::load(String p_path) {
	
	for (int i=0;i<MAX_LANGUAGES;i++) {
		
		if (_languages[i] && p_path.findn(_languages[i]->get_extension())!=-1) {
			
			IRef<Script> script = _languages[i]->create_script();
			Error err = script->load(p_path);
			ERR_FAIL_COND_V(err,IRef<Script>());
			return script;
			
		}
	}
	
	ERR_FAIL_V( IRef<Script>() );
}
IRef<Script> ScriptServer::parse(String p_text,String p_type) {
	
	for (int i=0;i<MAX_LANGUAGES;i++) {
		
		if (_languages[i] && _languages[i]->get_type()==p_type) {
			
			IRef<Script> script = _languages[i]->create_script();
			Error err = script->load(p_text);
			ERR_FAIL_COND_V(err,IRef<Script>());
			return script;
			
		}
	}
	
	ERR_FAIL_V( IRef<Script>() );
}

Error ScriptServer::load_code(String p_path) {
	
	for (int i=0;i<MAX_LANGUAGES;i++) {
		
		if (_languages[i] && p_path.findn(_languages[i]->get_extension())!=-1) {
			
			return _languages[i]->parse_code(p_path);
		}
	}
	
	ERR_FAIL_V( ERR_UNAVAILABLE );
}
Error ScriptServer::parse_code(String p_text,String p_type) {
	
	for (int i=0;i<MAX_LANGUAGES;i++) {
		
		if (_languages[i] && _languages[i]->get_type()==p_type) {
			
			return _languages[i]->load_code(p_text);
		}
	}
	
	ERR_FAIL_V( ERR_UNAVAILABLE );
}

	
ScriptLanguage::ScriptLanguage() {
	
	for (int i=0;i<ScriptServer::MAX_LANGUAGES;i++) {
		
		if (ScriptServer::_languages[i]==NULL) {
			ScriptServer::_languages[i]=this;
			return;
		}
	}
	
	ERR_PRINT("All languages assigned\n");
	
}

ScriptLanguage::~ScriptLanguage() {
	
	for (int i=0;i<ScriptServer::MAX_LANGUAGES;i++) {
		
		if (ScriptServer::_languages[i]==this)
			ScriptServer::_languages[i]=NULL;			
	}
}

