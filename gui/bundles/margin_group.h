//
// C++ Interface: margin_group
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIMARGIN_GROUP_H
#define PIGUIMARGIN_GROUP_H

#include "containers/box_container.h"
#include "base/widget.h"

namespace GUI {


class Label;

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class MarginGroup : public VBoxContainer {


	Widget *spacer;
	VBoxContainer * internal_vb;
	HBoxContainer * label_hb;
	Label *label;
	void set_in_window();
public:

	void add_frame(Frame *p_frame,int p_stretch=0);
	
	/* Very helpful templatized handler for reducing code size */
	template<class T>
	inline T *add(T *p_frame,int p_stretch=0) {
		internal_vb->add_frame(p_frame,p_stretch);
		return p_frame;
	}
	
	HBoxContainer *get_label_hb();
	void set_label_text(String p_label);
	
	MarginGroup(String p_label="");
	~MarginGroup();

};

}

#endif
