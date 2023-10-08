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
#ifdef WINDOWS_ENABLED

#include "file_windows.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <wchar.h>
#include <tchar.h>

#ifdef MSVC
 #define S_ISREG(m) ((m)&_S_IFREG)
#endif

namespace GUI {

void FileWindows::check_errors() {

	if (!f)
		return;

	if (feof(f)) {

		last_error=ERR_EOF;;
	}

}

File::FileError FileWindows::open(String p_filename, int p_mode_flags) {

	p_filename=(p_filename);

	if (f)
		return ERR_IN_USE;

	const wchar_t* mode_string;

	if (p_mode_flags==READ)
		mode_string=L"rb";
	else if (p_mode_flags==WRITE)
		mode_string=L"wb";
	else if (p_mode_flags==READ_WRITE)
		mode_string=L"wb+";
	else
		return ERR_INVALID_PARAM;

	/* pretty much every implementation that uses fopen as primary
	   backend supports utf8 encoding */

	struct _stat st;
	if (_wstat(p_filename.c_str(), &st) == 0) {

		if (!S_ISREG(st.st_mode))
			return ERR_CANT_OPEN;

	};

	f=_wfopen(p_filename.c_str(), mode_string);


	if (f==NULL) {
		last_error=ERR_CANT_OPEN;
		return ERR_CANT_OPEN;
	} else {
		last_error=OK;
		flags=p_mode_flags;
		return OK;
	}

}
void FileWindows::close() {

	if (!f)
		return;

	fclose(f);
	f = NULL;
}
bool FileWindows::is_open() {

	return (f!=NULL);
}

void FileWindows::seek(unsigned int p_position) {

	if (!f) 
		return;
		
	last_error=OK;
	if ( fseek(f,p_position,SEEK_SET) )
		check_errors();
}
void FileWindows::seek_end(signed int p_position) {

	if (!f)
		return;
		
	if ( fseek(f,p_position,SEEK_END) )
		check_errors();
}
unsigned int FileWindows::get_pos() {


	unsigned int aux_position=0;
	if ( !(aux_position = ftell(f)) ) {
		check_errors();
	};
	return aux_position;
}
unsigned int FileWindows::get_len() {


	if (!f) 
		return 0;

	int pos = get_pos();
	seek_end();
	int size = get_pos();
	seek(pos);

	return size;
}

bool FileWindows::eof_reached() {

	return last_error==ERR_EOF;
}

unsigned char FileWindows::get_8() {

	if (!f) 
		return 0;
		
	unsigned char b;
	if (fread(&b,1,1,f) == 0) {
		check_errors();
	};

	return b;
}


File::FileError FileWindows::get_error() {

	return last_error;
}

void FileWindows::store_8(unsigned char p_dest) {

	if (!f) 
		return;
		
	fwrite(&p_dest,1,1,f);

}


bool FileWindows::file_exists(String p_name) {

	FILE *g;
	//printf("opening file %s\n", p_fname.c_str());
	g=_wfopen(p_name.c_str(),L"rb");
	if (g==NULL) {

		return false;
	} else {

		fclose(g);
		return true;
	}
}

File * FileWindows::create_win32() {

	return GUI_NEW( FileWindows );
}
void FileWindows::set_as_default()  {

	create_func=create_win32;
}


FileWindows::FileWindows() {

	f=NULL;
	flags=0;
	last_error=OK;

}
FileWindows::~FileWindows() {

	if (f)
		fclose(f);
}
}
#endif

