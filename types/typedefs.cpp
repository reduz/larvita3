//
// C++ Implementation: typedefs
//
// Description: 
//
//
// Author: Juan Linietsky <red@lunatea>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//


#include "error_macros.h"
#include "typedefs.h"
#include "error_macros.h"
#include "rstring.h"

#ifndef GUI_DISABLED

namespace GUI {

void __print_error(const char *p_file,int p_line,const char *p_error) {

  String gui_error = String("GUI: ") + p_file + String(":") + String::num(p_line) + " - " + String(p_error);
  ERR_PRINT(gui_error.ascii().get_data());
}

}

#endif

void *operator new(size_t p_bytes,void *p_alloc_pos,size_t p_alloc_check) {

	void *nil=0;
	ERR_FAIL_COND_V(p_bytes > p_alloc_check,nil);
	
	return p_alloc_pos;

}
