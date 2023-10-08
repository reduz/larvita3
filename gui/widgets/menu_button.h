//
// C++ Interface: menu_button
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIMENU_BUTTON_H
#define PIGUIMENU_BUTTON_H

#include "widgets/button.h"

namespace GUI {

class MenuButton : public Button {

	virtual const StyleBox& stylebox(int p_which);
	virtual FontID font(int p_which);
	virtual BitmapID bitmap(int p_which);	
	virtual int constant(int p_which);
	virtual const Color& color(int p_which);

	bool has_check;
	int id;
	void *userdata;
	
	virtual void pressed();
	virtual void toggled(bool p_on);
	
	virtual String get_type();
	
protected:

friend class PopUpMenu;	

	Signal< Method1<MenuButton*> > menubutton_pressed_signal;
	Signal< Method2<MenuButton*,bool> > menubutton_toggled_signal;

	void set_id(int p_id);
	int get_id();
	void set_userdata(void *p_userdata);
	void *get_userdata();
public:	
	bool is_checked(); ///< Same as is_pressed()
	void set_checked(bool p_checked);
	
	MenuButton(BitmapID p_icon,bool p_has_check=false);
	MenuButton(String p_text="",BitmapID p_icon=-1,bool p_has_check=false);
};

};

#endif
