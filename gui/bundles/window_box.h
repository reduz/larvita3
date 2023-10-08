//
// C++ Interface: window_box
//
// Description: 
//
//
// Author: Juan Linietsky <reduz@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIWINDOW_BOX_H
#define PIGUIWINDOW_BOX_H

#include "containers/box_container.h"



namespace GUI {

/**
	@author Juan Linietsky <reduz@gmail.com>
*/

class Widget;
class WindowTop;	
	
class WindowBox : public VBoxContainer {
	
	Widget *spacer;
	WindowTop *top;
	VBoxContainer * internal_vb;
	HBoxContainer * top_hb;
	void set_in_window();
public:
	
	void add_frame(Frame *p_frame,int p_stretch=0);
	
	/* Very helpful templatized handler for reducing code size */
	template<class T>
	inline T *add(T *p_frame,int p_stretch=0) {
		
		internal_vb->add_frame(p_frame,p_stretch);
		return p_frame;
	}
	
	HBoxContainer *get_top_hb();
	WindowTop * get_window_top();

	WindowBox(String p_top_text="",bool p_close=true,bool p_resize=false);
	~WindowBox();

};

}

#endif
