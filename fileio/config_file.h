//
// C++ Interface: config_file
//
// Description: 
//
//
// Author: Juan Linietsky <reduz@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#include "fileio/config_api.h"
/**
	@author Juan Linietsky <reduz@gmail.com>
*/


class ConfigFile : public ConfigApi {

	FileAccess *file;
	String path;
public:
	
	/* Save Methods */
	void set_section(String p_section);
	void add_entry(String p_name, String p_var,String p_comment="");
	
	
	Error save();
	Error load();
	
	void set_path(String p_path);
	
	ConfigFile(FileAccess *p_file);
	~ConfigFile();

};




#endif
