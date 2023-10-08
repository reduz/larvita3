//
// C++ Interface: updown
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIUPDOWN_H
#define PIGUIUPDOWN_H

#include "widgets/range_owner.h"

namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class UpDown : public RangeOwner {
	
	Size size;
	
	virtual void resize(const Size& p_new_size);
	virtual Size get_minimum_size_internal();
	virtual String get_type();	
	void draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed);
	void mouse_button(const Point& p_pos, int p_button,bool p_press,int p_modifier_mask);
	
public:
	UpDown();
	~UpDown();
};

}

#endif
