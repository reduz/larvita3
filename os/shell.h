//
// C++ Interface: shell
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SHELL_H
#define SHELL_H

#include "typedefs.h"
#include "rstring.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class Shell {

	static Shell * singleton;
public:

	static Shell * get_singleton();
	virtual void execute(String p_path)=0;
	
	Shell();
	virtual ~Shell();	
};

#endif
