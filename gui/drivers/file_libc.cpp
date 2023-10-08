
//
// C++ Implementation: file_access_libc
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "file_libc.h"

#ifdef POSIX_ENABLED

#include "base/memory.h"

#include <sys/types.h>
#include <sys/stat.h>

#ifdef MSVC
 #define S_ISREG(m) ((m)&_S_IFREG)
#endif

namespace GUI {

void FileLibC::check_errors() {

	if (!f)
		return;

	if (feof(f)) {

		last_error=ERR_EOF;;
	}

}

File::FileError FileLibC::open(String p_filename, int p_mode_flags) {

	if (f)
		return ERR_IN_USE;
				
	const char* mode_string;

	if (p_mode_flags==READ)
		mode_string="rb";
	else if (p_mode_flags==WRITE)
		mode_string="wb";
	else if (p_mode_flags==READ_WRITE)
		mode_string="wb+";
	else
		return ERR_INVALID_PARAM;

	/* pretty much every implementation that uses fopen as primary
	   backend supports utf8 encoding... EXCEPT WINDOWS! :( */

	struct stat st;
	if (stat(p_filename.utf8().get_data(),&st) == 0) {

		if (!S_ISREG(st.st_mode))
			return ERR_CANT_OPEN;

	}

	f=fopen(p_filename.utf8().get_data(),mode_string);


	if (f==NULL) {
		last_error=ERR_CANT_OPEN;
		return ERR_CANT_OPEN;
	} else {
		last_error=OK;
		flags=p_mode_flags;
		return OK;
	}

}
void FileLibC::close() {

	if (!f)
		return;
	fclose(f);
	f = NULL;
}
bool FileLibC::is_open() {

	return (f!=NULL);
}
void FileLibC::seek(unsigned int p_position) {

	if (!f)
		return;

	last_error=OK;
	if ( fseek(f,p_position,SEEK_SET) )
		check_errors();
}
void FileLibC::seek_end(signed int p_position) {

	if (!f) return;
	if ( fseek(f,p_position,SEEK_END) )
		check_errors();
}
unsigned int FileLibC::get_pos() {

	if (!f)
		return 0;

	unsigned int aux_position=0;
	if ( !(aux_position = ftell(f)) ) {
		check_errors();
	};
	return aux_position;
}
unsigned int FileLibC::get_len() {

	if (!f)
		return 0;

	int pos = get_pos();
	seek_end();
	int size = get_pos();
	seek(pos);

	return size;
}

bool FileLibC::eof_reached() {

	return last_error==ERR_EOF;
}

unsigned char FileLibC::get_8() {

	if (!f)
		return 0;
	unsigned char b;
	if (fread(&b,1,1,f) == 0) {
		check_errors();
	};

	return b;
}


File::FileError FileLibC::get_error() {

	return last_error;
}

void FileLibC::store_8(unsigned char p_dest) {

	if (!f) return;
	fwrite(&p_dest,1,1,f);

}


bool FileLibC::file_exists(String p_name) {

	FILE *g;
	//printf("opening file %s\n", p_fname.c_str());
	g=fopen(p_name.utf8().get_data(),"rb");
	if (g==NULL) {

		return false;
	} else {

		fclose(g);
		return true;
	}
}

GUI::File * FileLibC::create_libc() {

	return GUI_NEW( FileLibC );
}
void FileLibC::set_as_default()  {

	create_func=create_libc;
}


FileLibC::FileLibC() {

	f=NULL;
	flags=0;
	last_error=OK;

}
FileLibC::~FileLibC() {

	if (f)
		fclose(f);
}

}
#endif
