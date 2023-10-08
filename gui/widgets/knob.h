//
// C++ Interface: knob
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef GUIKNOB_H
#define GUIKNOB_H

#include "widgets/range_owner.h"

namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class Knob : public RangeOwner {

        bool grabbing;
        Point old_pos;
        float grab_val;


	virtual void mouse_button(const  Point& p_pos, int p_button,bool p_press,int p_modifier_mask);	
	virtual void mouse_motion(const  Point& p_pos, const  Point& p_rel, int p_button_mask);
	
	virtual Size get_minimum_size_internal(); ///< Use this one when writing widgets

	virtual void draw(const  Point& p_global,const Size& p_size,const Rect& p_exposed);

	virtual void range_changed();
	virtual void value_changed(double p_new_val);

public:
	Knob();
	~Knob();

};

}

#endif
