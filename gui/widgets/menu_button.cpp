//
// C++ Implementation: menu_button
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "menu_button.h"
#include "base/skin.h"
namespace GUI {

String MenuButton::get_type() {
	
	return "MenuButton";	
}
const StyleBox& MenuButton::stylebox(int p_which) {
	
	switch(p_which) {
		case SB_BUTTON_NORMAL: return Frame::stylebox( SB_MENUBUTTON_NORMAL ); break;
		case SB_BUTTON_PRESSED: return Frame::stylebox( SB_MENUBUTTON_PRESSED ); break;
		case SB_BUTTON_HOVER: return Frame::stylebox( SB_MENUBUTTON_HOVER ); break;
		case SB_BUTTON_FOCUS: return Frame::stylebox( SB_MENUBUTTON_FOCUS ); break;
		case SB_BUTTON_CHECKED: return Frame::stylebox( SB_MENUBUTTON_CHECKED ); break;
		case SB_BUTTON_UNCHECKED: return Frame::stylebox( SB_MENUBUTTON_UNCHECKED ); break;
	}		
		
	return Frame::stylebox(p_which); //should never reach here
}
FontID MenuButton::font(int p_which) {
	
	return Frame::font( FONT_MENUBUTTON );
}

const Color& MenuButton::color(int p_which) {
	
	switch (p_which) {
		
		case COLOR_BUTTON_FONT: return Frame::color(COLOR_MENUBUTTON_FONT); break;
		case COLOR_BUTTON_FONT_HOVER: return Frame::color(COLOR_MENUBUTTON_FONT_HOVER); break;
		case COLOR_BUTTON_SHORTCUT_FONT: return Frame::color(COLOR_MENUBUTTON_SHORTCUT_FONT); break;
		
	}
	
	return Frame::color(p_which);
}

BitmapID MenuButton::bitmap(int p_which) {
	
	switch( p_which ) {
		
		case BITMAP_BUTTON_CHECKED: return Frame::bitmap( BITMAP_MENUBUTTON_CHECKED ); break;
		case BITMAP_BUTTON_UNCHECKED: return Frame::bitmap( BITMAP_MENUBUTTON_UNCHECKED ); break;
	}
	
	return Frame::bitmap( p_which ); //should never reach here
}
int MenuButton::constant(int p_which) {
	
	switch( p_which ) {
		case C_BUTTON_SEPARATION: return Frame::constant( C_MENUBUTTON_SEPARATION ); break;
		case C_BUTTON_EXTRA_MARGIN: return Frame::constant( C_MENUBUTTON_EXTRA_MARGIN ); break;
		case C_BUTTON_DISPLACEMENT: return Frame::constant( C_MENUBUTTON_DISPLACEMENT ); break;
		case C_BUTTON_CHECKBOX_SIZE: return Frame::constant( C_MENUBUTTON_CHECKBOX_SIZE ); break;
		case C_BUTTON_HAS_CHECKBOX: return has_check; break; //checkbutton always has checkbox
		case C_BUTTON_LABEL_ALIGN_CENTER: return 0; break;		
	}
	
	return Frame::constant(p_which); //should never reach here
}

void MenuButton::set_id(int p_id) {
	
	id=p_id;
}
int MenuButton::get_id() {
	
	return id;
}

void *MenuButton::get_userdata() {
	
	return userdata;
}
void MenuButton::set_userdata(void *p_userdata) {
	
	userdata=p_userdata;	
}

void MenuButton::pressed() {
	
	menubutton_pressed_signal.call(this);
}
void MenuButton::toggled(bool p_on) {
	
	menubutton_toggled_signal.call(this,p_on);
}

MenuButton::MenuButton(BitmapID p_icon,bool p_has_check) : Button(p_icon) {
	
	id=-1;
	set_toggle_mode(p_has_check);	
	has_check=p_has_check;
	userdata=0;
}

bool MenuButton::is_checked() {
	
	return is_pressed();
}
void MenuButton::set_checked(bool p_checked) {
	
	set_pressed(p_checked);
}

MenuButton::MenuButton(String p_text,BitmapID p_icon,bool p_has_check) : Button(p_text,p_icon) {
	
	id=-1;
	set_toggle_mode(p_has_check);
	has_check=p_has_check;
	userdata=0;
}

};
