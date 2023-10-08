//
// C++ Implementation: scroll_box
//
// Description:
//
//
// Author: Juan Linietsky <reshaked@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "scroll_box.h"

namespace GUI {

void ScrollBox::set_expand_h(bool p_enable) {

	scroll->set_expand_h(p_enable);
}
void ScrollBox::set_expand_v(bool p_enable) {

	scroll->set_expand_v(p_enable);

}
bool ScrollBox::get_expand_h() {

	return scroll->get_expand_h();

}
bool ScrollBox::get_expand_v() {

	return scroll->get_expand_v();

}

void ScrollBox::set_scroll_h(bool p_enable) {

	scroll->set_scroll_h(p_enable);
}
void ScrollBox::set_scroll_v(bool p_enable) {

	scroll->set_scroll_v(p_enable);

}
bool ScrollBox::get_scroll_h() {

	return scroll->get_scroll_h();
}
bool ScrollBox::get_scroll_v() {

	return scroll->get_scroll_v();

}

ScrollContainer * ScrollBox::get_scroll_container() {

	return scroll;
}

Range* ScrollBox::get_h_range() {

	return scroll->get_h_range();
}
Range* ScrollBox::get_v_range() {

	return scroll->get_v_range();
}

HScrollBar *ScrollBox::get_h_scrollbar() {

	return hbar;
}
VScrollBar *ScrollBox::get_v_scrollbar() {

	return vbar;
}

void ScrollBox::set_frame(Frame *p_frame) {

	scroll->set_frame(p_frame);
}

ScrollBox::ScrollBox( bool p_buttons ) : GridContainer(2)
{

	scroll = GridContainer::add( GUI_NEW( ScrollContainer), true, true );
	vbox = GridContainer::add( GUI_NEW( VBoxContainer ), false, true );
	hbox = GridContainer::add( GUI_NEW( HBoxContainer ), true, false );
	//set_separation(0);
	vbox->set_separation(0);	
	hbox->set_separation(0);
	
	if ( p_buttons ) {

		vbar_button_up = vbox->add( GUI_NEW( ArrowButton( GUI::UP ) ), 0 );
		vbar = vbox->add( GUI_NEW( VScrollBar), 1 );
		vbar_button_down = vbox->add( GUI_NEW( ArrowButton( GUI::DOWN ) ), 0 );

		hbar_button_left = hbox->add( GUI_NEW( ArrowButton( GUI::LEFT ) ), 0 );
		hbar = hbox->add( GUI_NEW( HScrollBar), 1 );
		hbar_button_right = hbox->add( GUI_NEW( ArrowButton( GUI::RIGHT ) ), 0 );

		vbar_button_up->set_range(scroll->get_v_range());
		vbar_button_down->set_range(scroll->get_v_range());
		hbar_button_left->set_range(scroll->get_h_range());
		hbar_button_right->set_range(scroll->get_h_range());

		vbar_button_up->set_auto_hide(true);
		vbar_button_down->set_auto_hide(true);
		hbar_button_left->set_auto_hide(true);
		hbar_button_right->set_auto_hide(true);
		vbar_button_up->set_page_increment(true);
		vbar_button_down->set_page_increment(true);
		hbar_button_left->set_page_increment(true);
		hbar_button_right->set_page_increment(true);

	} else {

		vbar = vbox->add( GUI_NEW( VScrollBar), 1 );
		hbar = hbox->add( GUI_NEW( HScrollBar), 1 );
	}

	vbar->set_range(scroll->get_v_range());
	hbar->set_range(scroll->get_h_range());

	vbar->set_auto_hide(true);
	hbar->set_auto_hide(true);
}


ScrollBox::~ScrollBox()
{
}


}
