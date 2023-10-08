//
// C++ Implementation: option_box
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "option_box.h"


namespace GUI {


String OptionBox::get_type() {
	
	return "OptionBox";
}
const StyleBox& OptionBox::stylebox(int p_which) {
	
	switch(p_which) {
		case SB_BUTTON_NORMAL: return Frame::stylebox( SB_OPTIONBOX_NORMAL ); break;
		case SB_BUTTON_PRESSED: return Frame::stylebox( SB_OPTIONBOX_PRESSED ); break;
		case SB_BUTTON_HOVER: return Frame::stylebox( SB_OPTIONBOX_HOVER ); break;
		case SB_BUTTON_FOCUS: return Frame::stylebox( SB_OPTIONBOX_FOCUS ); break;
		case SB_BUTTON_CHECKED: return Frame::stylebox( -1 ); break;
		case SB_BUTTON_UNCHECKED: return Frame::stylebox( -1 ); break;
	}
		
	return Frame::stylebox(p_which); //should never reach here
}
FontID OptionBox::font(int p_which) {
	
	return Frame::font( FONT_OPTIONBOX );
}
BitmapID OptionBox::bitmap(int p_which) {

    return Frame::bitmap(p_which);
}

const Color& OptionBox::color(int p_which) {
	
	switch (p_which) {
		
		case COLOR_BUTTON_FONT: return Frame::color(COLOR_OPTIONBOX_FONT); break;
		case COLOR_BUTTON_FONT_HOVER: return Frame::color(COLOR_OPTIONBOX_FONT_HOVER); break;
		
	}
	
	return Frame::color(p_which);
}
int OptionBox::constant(int p_which) {
	
	switch( p_which ) {
		case C_BUTTON_SEPARATION: return Frame::constant( C_OPTIONBOX_SEPARATION ); break;
		case C_BUTTON_EXTRA_MARGIN: return Frame::constant( C_OPTIONBOX_EXTRA_MARGIN ); break;
		case C_BUTTON_DISPLACEMENT: return Frame::constant( C_OPTIONBOX_DISPLACEMENT ); break;
		case C_BUTTON_CHECKBOX_SIZE: return 0; break;
		case C_BUTTON_HAS_CHECKBOX: return 0; break;
		case C_BUTTON_LABEL_ALIGN_CENTER: return 0; break;
	}
	
	return Frame::constant(p_which); //should never reach here
}

void OptionBox::resize(const Size& p_new_size) {
	
	size=p_new_size;
}

void OptionBox::pressed() {
	
	pre_show_signal.call();
	
	popup->set_size( Size( size.width, 0 ) );
	popup->popup( get_global_pos()+Point( 0, size.height) );
}
void OptionBox::item_activated(int p_which) {
	
	select(p_which);
	selected_signal.call(p_which);
	popup->hide(); //this at end of function to reproduce bug
	
}

void OptionBox::add_item(const String& p_str,int p_id,unsigned int p_shortcut,bool p_shortcut_active) {

	if (!popup) {

		add_to_defered_list( GUI_NEW( DeferredAdd( p_str, p_id, -1,p_shortcut,p_shortcut_active )) );
		return;

	}
	
	
	if (p_id!=-1)
		popup->add_item( p_str,p_id,NULL,p_shortcut,p_shortcut_active );
	else
		popup->add_item( p_str,NULL,p_shortcut,p_shortcut_active );
	
}
void OptionBox::add_item(BitmapID p_ID,const String& p_str,int p_id,unsigned int p_shortcut,bool p_shortcut_active) {
	
	if (!popup) {


		add_to_defered_list( GUI_NEW( DeferredAdd( p_str, p_id, p_ID,p_shortcut,p_shortcut_active )) );
		return;

	}
	
	
	
	
	if (p_id!=-1)
		popup->add_item( p_ID, p_str,p_id,NULL,p_shortcut,p_shortcut_active);
	else
		popup->add_item( p_ID, p_str,NULL,p_shortcut,p_shortcut_active );

}

void OptionBox::add_separator() {

	if (!popup) {

		add_to_defered_list( GUI_NEW( DeferredAdd ) );
		return;

	}

	popup->add_separator();
}

void OptionBox::add_to_defered_list( DeferredAdd *p_elem ) {
	
	if ( deferred_add_list ) {
		DeferredAdd *last=deferred_add_list;
		while (last->next) {
				
			last=last->next;
		}
		last->next = p_elem;
	} else {
		
		deferred_add_list = p_elem;

	}
	
}


void OptionBox::select(int p_at) {
	
	if (!popup) {
		
		DeferredAdd *dfl = deferred_add_list;
		
		while (dfl) {
		
			if (dfl->id==p_at) {
			
				selected=p_at;
				set_icon( dfl->bitmap );
				set_text( dfl->text );
				return;
			}
			
			dfl=dfl->next;
		}
		
	} else if (popup->has_ID(p_at)) {
			
		selected=p_at;
		set_text( popup->get_item_text( p_at ) );
		set_icon( popup->get_item_icon( p_at ) );
	}
		
}
int OptionBox::get_selected() {
	
	return selected;
}


void OptionBox::clear_deferred_list() {


	while (deferred_add_list) {

		DeferredAdd *aux=deferred_add_list;

		deferred_add_list=deferred_add_list->next;
		GUI_DELETE( aux );
	}
	
}

void OptionBox::clear() {

	if (popup)
		popup->clear();

	clear_deferred_list();
}

void OptionBox::set_in_window() {
	
	popup = GUI_NEW( PopUpMenu( get_window() ) );
	popup->selected_id_signal.connect( this, &OptionBox::item_activated );
	
	DeferredAdd *l=deferred_add_list;
	while (l) {
		
		if (l->is_sep)
			add_separator();
		else if (l->bitmap<0)
			add_item( l->text, l->id,l->shortcut, l->shortcut_active );
		else
			add_item( l->bitmap, l->text, l->id,l->shortcut, l->shortcut_active );
		
		l=l->next;		
	}
	
	clear_deferred_list();
}

OptionBox::OptionBox(String p_text,BitmapID p_bitmap_ID) : Button(p_text,p_bitmap_ID) {

	deferred_add_list=0;
	popup=0;
	selected=0;
}


OptionBox::~OptionBox() {

	if (popup) {
		GUI_DELETE( popup );
	}
	clear_deferred_list();
}


}
