//
// C++ Interface: value_label
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIVALUE_LABEL_H
#define PIGUIVALUE_LABEL_H


#include "base/range.h"
#include "base/widget.h"

namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class ValueLabel : public Widget {
public:

	enum Align {
		
		ALIGN_LEFT,
		ALIGN_CENTER,
		ALIGN_RIGHT
	};
private:
	
	Align align;
	RangeBase *range;

	String suffix;

	Size get_minimum_size_internal();
	void draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed);

	void range_changed();
	void range_value_changed(double p_to_what);
public:

	void set_suffix(String p_suffix);
	void set_align(Align p_align);
	Align get_align();
	
	void set_range(RangeBase *p_range);
	ValueLabel();
	~ValueLabel();

};

}

#endif
