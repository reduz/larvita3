//
// C++ Implementation: stack_container
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "stack_container.h"

namespace GUI {


Size StackContainer::get_minimum_size_internal() {
	
	Element *e=get_element_list();
	
	Size minsize;
	
	while (e) {

		if (!e->frame->is_visible()) {
			e = e->next;
			continue;
		};
		Size element_min=e->frame->get_minimum_size();
		
		if (element_min.width>minsize.width)
			minsize.width=element_min.width;
		
		if (element_min.height>minsize.height)
			minsize.height=element_min.height;
		
		e=e->next;
	}
	
	return minsize;
}

void StackContainer::resize_internal(const Size& p_new_size) {
	
	Element *e=get_element_list();
	
	
	while (e) {
		
		if (!e->frame->is_visible()) {
			e = e->next;
			continue;
		};
		e->rect.pos=Point();
		e->rect.size=p_new_size;
		e->frame->resize_tree(p_new_size);
		
		e=e->next;
	}
	
	
}

	
void StackContainer::add_frame(Frame *p_child,bool p_at_top) {
	
	if (get_element_list()) {
		
		if (p_at_top) {
			
			raise(-1); //hide all, show this
		} else {
			p_child->hide(); //hide this, show current
		}
	
	}
	
	add_frame_internal( p_child, p_at_top);
	
}
	
	
void StackContainer::raise(int p_index) {
	
	Element *e=get_element_list_end();
	int idx=0;
	int raised=-1;
	while (e) {
		
		if (idx==p_index) {
			
			e->frame->show();
			raised=idx;
		} else {
				
			e->frame->hide();
		}
		e=e->prev;
		idx++;
	}
	
	if (raised==-1)
		return;
	child_raised_signal.call(raised);
}
void StackContainer::raise_frame(Frame *p_child) {
	
	
	Element *e=get_element_list_end();

	int idx=0;
	int raised=-1;
	while (e) {
		
		if (p_child==e->frame) {
			
			e->frame->show();
			raised=idx;
		} else {
				
			e->frame->hide();
		}
		e=e->prev;
		idx++;
	}
	
	child_raised_signal.call(raised);
}

StackContainer::StackContainer() {
	
	
}


StackContainer::~StackContainer()
{
}


}
