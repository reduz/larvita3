//
// C++ Interface: file_system_dirent
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIFILE_SYSTEM_DIRENT_H
#define PIGUIFILE_SYSTEM_DIRENT_H


#ifdef POSIX_ENABLED

#include "base/defs.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>

#include "base/file_system.h"

namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class FileSystemDirent : public FileSystem {
	
	DIR *dir_stream;
	
	static FileSystem *create_fs();
	
	String current_dir;
	
public:
	
	virtual bool list_dir_begin(); ///< This starts dir listing
	virtual String get_next(bool* p_is_dir=0); ///< This retrieves dir entries, until an empty string is returned, optional p_is_dir can be passed
	virtual void list_dir_end(); ///< 
	
	virtual int get_drive_count();
	virtual String get_drive(int p_drive);
	
	virtual bool change_dir(String p_dir); ///< can be relative or absolute, return false on success
	virtual String get_current_dir(); ///< return current dir location
	virtual bool make_dir(String p_dir);
	
	virtual bool file_exists(String p_file);	
	
	virtual bool rename(String p_path,String p_new_path); ///< false on succes
	virtual bool remove(String p_path); ///< false on success
	
	static void set_default_filesystem();
	
	FileSystemDirent();
	~FileSystemDirent();

};

}

#endif //POSIX ENABLED
#endif
