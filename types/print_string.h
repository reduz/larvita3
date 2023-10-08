//
// C++ Interface: print_string
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PRINT_STRING_H
#define PRINT_STRING_H

#include "rstring.h"

extern void (*_print_func)(String);

extern void print_line(String p_string);

#endif
