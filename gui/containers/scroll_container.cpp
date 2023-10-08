//
// C++ Implementation: scroll_container
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "scroll_container.h"
#include <stdio.h>

namespace GUI {

	
void ScrollContainer::mouse_button(const GUI::Point& p_pos, int p_button,bool p_press,int p_modifier_mask) {
	
	if (p_modifier_mask&KEY_MASK_ALT) {
		
		if (p_button==BUTTON_WHEEL_DOWN)
			h_scroll.set( h_scroll.get() - h_scroll.get_page() / 8.0);
		if (p_button==BUTTON_WHEEL_UP)
			h_scroll.set( h_scroll.get() + h_scroll.get_page() / 8.0);
	} else {
		
		if (p_button==BUTTON_WHEEL_DOWN)
			v_scroll.set( v_scroll.get() + v_scroll.get_page() / 8.0);
		if (p_button==BUTTON_WHEEL_UP)
			v_scroll.set( v_scroll.get() - v_scroll.get_page() / 8.0);
		
	}
			
	if (p_button==BUTTON_WHEEL_UP || p_button==BUTTON_WHEEL_DOWN)
		get_window()->stop_event();
}
	
	
void ScrollContainer::set_expand_h(bool p_enable) {

	expand_h=p_enable;

	if (!get_window())
		return;

	if (!get_window()->get_painter())
		return;
	
	adjust_minimum_size();
}
void ScrollContainer::set_expand_v(bool p_enable) {


	expand_v=p_enable;

	if (!get_window())
		return;

	if (!get_window()->get_painter())
		return;
	
	adjust_minimum_size();

}
bool ScrollContainer::get_expand_h() {


	return expand_h;
}
bool ScrollContainer::get_expand_v() {


	return expand_v;
}

void ScrollContainer::set_scroll_h(bool p_enable) {

	scroll_h=p_enable;

	if (!get_window())
		return;

	if (!get_window()->get_painter())
		return;
	
	adjust_minimum_size();
}
void ScrollContainer::set_scroll_v(bool p_enable) {


	scroll_v=p_enable;

	if (!get_window())
		return;

	if (!get_window()->get_painter())
		return;
	
	adjust_minimum_size();

}
bool ScrollContainer::get_scroll_h() {


	return scroll_h;
}
bool ScrollContainer::get_scroll_v() {


	return scroll_v;
}



Range* ScrollContainer::get_h_range() {

	return &h_scroll;
}
Range* ScrollContainer::get_v_range() {


	return &v_scroll;
}

void ScrollContainer::v_scroll_changed(double p_new) {

	Element *e=get_element_list();
	if (!e)
		return;
			
	e->rect.pos.y=-p_new;
	
	update();
}
void ScrollContainer::h_scroll_changed(double p_new) {


	Element *e=get_element_list();
	if (!e)
		return;
			
	
	e->rect.pos.x=-p_new;
	
	update();
}

void ScrollContainer::check_minimum_size() {


	Container::check_minimum_size();
	adjust_child_size();
}

Size ScrollContainer::get_minimum_size_internal() {
	
	Size minsize;
	
	Element *e=get_element_list();
	if (!e)
		return minsize;
			
	minsize=e->frame->get_minimum_size();
	
	if (scroll_h)
		minsize.width=0;
	if (scroll_v)
		minsize.height=0;
	
	
	return minsize;
}


void ScrollContainer::adjust_child_size() {

	Size minsize;
	
	Element *e=get_element_list();
	if (!e)
		return;
			
	minsize=e->frame->get_minimum_size();

	if (expand_h && get_size_cache().width>minsize.width)
		minsize.width=get_size_cache().width;
	
	
	h_scroll.set_min(0);
	h_scroll.set_max(minsize.width);
	h_scroll.set_page(get_size_cache().width);

	if (get_size_cache().width>minsize.width)
		h_scroll.set(0);
		   
	if (expand_v && get_size_cache().height>minsize.height)
		minsize.height=get_size_cache().height;
	
	
	v_scroll.set_min(0);
	v_scroll.set_max(minsize.height);
	v_scroll.set_page(get_size_cache().height);

	if (get_size_cache().height>minsize.height)
		v_scroll.set(0);
	
	e->rect.pos.x=-(int)(h_scroll.get());
	e->rect.pos.y=-(int)(v_scroll.get());

	e->rect.size=minsize;
	e->frame->resize_tree( minsize );

}

void ScrollContainer::resize_internal(const Size& p_new_size) {
	
	adjust_child_size();
}


void ScrollContainer::set_frame(Frame *p_frame) {
	
	if (get_element_list()) //this container can only add one child
		return;
	
	add_frame_internal( p_frame);
	
}

ScrollContainer::ScrollContainer()
{
	
	expand_h=false;
	expand_v=false;
	scroll_h=true;
	scroll_v=true;
	
	set_clipping( true );

	h_scroll.value_changed_signal.connect( this, &ScrollContainer::h_scroll_changed );
	v_scroll.value_changed_signal.connect( this, &ScrollContainer::v_scroll_changed );
	
	
}


ScrollContainer::~ScrollContainer()
{
}


}
