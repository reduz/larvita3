//
// C++ Implementation: print_string
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "print_string.h"

void (*_print_func)(String)=NULL;


void print_line(String p_string) {

	if (_print_func)
		_print_func(p_string);
}