//
// C++ Interface: stack_container
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUISTACK_CONTAINER_H
#define PIGUISTACK_CONTAINER_H

#include "base/container.h"

namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class StackContainer : public Container {
	
	virtual Size get_minimum_size_internal();
	void resize_internal(const Size& p_new_size);
	
public:
	
	Signal< Method1<int> > child_raised_signal;
	
	void add_frame(Frame *p_child,bool p_at_top=false);
	
	template<class T>
	T* add(T* p_child, bool p_at_top=false) {
		
		add_frame( p_child, p_at_top );
		return p_child;
	}
	
	void raise(int p_index);
	void raise_frame(Frame *p_child);
	
	StackContainer();
	~StackContainer();

};

}

#endif
