//
// C++ Interface: scroll_container
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUISCROLL_CONTAINER_H
#define PIGUISCROLL_CONTAINER_H


#include "base/container.h"
#include "base/range.h"
namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class ScrollContainer : public Container {
	
	
	Range h_scroll;
	Range v_scroll;

	bool expand_h;
	bool expand_v;
	bool scroll_h;
	bool scroll_v;
	
	virtual void mouse_button(const GUI::Point& p_pos, int p_button,bool p_press,int p_modifier_mask);
	
	virtual Size get_minimum_size_internal();
	virtual void resize_internal(const Size& p_new_size);
	virtual void check_minimum_size();
	void adjust_child_size();


	void v_scroll_changed(double p_new);
	void h_scroll_changed(double p_new);
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
	
	void set_frame(Frame *p_frame);
	
	template<class T>
	T* set( T*p_frame) {
		
		set_frame(p_frame);
		return p_frame;
	}
	
	ScrollContainer();
	~ScrollContainer();

};

}

#endif
