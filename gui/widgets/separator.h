//
// C++ Interface: separator
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUISEPARATOR_H
#define PIGUISEPARATOR_H

#include "base/widget.h"

namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class Separator : public Widget {

	Orientation dir;
	void draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed);
	Size get_minimum_size_internal();
	
public:
	
	Separator(Orientation p_dir);
	~Separator();

};


class VSeparator : public Separator {


public:

	VSeparator() : Separator(VERTICAL) {}
};

class HSeparator : public Separator {


public:
	HSeparator() : Separator(HORIZONTAL) {}
};

}

#endif
