//
// C++ Interface: custom_label
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CUSTOM_LABEL_H
#define CUSTOM_LABEL_H

#include "widgets/label.h"

namespace GUI {
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class CustomLabel : public Label {
	
	int skin_font;
	FontID font_id;
	virtual FontID font(int p_which);
public:
	void set_font(FontID p_font);
	FontID get_font() const;
	void set_skin_font(int p_font);
	int get_skin_font() const;
	
	CustomLabel(String p_text="",Align p_align=ALIGN_LEFT);
	CustomLabel(int p_skin_font=-1,String p_text="",Align p_align=ALIGN_LEFT);
	
	~CustomLabel();

};

}

#endif
