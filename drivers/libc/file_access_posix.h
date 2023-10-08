//
// C++ Interface: file_access_libc
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FILE_ACCESS_POSIX_H
#define FILE_ACCESS_POSIX_H

#include "os/file_access.h"
#include "os/memory.h"
#include <stdio.h>

#ifdef POSIX_ENABLED

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class FileAccessPosix : public FileAccess {
	
	FILE *f;
	int flags;
	void check_errors();
	Error last_error;
	
	static FileAccess* create_libc();
public:
	
	virtual Error open(String p_filename, int p_mode_flags); ///< open a file 
	virtual void close(); ///< close a file
	virtual bool is_open(); ///< true when file is open 

	virtual void seek(Uint32 p_position); ///< seek to a given position 
	virtual void seek_end(Sint32 p_position=0); ///< seek from the end of file 
	virtual Uint32 get_pos(); ///< get position in the file 
	virtual Uint32 get_len(); ///< get size of the file 

	virtual bool eof_reached(); ///< reading passed EOF 

	virtual Uint8 get_8(); ///< get a byte 


	virtual Error get_error(); ///< get last error 

	virtual void store_8(Uint8 p_dest); ///< store a byte 
	
	virtual bool file_exists(String p_name); ///< return true if a file exists 

	static void set_as_default();
	

	FileAccessPosix();
	virtual ~FileAccessPosix();

};


#endif
#endif
