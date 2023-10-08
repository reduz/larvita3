//
// C++ Interface: bitmap_button
//
// Description:
//
//
// Author: Juan,,, <red@hororo>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef GUIBITMAP_BUTTON_H
#define GUIBITMAP_BUTTON_H

#include "widgets/base_button.h"

namespace GUI {

/**
	@author Juan,,, <red@hororo>
*/
class BitmapButton : public BaseButton {

	BitmapID normal;
	BitmapID pressed;
	BitmapID hover;
	BitmapID icon;
	BitmapID disabled;

	Size get_minimum_size_internal();
	void draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed);

public:

	void set_normal_bitmap(BitmapID p_bitmap);
	void set_pressed_bitmap(BitmapID p_bitmap);
	void set_hover_bitmap(BitmapID p_bitmap);
	void set_disabled_bitmap(BitmapID p_bitmap);
	void set_icon_bitmap(BitmapID p_icon);

	BitmapButton(BitmapID p_normal=-1, BitmapID p_pressed=-1, BitmapID p_hover=-1,BitmapID p_icon=-1,BitmapID p_disabled=-1);
	~BitmapButton();

};

}

#endif
