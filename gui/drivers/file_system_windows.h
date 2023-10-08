//
// C++ Interface: file_system_windows
//
// Description:
//
//
// Author: Juan Linietsky <reduz@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FILE_SYSTEM_WINDOWS_H
#define FILE_SYSTEM_WINDOWS_H


#ifdef WINDOWS_ENABLED

#include "base/file_system.h"
#include "base/defs.h"
#include "string/rstring.h"

namespace GUI {

/**
	@author Juan Linietsky <reduz@gmail.com>
*/

struct FileSystemWindowsPrivate;


class FileSystemWindows : public FileSystem {

	enum {
		MAX_DRIVES=25
	};


	FileSystemWindowsPrivate *p;
	/* Windows stuff */

	char drives[MAX_DRIVES]; // a-z:
	int drive_count;

	String current_dir;

	static FileSystem *create_fs();
	bool unicode;

public:

	virtual bool list_dir_begin(); ///< This starts dir listing
	virtual String get_next(bool* p_is_dir=0); ///< This retrieves dir entries, until an empty string is returned, optional p_is_dir can be passed
	virtual void list_dir_end(); ///<

	virtual int get_drive_count();
	virtual String get_drive(int p_drive);

	virtual bool change_dir(String p_dir); ///< can be relative or absolute, return false on success
	virtual String get_current_dir(); ///< return current dir location

	virtual bool file_exists(String p_file);

	virtual bool make_dir(String p_dir);

	virtual bool rename(String p_path,String p_new_path); ///< false on succes
	virtual bool remove(String p_path); ///< false on success

	static void set_default_filesystem();

	FileSystemWindows();
	~FileSystemWindows();

};

}

#endif //WINDOWS_ENABLED

#endif
