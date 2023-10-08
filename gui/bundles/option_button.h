//
// C++ Interface: option_button
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIOPTION_BUTTON_H
#define PIGUIOPTION_BUTTON_H

#include "widgets/button.h"
#include "bundles/popup_menu.h"

namespace GUI {

class OptionButton : public Button {
	
	virtual const StyleBox& stylebox(int p_which);
	virtual FontID font(int p_which);
	virtual BitmapID bitmap(int p_which);	
	virtual int constant(int p_which);
	virtual const Color& color(int p_which);
	
	virtual String get_type();
	
	PopUpMenu *popup;
	int selected;
	int count;
	
	void set_in_window();
	
	void item_activated(int p_which);
	
	virtual void pressed();
	Size size;
	void resize(const Size& p_new_size);
	
public:

	Signal< Method1<int> > selected_signal; /// Something was selected
		
	int get_size(); ///< return amount of elements in list
	int get_selected(); ///< Index of selected item
	String get_string(); ///< String of selected  item, or default for the first
	
	void add_item(const String& p_str);
	void add_item(BitmapID p_ID,const String& p_str="");
	
	String get_item_string(int p_at);
	
	void select(int p_at); 

	void clear(); ///< Clear the list
	
	OptionButton();
	~OptionButton();
	
};

};

#endif
