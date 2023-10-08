//
// C++ Implementation: combo_box
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "combo_box.h"

#include "widgets/line_edit.h"
#include "widgets/arrow_button.h"
#include "widgets/list.h"
#include "widgets/scroll_bar.h"

namespace GUI {


void ComboBox::resize(const Size& p_new_size) {

	Container::resize(p_new_size);

	size=p_new_size;
}

void ComboBox::skin_changed() {

	HBoxContainer::skin_changed();

}


const StyleBox & ComboBox::HBCCombo::stylebox(int p_which) {

	if (p_which == SB_POPUP_BG)
		return Frame::stylebox(SB_COMBO_POPUP);

	return Frame::stylebox(p_which);
}

void ComboBox::button_clicked() {

	if (list->get_size()==0)
		return;

	Point global=get_global_pos();
	popup->set_pos( Point( global.x , global.y+size.height) );
	int mins=list->get_min_total_height()+get_window()->get_painter()->get_stylebox_min_size(hbc->stylebox(SB_COMBO_POPUP)).height;
	if (mins>constant( C_COMBOBOX_POPUP_MAX_HEIGHT ) )
		mins=constant( C_COMBOBOX_POPUP_MAX_HEIGHT );
	popup->set_size( Size( size.width, mins) );
	popup->show();
	list->get_focus();
}



int ComboBox::get_size() {

	if (!list)
		return 0;

	return list->get_size();
}
int ComboBox::get_selected() {

	if (!list)
		return 0;

	return list->get_selected();

}
String ComboBox::get_string() {

	return line_edit->get_text();

}

void ComboBox::add_string(const String& p_str,int p_at) {

	if (!list)
		return;

	setting_line_edit=true;
	if (!get_size())
		line_edit->set_text( p_str );
	setting_line_edit=false;
	list->add_string( p_str, p_at );
	if (get_size()==1)
		list->select( 0 );


}

void ComboBox::add_sorted_string(const String& p_str) {

	if (!list)
		return;

	setting_line_edit=true;
	if (!get_size())
		line_edit->set_text( p_str );
	setting_line_edit=false;
	list->add_sorted_string( p_str );
	if (get_size()==1)
		list->select( 0 );

}

String ComboBox::get_string(int p_at) {

	if (!list)
		return "";

	return list->get_string( p_at );

}
void ComboBox::set_string(const String& p_str,int p_at) {

	if (!list)
		return;

	list->set_string( p_str, p_at );
	setting_line_edit=true;
	if (list->get_selected()==p_at)
		line_edit->set_text( list->get_selected_string() );
	setting_line_edit=false;

}

void ComboBox::erase(int p_at) {

	if (!list)
		return;

	return list->erase( p_at );

}

void ComboBox::select(int p_at) {

	if (!list)
		return;

	list->select( p_at );
	setting_line_edit=true;
	line_edit->set_text( list->get_selected_string() );
	setting_line_edit=false;

}

void ComboBox::clear() {

	list->clear();
	line_edit->clear();
}

LineEdit *ComboBox::get_line_edit() {

	return line_edit;
}

void ComboBox::line_edit_enter_pressed(String p_string) {

	if (!list || (mode!=MODE_EDIT && mode!=MODE_ADD))
		return;

	if (mode==MODE_EDIT)
		list->set_string( p_string, list->get_selected() );
	else if (mode==MODE_ADD) {
		list->add_string( p_string );
		line_edit->clear();
	}
}

void ComboBox::list_selection_changed_slot(int p_which) {

	popup->hide();
	setting_line_edit=true;
	line_edit->set_text( list->get_selected_string() );
	setting_line_edit=false;
	selected_signal.call( p_which );

}

int ComboBox::find_string_index(String p_string) {

	return list->find_string_index( p_string );
}
void ComboBox::set_in_window() {

	Container::set_in_window();

	popup = GUI_NEW( Window(get_window(),Window::MODE_POPUP) );

	hbc = GUI_NEW( HBCCombo );
	popup->set_root_frame( hbc );
	hbc->add( list, 1);
	list->set_cursor_hint( true );
	ScrollBar *sb = hbc->add( GUI_NEW( ScrollBar(VERTICAL)), 0);
	sb->set_range( list->get_range() );
	sb->set_auto_hide( true );
	list->selected_signal.connect( this, &ComboBox::list_selection_changed_slot );


}

ComboBox::ComboBox(Mode p_mode) {

	list=GUI_NEW( List );
	mode=p_mode;
	line_edit = add( GUI_NEW( LineEdit ), 1 );
	if (p_mode==MODE_EDIT || p_mode==MODE_ADD)
		line_edit->text_enter_signal.connect( this, &ComboBox::line_edit_enter_pressed );
	if (p_mode==MODE_SELECT)
		line_edit->set_editable( false );

	arrow_button = add( GUI_NEW( ArrowButton( DOWN )) , 0 );
	arrow_button->set_focus_mode( FOCUS_NONE );

	arrow_button->pressed_signal.connect( this, &ComboBox::button_clicked );
	line_edit->non_editable_clicked_signal.connect( this, &ComboBox::button_clicked );

	setting_line_edit=false;
	popup=NULL;
	set_fill_vertical( false );
	set_separation(0);
}


ComboBox::~ComboBox(){

	if (popup) {

		GUI_DELETE( popup );
	}
}


}
