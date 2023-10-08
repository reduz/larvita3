//
// C++ Interface: tab_bar
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUITAB_BAR_H
#define PIGUITAB_BAR_H

#include "base/widget.h"
namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class TabBar : public Widget {

	struct Tab {
		
		String text;
		
		Tab *next;
		
		int offset;
		bool visible;
		
		Tab() { next=0; offset=0; visible=true; }
	};
	
	
	
	Tab *list;
	int selected;
	Size get_minimum_size_internal();
	void draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed);
	virtual String get_type();	

	
	void mouse_button(const Point& p_pos, int p_button,bool p_press,int p_modifier_mask);
	bool key(unsigned long p_unicode, unsigned long p_scan_code,bool p_press,bool p_repeat,int p_modifier_mask);
	
public:
	
	Signal< Method1<int> > pre_tab_changed_signal;
	Signal< Method1<int> > tab_changed_signal;
	
	int get_selected_tab();
	void set_tab(int p_selected); //set 
	void select_tab(int p_select); //set and emit signal
	
	int get_tab_count();
	void add_tab(String p_text,bool p_front=false);
	
	void set_tab_visible(int p_which, bool p_visible);
	bool is_tab_visible(int p_which);
	TabBar();	
	~TabBar();

};

}

#endif
