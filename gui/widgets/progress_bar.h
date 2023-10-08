//
// C++ Interface: progress_bar
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIPROGRESS_BAR_H
#define PIGUIPROGRESS_BAR_H

#include "widgets/range_owner.h"

namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class ProgressBar : public RangeOwner {
	
	String suffix;
	virtual Size get_minimum_size_internal();
	virtual void draw(const Point& p_global,const Size& p_size,const Rect& p_exposed);
public:
	void set_suffix(String p_suffix);
	ProgressBar();
	~ProgressBar();

};

}

#endif
