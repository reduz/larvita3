//
// C++ Implementation: tab_box
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "tab_box.h"
#include <stdio.h>

namespace GUI {

	
class TabBoxContainer : public StackContainer { 
	
	
	const StyleBox& stylebox(int p_which) { 
			
		if ( p_which == SB_CONTAINER )
			return Frame::stylebox( SB_TABBOX );
		
		return Frame::stylebox( p_which );
	}
};
	
void TabBox::add_frame(String p_label,Frame *p_child,bool p_at_top) {
	
	tabs->add_tab(p_label,p_at_top);
	stack->add_frame(p_child,p_at_top);
}
		
void TabBox::raise(int p_index) {
	
	tabs->select_tab(p_index);
	stack->raise(p_index);
}
void TabBox::raise_frame(Frame *p_child) {
	
	stack->raise_frame(p_child);
}


void TabBox::skin_changed() {
	
//	printf("Skinchange in tabbox\n");
	set_separation( constant( C_TABBOX_SEPARATION ) );
	Container::skin_changed();	
}

void TabBox::set_in_window() {
	
	set_separation( constant( C_TABBOX_SEPARATION ) );

	
}
TabBox::TabBox() {
	
	tabs=BoxContainer::add( GUI_NEW( TabBar) );
	stack=BoxContainer::add( GUI_NEW( TabBoxContainer),1 );
	tabs->tab_changed_signal.connect( stack, &StackContainer::raise );
	
}


TabBox::~TabBox()
{
}


}
