//
// C++ Implementation: check_button
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "check_button.h"
#include "base/skin.h"
namespace GUI {

bool CheckButton::is_checked() {
	
	return is_pressed();
}
void CheckButton::set_checked(bool p_checked) {
	
	set_pressed(p_checked);
}


const StyleBox& CheckButton::stylebox(int p_which) {
	
	switch(p_which) {
		case SB_BUTTON_NORMAL: return Frame::stylebox( SB_CHECKBUTTON_NORMAL ); break;
		case SB_BUTTON_PRESSED: return Frame::stylebox( SB_CHECKBUTTON_PRESSED ); break;
		case SB_BUTTON_HOVER: return Frame::stylebox( SB_CHECKBUTTON_HOVER ); break;
		case SB_BUTTON_FOCUS: return Frame::stylebox( SB_CHECKBUTTON_FOCUS ); break;
		case SB_BUTTON_CHECKED: return Frame::stylebox( SB_CHECKBUTTON_CHECKED ); break;
		case SB_BUTTON_UNCHECKED: return Frame::stylebox( SB_CHECKBUTTON_UNCHECKED ); break;
	}		
		
	return Frame::stylebox(p_which); //should never reach here
}
FontID CheckButton::font(int p_which) {

        if (p_which==FONT_BUTTON)
	    return Frame::font( FONT_CHECKBUTTON );
	else return Frame::font( p_which );
}
BitmapID CheckButton::bitmap(int p_which) {
	
	switch( p_which ) {
		
		case BITMAP_BUTTON_CHECKED: return Frame::bitmap( BITMAP_CHECKBUTTON_CHECKED ); break;
		case BITMAP_BUTTON_UNCHECKED: return Frame::bitmap( BITMAP_CHECKBUTTON_UNCHECKED ); break;
	}
	
	return Frame::bitmap(p_which); //should never reach here
}

const Color& CheckButton::color(int p_which) {
	
	switch (p_which) {
		
		case COLOR_BUTTON_FONT: return Frame::color(COLOR_CHECKBUTTON_FONT); break;
		case COLOR_BUTTON_FONT_HOVER: return Frame::color(COLOR_CHECKBUTTON_FONT_HOVER); break;
		case COLOR_BUTTON_SHORTCUT_FONT: return Frame::color(COLOR_CHECKBUTTON_SHORTCUT_FONT); break;
		
	}
	
	return Frame::color(p_which);
}

int CheckButton::constant(int p_which) {
	
  	switch( p_which ) {
		case C_BUTTON_SEPARATION: return Frame::constant( C_CHECKBUTTON_SEPARATION ); break;
		case C_BUTTON_EXTRA_MARGIN: return Frame::constant( C_CHECKBUTTON_EXTRA_MARGIN ); break;
		case C_BUTTON_DISPLACEMENT: return Frame::constant( C_CHECKBUTTON_DISPLACEMENT ); break;
		case C_BUTTON_CHECKBOX_SIZE: return Frame::constant( C_CHECKBUTTON_CHECKBOX_SIZE ); break;
		case C_BUTTON_HAS_CHECKBOX: return 1; break; //checkbutton always  checkbox
		case C_BUTTON_LABEL_ALIGN_CENTER: return 0; break;
	}
	
	return Frame::constant(p_which); //should never reach here
}

CheckButton::CheckButton(BitmapID p_icon) : Button(p_icon) {
	
	set_toggle_mode(true);	
}
CheckButton::CheckButton(String p_text,BitmapID p_icon) : Button(p_text,p_icon) {
	
	set_toggle_mode(true);	
	
}

};
