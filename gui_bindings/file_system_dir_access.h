//
// C++ Interface: file_system_dir_access
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FILE_SYSTEM_DIR_ACCESS_H
#define FILE_SYSTEM_DIR_ACCESS_H

#ifndef GUI_DISABLED

#include "base/file_system.h"
#include "os/dir_access.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

class FileSystem_DirAccess : public GUI::FileSystem {

	DirAccess *dir_access;
	static GUI::FileSystem *create_fs();
	
public:
	
	virtual bool list_dir_begin(); 
	virtual String get_next(bool* p_is_dir=0);
	virtual void list_dir_end(); 
	virtual int get_drive_count();
	virtual String get_drive(int p_drive);
	virtual bool change_dir(String p_dir); 
	virtual String get_current_dir(); 
	virtual bool make_dir(String p_dir); 
	virtual bool file_exists(String p_file);
	virtual bool rename(String p_path,String p_new_path); ///< false on succes
	virtual bool remove(String p_path); ///< false on success

	
	static void set_as_default();
	
	FileSystem_DirAccess();
	virtual ~FileSystem_DirAccess();

	
};

#endif
#endif
