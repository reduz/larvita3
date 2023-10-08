//
// C++ Interface: icon
//
// Description: 
//
//
// Author: Juan Linietsky <reduz@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIICON_H
#define PIGUIICON_H


#include "base/widget.h"
namespace GUI {

/**
	@author Juan Linietsky <reduz@gmail.com>
*/
class Icon : public Widget {
	
	Size get_minimum_size_internal();
	
	void draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed);
	BitmapID bitmapid;
	
protected:

	virtual String get_type();	
	
public:
	
	BitmapID get_bitmap();
	void set_bitmap(BitmapID p_bitmap);
	Icon(BitmapID p_bitmap=-1);

	~Icon();

};

}

#endif
