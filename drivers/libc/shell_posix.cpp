//
// C++ Implementation: shell_posix
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "shell_posix.h"

#ifdef POSIX_ENABLED

#include <stdlib.h>
void ShellPosix::execute(String p_path) {

#ifdef OSX_ENABLED

	String cmd = "open "+p_path;
	system(cmd.utf8().get_data());
#else
	if (getenv("GNOME_DESKTOP_SESSION_ID")) {
		// gnome no ide..
	
	} else if (getenv("KDEDIR")) {
		// kde..
		String cmd = "kfmclient exec "+p_path;
		system( cmd.utf8().get_data() );
	}		
#endif

}


ShellPosix::ShellPosix()
{
}


ShellPosix::~ShellPosix()
{
}


#endif