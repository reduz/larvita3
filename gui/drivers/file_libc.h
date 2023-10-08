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

#include "base/file.h"

#ifdef POSIX_ENABLED

#include <stdio.h>
namespace GUI {
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class FileLibC : public File {
	
	FILE *f;
	int flags;
	void check_errors();
	FileError last_error;
	
	static File* create_libc();
public:
	
	virtual FileError open(String p_filename, int p_mode_flags); ///< open a file 
	virtual void close(); ///< close a file
	virtual bool is_open(); ///< true when file is open 

	virtual void seek(unsigned int p_position); ///< seek to a given position 
	virtual void seek_end(signed int p_position=0); ///< seek from the end of file 
	virtual unsigned int get_pos(); ///< get position in the file 
	virtual unsigned int get_len(); ///< get size of the file 

	virtual bool eof_reached(); ///< reading passed EOF 

	virtual unsigned char get_8(); ///< get a byte 


	virtual FileError get_error(); ///< get last error 

	virtual void store_8(unsigned char p_dest); ///< store a byte 
	
	virtual bool file_exists(String p_name); ///< return true if a file exists 

	static void set_as_default();
	

	FileLibC();
	virtual ~FileLibC();

};

}

#endif
#endif
