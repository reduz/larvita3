//
// C++ Implementation: file_system_dirent
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//


#include "file_system_dirent.h"

#ifdef POSIX_ENABLED

namespace GUI {


FileSystem *FileSystemDirent::create_fs() {
	
	return GUI_NEW( FileSystemDirent );
}

bool FileSystemDirent::list_dir_begin() {
	
	list_dir_end(); //close any previous dir opening!
	
//	char real_current_dir_name[2048]; //is this enough?!	
	//getcwd(real_current_dir_name,2048);	
	//chdir(curent_path.utf8().get_data());
	dir_stream = opendir(current_dir.utf8().get_data());
	//chdir(real_current_dir_name);	
	if (!dir_stream)
		return true; //error!
	
	return false;
}

bool FileSystemDirent::file_exists(String p_file) {
	
	
	struct stat flags;	
	if ((stat(p_file.utf8().get_data(),&flags)!=0))
		return false;
	
	if (S_ISDIR(flags.st_mode))
		return false;
	
	return true;
	
}

String FileSystemDirent::get_next(bool* p_is_dir) {
	
	if (!dir_stream)
		return "";
	dirent *entry;
	
	entry=readdir(dir_stream);
	
	if (entry==NULL) {
		
		list_dir_end();
		return "";
	}
	
	//typedef struct stat Stat;
	struct stat flags;	
	
	String fname;
	if (fname.parse_utf8(entry->d_name))
		fname=entry->d_name; //no utf8, maybe latin?
	
	String f=current_dir+"/"+fname;
	
	if (p_is_dir) {
			
		
		if (stat(f.utf8().get_data(),&flags)==0) {
			
			if (S_ISDIR(flags.st_mode)) {
				
				*p_is_dir=true;
								
			} else {
				
				*p_is_dir=false;
			}	
		
		} else {
			
			*p_is_dir=false;
			
		}
		
	}
		
		
		
	
	return fname;
	
}
void FileSystemDirent::list_dir_end() {
	
	if (dir_stream)
		closedir(dir_stream);	
	dir_stream=0;
}

int FileSystemDirent::get_drive_count() {
	
	return 0;
}
String FileSystemDirent::get_drive(int p_drive) {
	
	return "";
}

bool FileSystemDirent::make_dir(String p_dir) {
	
	char real_current_dir_name[2048];
	getcwd(real_current_dir_name,2048);
	chdir(current_dir.utf8().get_data()); //ascii since this may be unicode or wathever the host os wants
	
	bool success=(mkdir(p_dir.utf8().get_data(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)==0);
	
	chdir(real_current_dir_name);
	
	return !success;
}


bool FileSystemDirent::change_dir(String p_dir) {
	
	char real_current_dir_name[2048];
	getcwd(real_current_dir_name,2048);
	String prev_dir;
	if (prev_dir.parse_utf8(real_current_dir_name))
		prev_dir=real_current_dir_name; //no utf8, maybe latin?

	chdir(current_dir.utf8().get_data()); //ascii since this may be unicode or wathever the host os wants
	bool worked=(chdir(p_dir.utf8().get_data())==0); // we can only give this utf8
	

	if (worked) {
	
		getcwd(real_current_dir_name,2048);
		if (current_dir.parse_utf8(real_current_dir_name))
			current_dir=real_current_dir_name; //no utf8, maybe latin?
	}
	
	chdir(prev_dir.utf8().get_data());
	
	return !worked;
	
}

String FileSystemDirent::get_current_dir() {
	
	return current_dir;
}

void FileSystemDirent::set_default_filesystem() {
	
	FileSystem::instance_func=&FileSystemDirent::create_fs;
}

bool FileSystemDirent::rename(String p_path,String p_new_path) {

	return ::rename(p_path.utf8().get_data(),p_new_path.utf8().get_data());
}
bool FileSystemDirent::remove(String p_path)  {

	printf("erasing %s\n",p_path.utf8().get_data());
	struct stat flags;	
	if ((stat(p_path.utf8().get_data(),&flags)!=0))
		return true;
	
	if (S_ISDIR(flags.st_mode))
		return ::rmdir(p_path.utf8().get_data());
	else 
		return ::unlink(p_path.utf8().get_data());
}

FileSystemDirent::FileSystemDirent() {
	
	dir_stream=0;
	current_dir=".";	
	
	/* determine drive count */

	change_dir(current_dir);
	
}


FileSystemDirent::~FileSystemDirent() {
	
	list_dir_end();
}


}

#endif //posix_enabled
