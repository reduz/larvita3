//
// C++ Interface: arrow_button
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIARROWBUTTON_H
#define PIGUIARROWBUTTON_H

#include "widgets/base_button.h"
#include "base/range.h"


namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class ArrowButton : public BaseButton {

	Direction dir;
	bool no_minsize;
	RangeBase *range;
	bool auto_hide;
	bool page_increment;

	void _check_auto_hide();
	void _range_changed();
	Size get_minimum_size_internal();
	void draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed);

	BitmapID get_bitmap();
	
	virtual String get_type();
	void press_slot();
public:

	void set_page_increment(bool p_increment);
	void set_dir(Direction p_dir);
	Direction get_dir () const;
	void set_range( RangeBase *p_range );
	void set_auto_hide( bool p_enabled );

	ArrowButton(Direction p_dir,bool p_no_minsize=false);
	~ArrowButton();

};

}

#endif
