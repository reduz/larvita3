#ifndef FILE_ACCESS_BUFFERED_H
#define FILE_ACCESS_BUFFERED_H

#include "os/file_access.h"

#include "types/dvector.h"
#include "rstring.h"

class HTTPClient;

class FileAccessBuffered : public FileAccess {

public:
	enum {
		DEFAULT_CACHE_SIZE = 512,
	};

private:

	int cache_size;

	bool perform_endian_swap;

	template<class T>
	void endian_swap(T &a, T &b) {

		if  (perform_endian_swap) {
			T c;			
			c=a; a=b; b=c;
		}
	}

	bool cache_data_left(int p_len);
	Error last_error;

protected:

	Error set_error(Error p_error);

	struct File {

		bool open;
		int size;
		int offset;
		String name;
	} file;
	
	struct Cache {
		
		DVector<Uint8> buffer;
		int offset;
	} cache;

	virtual int read_data_block(int p_offset, int p_size)=0;

	void set_cache_size(int p_size);
	int get_cache_size();
	
public:

	virtual void seek(Uint32 p_position);
	virtual void seek_end(Sint32 p_position=0);
	virtual Uint32 get_pos();
	virtual Uint32 get_len();

	virtual bool eof_reached();

	virtual Uint8 get_8();
	virtual void get_buffer(Uint8 *p_dst,int p_length); ///< get an array of bytes 
	

	// use this for files WRITTEN in _big_ endian machines (ie, amiga/mac)
	// It's not about the current CPU type but file formats.
	// this flags get reset to false (little endian) on each open
	virtual void set_endian_conversion(bool p_swap);
	virtual bool is_open();

	virtual Error get_error();

	virtual void store_8(Uint8 p_dest);

	FileAccessBuffered();
	virtual ~FileAccessBuffered();
};

#endif

