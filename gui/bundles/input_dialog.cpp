//
// C++ Implementation: input_dialog
//
// Description:
//
//
// Author: Juan Linietsky <reshaked@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "input_dialog.h"
#include "containers/center_container.h"
#include "widgets/button.h"
#include "widgets/line_edit.h"

namespace GUI {


void NumericInputDialog::show(String p_question,double p_min,double p_max,double p_step,double p_default) {

	mg->set_label_text(p_question);
	sb->get_range()->set_min(p_min);
	sb->get_range()->set_max(p_max);
	sb->get_range()->set_step(p_step);
	sb->get_range()->set(p_default);
	sb->get_line_edit()->get_focus();
	sb->get_line_edit()->select_all();
	Window::show();
}

void NumericInputDialog::ok_pressed_text(String) {

	ok_pressed();
}
void NumericInputDialog::ok_pressed() {

	hide();
	entered_number_signal.call( sb->get_range()->get() );

}
NumericInputDialog::NumericInputDialog(Window *p_parent) : Window(p_parent,MODE_POPUP,SIZE_CENTER) {

 	mg = GUI_NEW( MarginGroup("Enter a Number:") );
	set_root_frame( mg );
	sb = mg->add( GUI_NEW( SpinBox) );
	sb->get_line_edit()->text_enter_signal.connect( this, &NumericInputDialog::ok_pressed_text);

	mg->add( GUI_NEW( CenterContainer))->set( GUI_NEW(Button("Ok"))) ->pressed_signal.connect( this, &NumericInputDialog::ok_pressed );
}

/******* STRING ********/

void StringInputDialog::show(String p_question,String p_current) {

	mg->set_label_text(p_question);
	le->set_text(p_current);
	le->select_all();
	le->get_focus();
	Window::show();
}

void StringInputDialog::ok_pressed_text(String) {

	ok_pressed();
}
void StringInputDialog::ok_pressed() {

	hide();
	entered_string_signal.call( le->get_text() );

}
StringInputDialog::StringInputDialog(Window *p_parent) : Window(p_parent,MODE_POPUP,SIZE_CENTER) {

	mg = GUI_NEW( MarginGroup("Enter a Number:") );
	set_root_frame( mg );
	le = mg->add( GUI_NEW( LineEdit) );
	le->text_enter_signal.connect( this, &StringInputDialog::ok_pressed_text);

	mg->add( GUI_NEW(CenterContainer))->set( GUI_NEW( Button("Ok"))) ->pressed_signal.connect( this, &StringInputDialog::ok_pressed );
}

/******* QUESTION ********/

void QuestionInputDialog::button_pressed(int p_but) {

	hide();
	button_pressed_signal.call(p_but);
};

void QuestionInputDialog::clear() {

	GUI_DELETE( button_hbox );
	button_hbox = vb->add( GUI_NEW(CenterContainer) )->set( GUI_NEW( HBoxContainer) );
	button_hbox->set_separation(5);
};

Button* QuestionInputDialog::add_button(int p_id, String p_text) {

	Button* but = button_hbox->add(GUI_NEW( Button(p_text)));
	but->pressed_signal.connect(Method( Method1<int>( this, &QuestionInputDialog::button_pressed ), p_id));

	return but;
};

void QuestionInputDialog::show(String p_question) {

	l->set_text(p_question);
	Window::show();
};

QuestionInputDialog::QuestionInputDialog(Window* p_parent) : Window(p_parent,MODE_POPUP,SIZE_CENTER) {

	WindowBox * vb=GUI_NEW( WindowBox );
	set_root_frame(vb);
	l=vb->add( GUI_NEW( Label("Question")) );
	button_hbox = vb->add( GUI_NEW(CenterContainer) )->set( GUI_NEW( HBoxContainer) );
	button_hbox->set_separation(5);
};


void OptionInputDialog::clear() {

	combo->clear();

}
void OptionInputDialog::add_option(String p_option) {

	combo->add_string(p_option);

}
void OptionInputDialog::show(String p_question) {

	question->set_label_text(p_question);
	Window::show();
}

void OptionInputDialog::option_select_callback() {

	int selected = combo->get_selected();
	if ( selected<0 || selected>=combo->get_size() )
		return;

	option_selected_signal.call( selected, combo->get_string(selected) );
	hide();
}

OptionInputDialog::OptionInputDialog(Window* p_parent) : Window(p_parent,MODE_POPUP,SIZE_CENTER) {

	question = GUI_NEW( MarginGroup("Choose:" ) );
	combo=question->add( GUI_NEW( ComboBox ) );
	((VBoxContainer*)question)->add( GUI_NEW( CenterContainer ) )->set( GUI_NEW( Button("Ok") ) )->pressed_signal.connect( this, &OptionInputDialog::option_select_callback );
	set_root_frame( question );

}


}
