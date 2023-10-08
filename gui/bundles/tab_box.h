//
// C++ Interface: tab_box
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUITAB_BOX_H
#define PIGUITAB_BOX_H


#include "containers/box_container.h"
#include "containers/stack_container.h"
#include "widgets/tab_bar.h"

namespace GUI {


/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class TabBox : public VBoxContainer {
	
	TabBar *tabs;
	
	StackContainer *stack;
	
	void set_in_window();
	void skin_changed();

public:
	
	void add_frame(String p_label,Frame *p_child,bool p_at_top=false);
	
	template<class T>
	T* add(String p_label,T* p_child, bool p_at_top=false) {
		
		add_frame( p_label,p_child, p_at_top );
		return p_child;
	}
	
	void raise(int p_index);
	void raise_frame(Frame *p_child);
	
	TabBar *get_tabs() { return tabs; }
	TabBox();
	~TabBox();
};

}

#endif
