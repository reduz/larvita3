//
// C++ Interface: dir_access
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DIR_ACCESS_H
#define DIR_ACCESS_H


#include "typedefs.h"
#include "rstring.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class DirAccess {
protected:
	static DirAccess*(*instance_func)(void); ///< set this to instance a filesystem object
	String fix_path(String p_path) const;
	
public:


	virtual bool list_dir_begin()=0; ///< This starts dir listing
	virtual String get_next(bool* p_is_dir=0)=0; ///< This retrieves dir entries, until an empty string is returned, optional p_is_dir can be passed
	virtual void list_dir_end()=0; ///< 
	
	virtual int get_drive_count()=0;
	virtual String get_drive(int p_drive)=0;
	
	virtual bool change_dir(String p_dir)=0; ///< can be relative or absolute, return false on success
	virtual String get_current_dir()=0; ///< return current dir location
	
	virtual bool make_dir(String p_dir)=0;

	virtual bool file_exists(String p_file)=0;

	virtual int get_space_left()=0;

	virtual Error copy(String p_from,String p_to);
	virtual Error rename(String p_from, String p_to)=0;
	virtual Error remove(String p_name)=0;
	
	static DirAccess *create();

	DirAccess();
	virtual ~DirAccess();

};

#endif
