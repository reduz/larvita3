//
// C++ Interface: check_button
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUICHECK_BUTTON_H
#define PIGUICHECK_BUTTON_H


#include "widgets/button.h"

namespace GUI {

class CheckButton : public Button {

	virtual const StyleBox& stylebox(int p_which);
	virtual FontID font(int p_which);
	virtual BitmapID bitmap(int p_which);	
	virtual int constant(int p_which);
	virtual const Color& color(int p_which);

public:
	
	bool is_checked(); ///< Same as is_pressed()
	void set_checked(bool p_checked);
	
	CheckButton(BitmapID p_icon);
	CheckButton(String p_text="",BitmapID p_icon=-1);
};

};

#endif
