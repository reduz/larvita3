//
// C++ Interface: window_button
//
// Description: 
//
//
// Author: Juan Linietsky <reduz@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIWINDOW_BUTTON_H
#define PIGUIWINDOW_BUTTON_H

#include "widgets/base_button.h"

namespace GUI  {

/**
	@author Juan Linietsky <reduz@gmail.com>
*/
class WindowButton : public BaseButton {
public:	
	
	enum Mode {
		MODE_CLOSE,
		MODE_FULL,
		MODE_MIN
	};
private:		
	
	Mode mode;
	
	Size get_minimum_size_internal();
	void draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed);
	
	virtual void pressed();
	
public:
	
	WindowButton(Mode p_mode=MODE_CLOSE);
	~WindowButton();

};

}

#endif
