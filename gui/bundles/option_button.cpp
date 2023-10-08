
//
// C++ Implementation: option_button
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "option_button.h"
#include "base/skin.h"

namespace GUI {


String OptionButton::get_type() {
	
	return "OptionButton";
}
const StyleBox& OptionButton::stylebox(int p_which) {
	
	switch(p_which) {
		case SB_BUTTON_NORMAL: return Frame::stylebox( SB_OPTIONBUTTON_NORMAL ); break;
		case SB_BUTTON_PRESSED: return Frame::stylebox( SB_OPTIONBUTTON_PRESSED ); break;
		case SB_BUTTON_HOVER: return Frame::stylebox( SB_OPTIONBUTTON_HOVER ); break;
		case SB_BUTTON_FOCUS: return Frame::stylebox( SB_OPTIONBUTTON_FOCUS ); break;
		case SB_BUTTON_CHECKED: return Frame::stylebox( -1 ); break;
		case SB_BUTTON_UNCHECKED: return Frame::stylebox( -1 ); break;
	}
		
	return Frame::stylebox(0); //should never reach here
}
FontID OptionButton::font(int p_which) {
	
	return Frame::font( FONT_OPTIONBUTTON );
}
BitmapID OptionButton::bitmap(int p_which) {

	return -1;
}

const Color& OptionButton::color(int p_which) {
	
	switch (p_which) {
		
		case COLOR_BUTTON_FONT: return Frame::color(COLOR_OPTIONBUTTON_FONT); break;
		case COLOR_BUTTON_SHORTCUT_FONT: return Frame::color(COLOR_OPTIONBUTTON_SHORTCUT_FONT); break;
		
	}
	
	return Frame::color(0);
}

int OptionButton::constant(int p_which) {
	
	switch( p_which ) {
		case C_BUTTON_SEPARATION: return Frame::constant( C_OPTIONBUTTON_SEPARATION ); break;
		case C_BUTTON_EXTRA_MARGIN: return Frame::constant( C_OPTIONBUTTON_EXTRA_MARGIN ); break;
		case C_BUTTON_DISPLACEMENT: return Frame::constant( C_OPTIONBUTTON_DISPLACEMENT ); break;
		case C_BUTTON_CHECKBOX_SIZE: return 0; break;
		case C_BUTTON_HAS_CHECKBOX: return 0; break; 
		case C_BUTTON_LABEL_ALIGN_CENTER: return 0; break;
	}
	
	return 0; //should never reach here
}

void OptionButton::resize(const Size& p_new_size) {
	
	size=p_new_size;
}

void OptionButton::pressed() {
	
	popup->set_size( Size( size.width, 0 ) );
	popup->popup( get_global_pos()+Point( 0, size.height) );
}
void OptionButton::item_activated(int p_which) {
		
	select(p_which);
	selected_signal.call(p_which);
	popup->hide(); //this at end of function to reproduce bug
		
}

int OptionButton::get_size() {
	
	return count; //cached
}

int OptionButton::get_selected() {
	
	return selected;
}
String OptionButton::get_string() {
	
	return get_text();
}

void OptionButton::add_item(const String& p_str) {
	
	popup->add_item( p_str,count );
	count++;
	if (selected==-1)
		select( 0 );
	
}
void OptionButton::add_item(BitmapID p_ID,const String& p_str) {
	
	popup->add_item( p_ID, p_str,count );
	count++;
}

String OptionButton::get_item_string(int p_at) {
	
	return popup->get_item_text( p_at );
}

void OptionButton::select(int p_at) {
	
	if (!popup->has_ID(p_at))
		return;
	
	selected=p_at;
	set_text( popup->get_item_text( p_at ) );
	set_icon( popup->get_item_icon( p_at ) );
}

void OptionButton::clear() {
	
	popup->clear();
	selected=-1;
	count=0;
	set_text("");
	set_icon(-1);
}

void OptionButton::set_in_window() {
	
	popup = GUI_NEW( PopUpMenu( get_window() ) );
	popup->selected_id_signal.connect( this, &OptionButton::item_activated );	
}

OptionButton::OptionButton() {
	count=0;
	selected=-1;
}


OptionButton::~OptionButton() {
	
	GUI_DELETE( popup );
}


}
