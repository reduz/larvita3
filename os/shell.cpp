//
// C++ Implementation: shell
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "shell.h"


Shell * Shell::singleton=NULL;


Shell * Shell::get_singleton() {

	return singleton;
}


Shell::Shell() {

	singleton=this;
}

Shell::~Shell() {

};


