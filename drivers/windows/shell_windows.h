//
// C++ Interface: shell_windows
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SHELL_WINDOWS_H
#define SHELL_WINDOWS_H

#include "os/shell.h"

#ifdef WINDOWS_ENABLED
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class ShellWindows : public Shell {
public:
	
	virtual void execute(String p_path);

	ShellWindows();
	
	~ShellWindows();

};

#endif
#endif
