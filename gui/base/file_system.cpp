//
// C++ Implementation: file_system
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "file_system.h"

namespace GUI {

FileSystem*(*FileSystem::instance_func)(void)=0;

String FileSystem::get_abs_dir_path(String p_dir) {
	
	String cwd = get_current_dir();
	
	change_dir(p_dir);
	String ret = get_current_dir();
	
	change_dir(cwd);
	
	return ret;
};

String FileSystem::get_abs_file_path(String p_file) {
	
	String fname;
	String path;
	
	int i = p_file.size();
	while (i--) {

		if (p_file[i] == '/') {
			fname = p_file.substr(i+1, p_file.size() - i);
			path = p_file.substr(0, i);
			break;
		};
	};
	if (i == -1) i = 0;

	return get_abs_dir_path(path) + "/" + fname;
};



FileSystem::~FileSystem()
{
}


}
