//
// C++ Implementation: range_owner
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "range_owner.h"
#include <stdio.h>
namespace GUI {

void RangeOwner::_check_auto_hide() {

	if (!range)
		return;

	if (auto_hide) {
		if ( ((range->get_max()-range->get_min())-range->get_page()) <=0 )
			hide();
		else {
			show();
		}
	}

}
void RangeOwner::_range_changed() {

	_check_auto_hide();

	range_changed();
	update();

}
void RangeOwner::_value_changed(double p_new_val) {

	value_changed(p_new_val);
	update();



}

RangeBase *RangeOwner::get_range() {

	return range;
}
void RangeOwner::set_range(RangeBase *p_range,bool p_own_it) {


	if (range) {

		range->range_changed_signal.disconnect( this, &RangeOwner::_range_changed );
		range->value_changed_signal.disconnect( this, &RangeOwner::_value_changed );

	}

	if (range && range_owned) {
		GUI_DELETE( range );
		range=NULL;
	}

	range=p_range;

	range_owned=p_own_it;

	if (range) {
		_check_auto_hide();

		range->range_changed_signal.connect( this, &RangeOwner::_range_changed );
		range->value_changed_signal.connect( this, &RangeOwner::_value_changed );

		if (get_parent()) {
			update();
			range_ptr_changed();
		}
	} else {
	
		range_owned=false;
	}
}

void RangeOwner::set_auto_hide(bool p_auto_hide) {

	auto_hide=p_auto_hide;
	_check_auto_hide();

	if (!p_auto_hide && !is_visible()) {

	    show();
	} 
	
	if (p_auto_hide && is_visible()) {
	
		hide();
	}
}
RangeOwner::RangeOwner(){

	range=0;
	range_owned=false;
	auto_hide=false;
	set_range( GUI_NEW( Range ), true );

}


RangeOwner::~RangeOwner() {

	if (range_owned) {
		GUI_DELETE( range );
	}

}


}
