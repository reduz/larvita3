//
// C++ Interface: button
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIBUTTON_H
#define PIGUIBUTTON_H


#include "widgets/base_button.h"


namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class Button : public BaseButton {
protected:

	BitmapID icon;
	String label_text;
	int accel_char;
	unsigned int shortcut;
	bool shortcut_active;
	bool flat;

	void parse_accelerator();

	Size get_minimum_size_internal();
	void draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed);

	virtual String get_type();

public:

	void set_text(String p_text);
	void set_icon(BitmapID p_icon);

	String::CharType get_accelerator(); ///< 0 means no accelerator

	String get_text();
	bool has_icon();
	BitmapID get_icon();
	
	void set_shurtcut(unsigned int p_shortcut,bool p_active=true);
	
	void set_flat(bool p_flat);

	Button(BitmapID p_icon);
	Button(String p_text="",BitmapID p_icon=-1);
	~Button();

};

}

#endif
