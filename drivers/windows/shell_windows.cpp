#ifdef WINDOWS_ENABLED
//
// C++ Implementation: shell_windows
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "shell_windows.h"

#include <windows.h>

void ShellWindows::execute(String p_path) {

	ShellExecuteW(NULL, L"open", p_path.c_str(), NULL, NULL, SW_SHOWNORMAL);

}


ShellWindows::ShellWindows()
{
}


ShellWindows::~ShellWindows()
{
}

#endif
