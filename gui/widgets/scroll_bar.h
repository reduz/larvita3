//
// C++ Interface: scroll_bar
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUISCROLL_BAR_H
#define PIGUISCROLL_BAR_H

#include "widgets/range_owner.h"


namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

class ScrollBar : public RangeOwner {


	String get_type();
	static bool focus_by_default;

	Orientation orientation;
	Size size;

	struct Drag {

		bool active;
		float pos_at_click;
		float value_at_click;
	} drag;


	int get_grabber_size();
	int get_margins_size();
	double get_click_pos(const Point& p_pos);
	int get_grabber_offset();


	// Overrides

	virtual Size get_minimum_size_internal();
	virtual void draw(const Point& p_global,const Size& p_size,const Rect& p_exposed);
	virtual void mouse_button(const Point& p_pos, int p_button,bool p_press,int p_modifier_mask);
	virtual void mouse_motion(const Point& p_pos, const Point& p_rel, int p_button_mask);
	virtual bool key(unsigned long p_unicode, unsigned long p_scan_code,bool p_press,bool p_repeat,int p_modifier_mask);
	virtual void resize(const Size& p_new_size);
public:

	static void set_can_focus_by_default(bool p_can_focus); ///< In some apps, people wont like scrollbars being focusable

	ScrollBar(Orientation p_orientation=HORIZONTAL);

	~ScrollBar();

};


class HScrollBar : public ScrollBar {

public:

	HScrollBar() : ScrollBar(HORIZONTAL) {}
};

class VScrollBar : public ScrollBar {

public:

	VScrollBar() : ScrollBar(VERTICAL) {}
};


}

#endif
