//
// C++ Implementation: error_macros
//
// Description:
//
//
// Author: Juan Linietsky <red@lunatea>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "error_macros.h"



void (*print_error_func)(const char* p_file,int p_line,const char *p_error)=0;


void print_error(const char* p_file,int p_line,const char *p_error) {

	if (print_error_func)
		print_error_func(p_file,p_line,p_error);
}
