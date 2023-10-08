//
// C++ Interface: file
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FILE_H
#define FILE_H

#include "string/rstring.h"
#include "base/defs.h"

namespace GUI {

/**
 * Multi-Platform abstraction for accessing to files.
 */

class File {
	
	bool endian_swap;
protected:

	
	static File*(*create_func)(); /** default file access creation function for a platform */
public:

	enum ModeFlags  {

		READ=1,
		WRITE=2,
		READ_WRITE=3,
	};
	
	enum FileError {
		OK,
		ERR_CANT_OPEN,
		ERR_IN_USE,
		ERR_INVALID_PARAM,		
		ERR_EOF	
	};
	
	virtual FileError open(String p_filename, int p_mode_flags)=0; ///< open a file 
	virtual void close()=0; ///< close a file 
	virtual bool is_open()=0; ///< true when file is open 

	virtual void seek(unsigned int p_position)=0; ///< seek to a given position 
	virtual void seek_end(signed int p_position=0)=0; ///< seek from the end of file 
	virtual unsigned int get_pos()=0; ///< get position in the file 
	virtual unsigned int get_len()=0; ///< get size of the file 

	virtual bool eof_reached()=0; ///< reading passed EOF 

	virtual unsigned char get_8()=0; ///< get a byte 
	virtual unsigned short get_16(); ///< get 16 bits uint  
	virtual unsigned int get_32(); ///< get 32 bits uint 
	virtual gui_u64 get_64(); ///< get 64 bits uint 

	virtual void get_buffer(unsigned char *p_dst,int p_length); ///< get an array of bytes 
	
	/**< use this for files WRITTEN in _big_ endian machines (ie, amiga/mac)
	 * It's not about the current CPU type but file formats.
	 * this flags get reset to false (little endian) on each open
	 */
	 
	void set_endian_swap(bool p_swap) { endian_swap=p_swap; }
	inline bool get_endian_swap() const { return endian_swap; }

	virtual FileError get_error()=0; ///< get last error 

	virtual void store_8(unsigned char p_dest)=0; ///< store a byte 
	virtual void store_16(unsigned short p_dest); ///< store 16 bits uint 
	virtual void store_32(unsigned int p_dest); ///< store 32 bits uint 
	virtual void store_64(gui_u64 p_dest); ///< store 64 bits uint 

	virtual void store_buffer(const unsigned char *p_src,int p_length); ///< store an array of bytes 
	
	virtual bool file_exists(String p_name)=0; ///< return true if a file exists 

	static File *create(); /// Create a file access (for the current platform) this is the only portable way of accessing files.

	File() { endian_swap=false; }
	virtual ~File(){}

};


}
#endif
