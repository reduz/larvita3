//
// C++ Interface: shell_posix
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SHELL_POSIX_H
#define SHELL_POSIX_H


#ifdef POSIX_ENABLED

#include "os/shell.h"
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class ShellPosix : public Shell {
public:
	
	virtual void execute(String p_path);

	ShellPosix();	
	~ShellPosix();

};

#endif
#endif
