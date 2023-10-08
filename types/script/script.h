//
// C++ Interface: script
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SCRIPT_H
#define SCRIPT_H

#include "iapi.h"
 

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

class ScriptInstance : public IAPI {
	
	IAPI_TYPE(ScriptInstance,IAPI);
public:	

	virtual Error construct()=0;
	
};

class Script : public IAPI {
	
	IAPI_TYPE(Script,IAPI);
	
public:
	
	enum StorageMode {
		
		STORAGE_FILE,
		STORAGE_BUILT_IN
	};
	
	
	virtual Error parse(const String& p_text)=0;
	virtual Error load(const String& p_path)=0;
	
	virtual IRef<ScriptInstance> instance(IRef<IAPI> p_self=IRef<IAPI>())=0;

	
	virtual String get_file() const=0;
		
	virtual void set_storage_mode(StorageMode p_mode)=0;
	virtual StorageMode get_storage_mode() const=0;
	virtual void set_storage_file(String p_file)=0;	
	
	Script();
	virtual ~Script();

};

#endif
