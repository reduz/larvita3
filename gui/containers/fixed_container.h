//
// C++ Interface: fixed_container
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIFIXED_CONTAINER_H
#define PIGUIFIXED_CONTAINER_H

#include "base/container.h"

namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class FixedContainer : public Container {
	
	struct FixedElement : public Element {
						
		Point desired_pos; /// desired position
		Size desired_size; /// desired size, if empty, minsize is used
	};
	
	Size get_minimum_size_internal();
	void resize_internal(const Size& p_new_size);
	
	virtual Element *create_new_element();	
	
public:
	
	void add_frame(Frame *p_frame,const Point& p_pos=Point(),const Size &p_size=Size());
	
	/* Helper for ease of coding */
	template<class T>
	inline T* add(T *p_frame,const Point& p_pos=Point(),const Size &p_size=Size()) {

		add_frame(p_frame,p_pos,p_size);
		return p_frame;
	}
	
	void set_child_pos(Frame *p_frame,const Point &p_pos);
	void set_child_size(Frame *p_frame,const Size &p_pos);
	
	Point get_child_pos(Frame *p_frame);
	Size get_child_size(Frame *p_frame);
	
	FixedContainer();
	~FixedContainer();

};

}

#endif
