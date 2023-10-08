//
// C++ Implementation: file_system_dir_access
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "file_system_dir_access.h"

#ifndef GUI_DISABLED

#include "os/memory.h"

GUI::FileSystem *FileSystem_DirAccess::create_fs() {
	
	return memnew( FileSystem_DirAccess );	
}

bool FileSystem_DirAccess::list_dir_begin() {
	
	return dir_access->list_dir_begin();
}
String FileSystem_DirAccess::get_next(bool* p_is_dir) {
	
	return dir_access->get_next(p_is_dir);
}
void FileSystem_DirAccess::list_dir_end() {
	
	dir_access->list_dir_end();
}
int FileSystem_DirAccess::get_drive_count() {
	
	return dir_access->get_drive_count();	
}
String FileSystem_DirAccess::get_drive(int p_drive) {
	
	return dir_access->get_drive(p_drive);		
}
bool FileSystem_DirAccess::change_dir(String p_dir) {
	
	return dir_access->change_dir(p_dir);
	
}
String FileSystem_DirAccess::get_current_dir() {
	
	return dir_access->get_current_dir();
	
}
bool FileSystem_DirAccess::make_dir(String p_dir) {
	
	return dir_access->make_dir(p_dir);	
}
bool FileSystem_DirAccess::file_exists(String p_file) {
	
	return dir_access->file_exists(p_file);	
	
}

bool FileSystem_DirAccess::rename(String p_path,String p_new_path) {
	
	return false;	
}
bool FileSystem_DirAccess::remove(String p_path) {
	
	return false;
}

void FileSystem_DirAccess::set_as_default() {
	
	instance_func=create_fs;
}

FileSystem_DirAccess::FileSystem_DirAccess() {
	
	dir_access=DirAccess::create();
	ERR_FAIL_COND(!dir_access);
}


FileSystem_DirAccess::~FileSystem_DirAccess() {
	
	memdelete( dir_access );
}

#endif
