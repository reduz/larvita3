//
// C++ Implementation: spin_box
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "spin_box.h"

#include "widgets/line_edit.h"
#include "widgets/updown.h"

namespace GUI {


void SpinBox::line_edit_enter_pressed(String p_string) {
	
	if (updating)
		return;
	
	get_range()->set(p_string.to_double());
}

void SpinBox::lost_focus() {
	
	if (updating)
		return;
	
	get_range()->set(line_edit->get_text().to_double());
}	

void SpinBox::_range_changed() {
	
	range_changed();
	update();
}
void SpinBox::_value_changed(double p_new_val) {

	if (updating)
		return;
	updating=true;
	
	line_edit->set_text( get_range()->get_as_text() );
	
	updating=false;
	
	value_changed(p_new_val);
	update();
	
}

RangeBase *SpinBox::get_range() {

	return range;
}
void SpinBox::set_range(RangeBase *p_range,bool p_own_it) {

	if (range) {

		range->range_changed_signal.disconnect( this, &SpinBox::_range_changed );
		range->value_changed_signal.disconnect( this, &SpinBox::_value_changed );

	}

	if (p_range) {

		updown->set_range( p_range, false );
		if ( range_owned ) {
			GUI_DELETE( range );
			range = 0;
		}

		range_owned=p_own_it;
	
		p_range->range_changed_signal.connect( this, &SpinBox::_range_changed );
		p_range->value_changed_signal.connect( this, &SpinBox::_value_changed );
	} else {
		
		range_owned = false;
	};

	range=p_range;
	
	if (get_parent()) {
		update();
		range_ptr_changed();
	}
}

void SpinBox::set_preffix(String p_preffix) {
	
	preffix=p_preffix;
}
void SpinBox::set_suffix(String p_suffix) {
	
	suffix = p_suffix;
}



SpinBox::SpinBox(){

	line_edit = add( GUI_NEW( LineEdit), 1);
	line_edit->add_constant_override( C_LINE_EDIT_MIN_WIDTH, C_SPINBOX_LINE_EDIT_MIN_WIDTH);
	updown= add( GUI_NEW( UpDown), 0);
	
		
	line_edit->text_enter_signal.connect( this, &SpinBox::line_edit_enter_pressed );
	
	line_edit->lost_focus_signal.connect( this, &SpinBox::lost_focus );
	range=0;
	range_owned=false;
	set_range( GUI_NEW( Range), true );	
	
	line_edit->set_text( get_range()->get_as_text() );
	
	updating=false;
}


SpinBox::~SpinBox() {

	if (range_owned) {
		GUI_DELETE( range );
	}

}




}
