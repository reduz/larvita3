//
// C++ Implementation: fixed_container
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "fixed_container.h"

namespace GUI {


Container::Element *FixedContainer::create_new_element() {
	
	return GUI_NEW( FixedElement );
}

Size FixedContainer::get_minimum_size_internal() {
	
	FixedElement * fe= (FixedElement*)get_element_list(); 
	
	Size minimum;
	
	while (fe) {
		
		Size elem_min=fe->frame->get_minimum_size();
		
		if (fe->desired_size.width>elem_min.width)
			elem_min.width=fe->desired_size.width;
		if (fe->desired_size.height>elem_min.height)
			elem_min.height=fe->desired_size.height;
			
		if ( (fe->desired_pos.x+elem_min.width)>minimum.width)
			minimum.width=fe->desired_pos.x+elem_min.width;
		
		if ( (fe->desired_pos.y+elem_min.height)>minimum.height)
			minimum.height=fe->desired_pos.y+elem_min.height;
		
		fe=(FixedElement*)fe->next;
	}

	return minimum;
}
void FixedContainer::resize_internal(const Size& p_new_size) {
	
	
	FixedElement * fe= (FixedElement*)get_element_list(); 
	
	
	while (fe) {
		
		Size elem_min=fe->frame->get_minimum_size();
		
		if (fe->desired_size.width>elem_min.width)
			elem_min.width=fe->desired_size.width;
		if (fe->desired_size.height>elem_min.height)
			elem_min.height=fe->desired_size.height;
			
		fe->rect.size=elem_min;
		fe->rect.pos=fe->desired_pos;
		
		fe->frame->resize_tree( fe->rect.size );
		
		fe=(FixedElement*)fe->next;
	}



}

void FixedContainer::add_frame(Frame *p_frame,const Point& p_pos,const Size&p_size) {
	
	FixedElement * e=(FixedElement *)add_frame_internal( p_frame );
	if (!e)
		return;
	e->desired_pos=p_pos;
	e->desired_size=p_size;

}
	

void FixedContainer::set_child_pos(Frame *p_frame,const Point &p_pos) {
	
	FixedElement * fe= (FixedElement*)get_element_list(); 
	
	Size minimum;
	
	while (fe) {
		
		if (fe->frame==p_frame) {
			
			fe->desired_pos=p_pos;
			
			check_minimum_size();
			update();
			return;
			
		}
		
		fe=(FixedElement*)fe->next;
		
	}
	
}

void FixedContainer::set_child_size(Frame *p_frame,const Size &p_size) {
	
	
	FixedElement * fe= (FixedElement*)get_element_list(); 
	
	Size minimum;
	
	while (fe) {
		
		if (fe->frame==p_frame) {
			
			fe->desired_size=p_size;
			
			check_minimum_size();
			update();
			return;
			
		}
		
		fe=(FixedElement*)fe->next;
		
	}
	
}

Point FixedContainer::get_child_pos(Frame *p_frame) {
	
	FixedElement * fe= (FixedElement*)get_element_list(); 
	
	Size minimum;
	
	while (fe) {
		
		if (fe->frame==p_frame) {
			
			return fe->desired_pos;
			
		}
		
		fe=(FixedElement*)fe->next;
		
	}
	
	return Point();
}
Size FixedContainer::get_child_size(Frame *p_frame) {
	
	FixedElement * fe= (FixedElement*)get_element_list(); 
	
	Size minimum;
	
	while (fe) {
		
		if (fe->frame==p_frame) {
			
			return fe->desired_size;
			
		}
		
		fe=(FixedElement*)fe->next;
		
	}
	
	return Size();
	
}
	

FixedContainer::FixedContainer() {
	
	
}


FixedContainer::~FixedContainer() {
	
	
}


}
