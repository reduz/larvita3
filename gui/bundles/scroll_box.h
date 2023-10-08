//
// C++ Interface: scroll_box
//
// Description:
//
//
// Author: Juan Linietsky <reshaked@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef GUISCROLL_BOX_H
#define GUISCROLL_BOX_H

#include "containers/grid_container.h"
#include "containers/scroll_container.h"
#include "containers/box_container.h"
#include "widgets/scroll_bar.h"
#include "widgets/arrow_button.h"

namespace GUI {

/**
	@author Juan Linietsky <reshaked@gmail.com>
*/
class ScrollBox : public GridContainer {

	ScrollContainer *scroll;
	HScrollBar *hbar;
	VScrollBar *vbar;

	VBoxContainer *vbox;
	HBoxContainer *hbox;
	ArrowButton *vbar_button_up;
	ArrowButton *vbar_button_down;
	ArrowButton *hbar_button_left;
	ArrowButton *hbar_button_right;

public:

	void set_expand_h(bool p_enable);
	void set_expand_v(bool p_enable);
	bool get_expand_h();
	bool get_expand_v();

	void set_scroll_h(bool p_enable);
	void set_scroll_v(bool p_enable);
	bool get_scroll_h();
	bool get_scroll_v();


	Range* get_h_range();
	Range* get_v_range();

	HScrollBar *get_h_scrollbar();
	VScrollBar *get_v_scrollbar();

	void set_frame(Frame *p_frame);

	ScrollContainer * get_scroll_container();

	template<class T>
	T* set( T*p_frame) {

		set_frame(p_frame);
		return p_frame;
	}

	ScrollBox( bool p_buttons = false );
	~ScrollBox();

};

}

#endif
