//
// C++ Interface: file_access_wrapper
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FILE_ACCESS_H
#define FILE_ACCESS_H

#include "typedefs.h"
#include "rstring.h"

/**
 * Multi-Platform abstraction for accessing to files.
 */

class FileAccess {
	
	bool endian_swap;
protected:

	String fix_path(String p_path) const;
	static FileAccess*(*create_func)(); /** default file access creation function for a platform */
public:

	enum ModeFlags  {

		READ=1,
		WRITE=2,
		READ_WRITE=3,
	};
	
	virtual Error open(String p_filename, int p_mode_flags)=0; ///< open a file 
	virtual void close()=0; ///< close a file 
	virtual bool is_open()=0; ///< true when file is open 

	virtual void seek(Uint32 p_position)=0; ///< seek to a given position 
	virtual void seek_end(Sint32 p_position=0)=0; ///< seek from the end of file 
	virtual Uint32 get_pos()=0; ///< get position in the file 
	virtual Uint32 get_len()=0; ///< get size of the file 

	virtual bool eof_reached()=0; ///< reading passed EOF 

	virtual Uint8 get_8()=0; ///< get a byte 
	virtual Uint16 get_16(); ///< get 16 bits uint  
	virtual Uint32 get_32(); ///< get 32 bits uint 
	virtual Uint64 get_64(); ///< get 64 bits uint 

	virtual void get_buffer(Uint8 *p_dst,int p_length); ///< get an array of bytes 
	
	/**< use this for files WRITTEN in _big_ endian machines (ie, amiga/mac)
	 * It's not about the current CPU type but file formats.
	 * this flags get reset to false (little endian) on each open
	 */
	 
	void set_endian_swap(bool p_swap) { endian_swap=p_swap; }
	inline bool get_endian_swap() const { return endian_swap; }

	virtual Error get_error()=0; ///< get last error 

	virtual void store_8(Uint8 p_dest)=0; ///< store a byte 
	virtual void store_16(Uint16 p_dest); ///< store 16 bits uint 
	virtual void store_32(Uint32 p_dest); ///< store 32 bits uint 
	virtual void store_64(Uint64 p_dest); ///< store 64 bits uint 

	virtual void store_buffer(const Uint8 *p_src,int p_length); ///< store an array of bytes 
	
	virtual bool file_exists(String p_name)=0; ///< return true if a file exists 

	static FileAccess *create(); /// Create a file access (for the current platform) this is the only portable way of accessing files.

	FileAccess() { endian_swap=false; }
	virtual ~FileAccess(){}

};


#endif
