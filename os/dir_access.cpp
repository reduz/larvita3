//
// C++ Implementation: dir_access
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "dir_access.h"
#include "os/file_access.h"
#include "os/memory.h"
#include "global_vars.h"

String DirAccess::fix_path(String p_path) const {
	
	return GlobalVars::get_singleton()->translate_path(p_path);
}


DirAccess* (*DirAccess::instance_func)() = NULL;


DirAccess *DirAccess::create() {
	
	return instance_func();
};

Error DirAccess::copy(String p_from,String p_to) {
	
	//printf("copy %s -> %s\n",p_from.ascii().get_data(),p_to.ascii().get_data());
	FileAccess *fsrc = FileAccess::create();
	ERR_FAIL_COND_V(!fsrc,ERR_UNCONFIGURED);
	Error err = fsrc->open( p_from, FileAccess::READ );
	
	if (err) {

		memdelete( fsrc );
		ERR_FAIL_COND_V( err, err );
	}
	
	FileAccess *fdst = FileAccess::create();
	ERR_FAIL_COND_V(!fdst,ERR_UNCONFIGURED);	
	err = fdst->open( p_to, FileAccess::WRITE );
	if (err) {

		fsrc->close();
		memdelete( fsrc );
		memdelete( fdst );
		ERR_FAIL_COND_V( err, err );
	}
	
	fsrc->seek_end(0);
	int size = fsrc->get_pos();
	fsrc->seek(0);
	err = OK;
	while(size--) {
		
		if (fsrc->get_error()!=OK) {
			err= fsrc->get_error();
			break;
		}
		if (fdst->get_error()!=OK) {
			err= fdst->get_error();
			break;
		}
		
		fdst->store_8( fsrc->get_8() );
	}
	
	fsrc->close();
	fdst->close();
	
	return err;
}

DirAccess::DirAccess(){

}


DirAccess::~DirAccess() {

}


