//
// C++ Implementation: center_container
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "center_container.h"

namespace GUI {

void CenterContainer::set_expand_h(bool p_expand) {

	expand_h=p_expand;
	adjust_minimum_size();
}	
bool CenterContainer::has_expand_h() const {

	return expand_h;
}
void CenterContainer::set_expand_v(bool p_expand) {

	expand_v=p_expand;
	adjust_minimum_size();
}
bool CenterContainer::has_expand_v() const {

	return expand_v;
}

Size CenterContainer::get_minimum_size_internal() {
	
	Size min;
		
	Element *e=get_element_list();
	
	if (e) {
		
		min=e->frame->get_minimum_size();
	}
	
	return min;
}

void CenterContainer::resize_internal(const Size& p_new_size) {
	
		
	Element *e=get_element_list();
	
	if (e) {

		Size min;
		min=e->frame->get_minimum_size();
		
		Rect final_rect( (p_new_size-min)/2, min );
		
		if (expand_h) {
		
			final_rect.pos.x=0;
			final_rect.size.x=p_new_size.x;
		}
		
		if (expand_v) {
		
			final_rect.pos.y=0;
			final_rect.size.y=p_new_size.y;
		}
		
		e->set_rect(final_rect);
			
		e->frame->resize_tree(final_rect.size);
	}
	
}
	
	
void CenterContainer::set_frame(Frame *p_frame) {
	
	if (get_element_list()) {
		
		PRINT_ERROR("Only ONE child allowed into CenterContainers...");
		return;
	}
	
	add_frame_internal( p_frame );
	check_minimum_size();
}

CenterContainer::CenterContainer()
{
	expand_h=false;
	expand_v=false;
}


CenterContainer::~CenterContainer()
{
}


}
