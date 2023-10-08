//
// C++ Interface: range_owner
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIRANGE_OWNER_H
#define PIGUIRANGE_OWNER_H

#include "base/widget.h"
#include "base/range.h"

namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class RangeOwner : public Widget {

	RangeBase *range;
	bool range_owned;
	bool auto_hide;

	/* signals can't connect to virtual functions */
	
	void _check_auto_hide();
	void _range_changed();
	void _value_changed(double p_new_val);
	
protected:

	virtual void range_changed() {}
	virtual void value_changed(double p_new_val) {}
	virtual void range_ptr_changed() {}
	
public:

	RangeBase *get_range();
	void set_range(RangeBase *p_range,bool p_own_it=false);
	void set_auto_hide(bool p_auto_hide);
	RangeOwner();
	~RangeOwner();

};

}

#endif
