//
// C++ Interface: spin_box
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUISPIN_BOX_H
#define PIGUISPIN_BOX_H

#include "containers/box_container.h"
#include "base/range.h"

namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

class UpDown;
class LineEdit;

class SpinBox : public HBoxContainer {

	
	RangeBase *range;
	bool range_owned;
	bool updating;
	/* signals can't connect to virtual functions */
	
	void _check_auto_hide();
	void _range_changed();
	void _value_changed(double p_new_val);
	
	LineEdit *line_edit;
	UpDown *updown;
	
	String preffix;
	String suffix;

	void lost_focus();
	void line_edit_enter_pressed(String p_string);
	
protected:

	virtual void range_changed() {}
	virtual void value_changed(double p_new_val) {}
	virtual void range_ptr_changed() {}
	
public:

	LineEdit *get_line_edit() { return line_edit; }
	void set_preffix(String p_preffix);
	void set_suffix(String p_suffix);
	
	RangeBase *get_range();
	void set_range(RangeBase *p_range,bool p_own_it=false);
	
	SpinBox();
	~SpinBox();
};

}

#endif
