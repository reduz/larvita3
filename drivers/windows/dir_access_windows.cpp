//
// C++ Implementation: file_system_windows
//
// Description:
//
//
// Author: Juan Linietsky <reduz@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifdef WINDOWS_ENABLED

#include "dir_access_windows.h"

#include "os/memory.h"

#include <windows.h>
#include <wchar.h>
#include <stdio.h>

/*

[03:57] <reduz> yessopie, so i dont have to rely on unicows
[03:58] <yessopie> reduz- yeah, all of the functions fail, and then you can call GetLastError () which will return 120
[03:58] <drumstick> CategoryApl, hehe, what? :)
[03:59] <CategoryApl> didn't Verona lead to some trouble
[03:59] <yessopie> 120 = ERROR_CALL_NOT_IMPLEMENTED
[03:59] <yessopie> (you can use that constant if you include winerr.h)
[03:59] <CategoryApl> well answer with winning a compo

[04:02] <yessopie> if ( SetCurrentDirectoryW ( L"." ) == FALSE && GetLastError () == ERROR_CALL_NOT_IMPLEMENTED ) { use ANSI }
*/

struct DirAccessWindowsPrivate {

	HANDLE h; //handle for findfirstfile
	WIN32_FIND_DATA f;
	WIN32_FIND_DATAW fu; //unicode version
};


bool DirAccessWindows::list_dir_begin() {

	if (unicode) {
		list_dir_end();
		p->h = FindFirstFileW((current_dir+"\\*").c_str(), &p->fu);

		return (p->h==INVALID_HANDLE_VALUE);
	} else {

		list_dir_end();
		p->h = FindFirstFileA((current_dir+"\\*").ascii().get_data(), &p->f);

		return (p->h==INVALID_HANDLE_VALUE);

	}

	return false;
}


String DirAccessWindows::get_next(bool* p_is_dir) {

	if (p->h==INVALID_HANDLE_VALUE)
		return "";

	if (unicode) {
		if (p_is_dir) {

			if (p->fu.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				*p_is_dir=true;
			else
				*p_is_dir=false;
		}

		String name=p->fu.cFileName;

		if (FindNextFileW(p->h, &p->fu) == 0) {

			FindClose(p->h);
			p->h=INVALID_HANDLE_VALUE;
		}

		return name;
	} else {

		if (p_is_dir) {

			if (p->f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				*p_is_dir=true;
			else
				*p_is_dir=false;
		}

		String name=p->f.cFileName;

		if (FindNextFileA(p->h, &p->f) == 0) {

			FindClose(p->h);
			p->h=INVALID_HANDLE_VALUE;
		}

		return name;

	}
}
void DirAccessWindows::list_dir_end() {

	if (p->h!=INVALID_HANDLE_VALUE) {

		FindClose(p->h);
		p->h=INVALID_HANDLE_VALUE;
	}

}
int DirAccessWindows::get_drive_count() {

	return drive_count;

}
String DirAccessWindows::get_drive(int p_drive) {

	if (p_drive<0 || p_drive>=drive_count)
		return "";

	return String::chr(drives[p_drive])+":";
}

bool DirAccessWindows::change_dir(String p_dir) {

	p_dir=fix_path(p_dir);
	
	p_dir.replace("/","\\");

	if (unicode) {

		wchar_t real_current_dir_name[2048];
		GetCurrentDirectoryW(2048,real_current_dir_name);
		String prev_dir=real_current_dir_name;

		SetCurrentDirectoryW(current_dir.c_str());
		bool worked=(SetCurrentDirectoryW(p_dir.c_str())!=0);

		if (worked) {

			GetCurrentDirectoryW(2048,real_current_dir_name);
			current_dir=real_current_dir_name; // TODO, utf8 parser

		}

		SetCurrentDirectoryW(prev_dir.c_str());

		return !worked;
	} else {

		char real_current_dir_name[2048];
		GetCurrentDirectoryA(2048,real_current_dir_name);
		String prev_dir=real_current_dir_name;

		SetCurrentDirectoryA(current_dir.ascii().get_data());
		bool worked=(SetCurrentDirectory(p_dir.ascii().get_data())!=0);

		if (worked) {

			GetCurrentDirectoryA(2048,real_current_dir_name);
			current_dir=real_current_dir_name; // TODO, utf8 parser

		}

		SetCurrentDirectoryA(prev_dir.ascii().get_data());

		return !worked;

	}

	return false;

}

bool DirAccessWindows::make_dir(String p_dir) {

	p_dir=fix_path(p_dir);
	
	p_dir.replace("/","\\");

	if (unicode) {
		wchar_t real_current_dir_name[2048];
		GetCurrentDirectoryW(2048,real_current_dir_name);

		SetCurrentDirectoryW(current_dir.c_str());

		bool success=CreateDirectoryW(p_dir.c_str(), NULL);

		SetCurrentDirectoryW(real_current_dir_name);

		return !success;
	} else {

		char real_current_dir_name[2048];
		GetCurrentDirectoryA(2048,real_current_dir_name);

		SetCurrentDirectoryA(current_dir.ascii().get_data());

		bool success=CreateDirectoryA(p_dir.ascii().get_data(), NULL);

		SetCurrentDirectoryA(real_current_dir_name);

		return !success;

	}

	return false;
}


String DirAccessWindows::get_current_dir() {

	return current_dir;
}

bool DirAccessWindows::file_exists(String p_file) {

	p_file=fix_path(p_file);
	
	p_file.replace("/","\\");

	if (unicode) {

		DWORD       fileAttr;

		fileAttr = GetFileAttributesW(p_file.c_str());
		if (0xFFFFFFFF == fileAttr)
			return false;

		return fileAttr&FILE_ATTRIBUTE_ARCHIVE;

	} else {
		DWORD       fileAttr;

		fileAttr = GetFileAttributesA(p_file.ascii().get_data());
		if (0xFFFFFFFF == fileAttr)
			return false;
		return fileAttr&FILE_ATTRIBUTE_ARCHIVE;

	}

	return false;
}

DirAccess *DirAccessWindows::create_fs() {

	return memnew( DirAccessWindows );
}

Error DirAccessWindows::rename(String p_path,String p_new_path) {

	p_path=fix_path(p_path);
	p_new_path=fix_path(p_new_path);
	
	if (file_exists(p_new_path)) {
		if (remove(p_new_path) != OK) {
			return FAILED;
		};
	};

	return ::_wrename(p_path.c_str(),p_new_path.c_str())==0?OK:FAILED;
}

Error DirAccessWindows::remove(String p_path)  {

	p_path=fix_path(p_path);
	
	printf("erasing %s\n",p_path.utf8().get_data());
	DWORD fileAttr = GetFileAttributesW(p_path.c_str());
	if (fileAttr == INVALID_FILE_ATTRIBUTES)
		return FAILED;

	if (fileAttr & FILE_ATTRIBUTE_DIRECTORY)
		return ::_wrmdir(p_path.c_str())==0?OK:FAILED;
	else
		return ::_wunlink(p_path.c_str())==0?OK:FAILED;
}

int DirAccessWindows::get_space_left() {

	return -1;
};


void DirAccessWindows::set_as_default() {

	DirAccess::instance_func=&DirAccessWindows::create_fs;
}


DirAccessWindows::DirAccessWindows() {

	p = memnew( DirAccessWindowsPrivate );
	current_dir=".";

	drive_count=0;
	DWORD mask=GetLogicalDrives();

	for (int i=0;i<MAX_DRIVES;i++) {

		if (mask&(1<<i)) { //DRIVE EXISTS

			drives[drive_count]='a'+i;
			drive_count++;
		}
	}

	unicode=true;

	/* We are running Windows 95/98/ME, so no unicode allowed */
	if ( SetCurrentDirectoryW ( L"." ) == FALSE && GetLastError () == ERROR_CALL_NOT_IMPLEMENTED )
		unicode=false;

	p->h=INVALID_HANDLE_VALUE;
	change_dir(".");
}


DirAccessWindows::~DirAccessWindows() {

	memdelete( p );
}

#endif //windows DirAccess support
