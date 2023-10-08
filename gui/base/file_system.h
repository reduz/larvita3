//
// C++ Interface: file_system
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIFILE_SYSTEM_H
#define PIGUIFILE_SYSTEM_H

#include "base/defs.h"


namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

class FileSystem {

public:
	
	virtual bool list_dir_begin()=0; ///< This starts dir listing
	virtual String get_next(bool* p_is_dir=0)=0; ///< This retrieves dir entries, until an empty string is returned, optional p_is_dir can be passed
	virtual void list_dir_end()=0; ///< 
	
	virtual int get_drive_count()=0;
	virtual String get_drive(int p_drive)=0;
	
	virtual bool change_dir(String p_dir)=0; ///< can be relative or absolute, return false on success
	virtual String get_current_dir()=0; ///< return current dir location
	
	virtual bool make_dir(String p_dir)=0; ///< Make a directory

	virtual bool file_exists(String p_file)=0; ///< true wether a (local/global) file exists

	virtual String get_abs_dir_path(String p_dir);
	virtual String get_abs_file_path(String p_file);
	
	virtual bool rename(String p_path,String p_new_path)=0; ///< false on succes
	virtual bool remove(String p_path)=0; ///< false on success
	
	virtual ~FileSystem();

	
	static FileSystem*(*instance_func)(void); ///< Call this to instance a filesystem object
};

}

#endif
