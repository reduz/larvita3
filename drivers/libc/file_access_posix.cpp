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
#include "file_access_posix.h"

#ifdef POSIX_ENABLED

#include <sys/types.h>
#include <sys/stat.h>

#ifdef MSVC
 #define S_ISREG(m) ((m)&_S_IFREG)
#endif

void FileAccessPosix::check_errors() {

	ERR_FAIL_COND(!f);

	if (feof(f)) {

		last_error=ERR_FILE_EOF;;
	}

}

Error FileAccessPosix::open(String p_filename, int p_mode_flags) {

	p_filename=fix_path(p_filename);

	ERR_FAIL_COND_V(f,ERR_ALREADY_IN_USE);
	const char* mode_string;

	if (p_mode_flags==READ)
		mode_string="rb";
	else if (p_mode_flags==WRITE)
		mode_string="wb";
	else if (p_mode_flags==READ_WRITE)
		mode_string="wb+";
	else
		return ERR_INVALID_PARAMETER;

	/* pretty much every implementation that uses fopen as primary
	   backend supports utf8 encoding */

	struct stat st;
	if (stat(p_filename.utf8().get_data(),&st) == 0) {

		if (!S_ISREG(st.st_mode))
			return ERR_FILE_CANT_OPEN;

	};

	f=fopen(p_filename.utf8().get_data(),mode_string);


	if (f==NULL) {
		last_error=ERR_FILE_CANT_OPEN;
		return ERR_FILE_CANT_OPEN;
	} else {
		last_error=OK;
		flags=p_mode_flags;
		return OK;
	}

}
void FileAccessPosix::close() {

	if (!f)
		return;
	fclose(f);
	f = NULL;
}
bool FileAccessPosix::is_open() {

	return (f!=NULL);
}
void FileAccessPosix::seek(Uint32 p_position) {

	ERR_FAIL_COND(!f);

	last_error=OK;
	if ( fseek(f,p_position,SEEK_SET) )
		check_errors();
}
void FileAccessPosix::seek_end(Sint32 p_position) {

	ERR_FAIL_COND(!f);
	if ( fseek(f,p_position,SEEK_END) )
		check_errors();
}
Uint32 FileAccessPosix::get_pos() {


	Uint32 aux_position=0;
	if ( !(aux_position = ftell(f)) ) {
		check_errors();
	};
	return aux_position;
}
Uint32 FileAccessPosix::get_len() {


	ERR_FAIL_COND_V(!f,0);

	int pos = get_pos();
	seek_end();
	int size = get_pos();
	seek(pos);

	return size;
}

bool FileAccessPosix::eof_reached() {

	return last_error==ERR_FILE_EOF;
}

Uint8 FileAccessPosix::get_8() {

	ERR_FAIL_COND_V(!f,0);
	Uint8 b;
	if (fread(&b,1,1,f) == 0) {
		check_errors();
	};

	return b;
}


Error FileAccessPosix::get_error() {

	return last_error;
}

void FileAccessPosix::store_8(Uint8 p_dest) {

	ERR_FAIL_COND(!f);
	fwrite(&p_dest,1,1,f);

}


bool FileAccessPosix::file_exists(String p_name) {

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

FileAccess * FileAccessPosix::create_libc() {

	return memnew( FileAccessPosix );
}
void FileAccessPosix::set_as_default()  {

	create_func=create_libc;
}


FileAccessPosix::FileAccessPosix() {

	f=NULL;
	flags=0;
	last_error=OK;

}
FileAccessPosix::~FileAccessPosix() {

	if (f)
		fclose(f);
}

#endif
