//
// C++ Interface: input_dialog
//
// Description:
//
//
// Author: Juan Linietsky <reshaked@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef GUIINPUT_DIALOG_H
#define GUIINPUT_DIALOG_H

#include "bundles/spin_box.h"
#include "widgets/line_edit.h"
#include "widgets/label.h"
#include "widgets/button.h"
#include "base/window.h"
#include "bundles/window_box.h"
#include "bundles/margin_group.h"
#include "bundles/combo_box.h"


namespace GUI {

/**
	@author Juan Linietsky <reshaked@gmail.com>
*/


class NumericInputDialog : public Window {

	MarginGroup *mg;
	SpinBox *sb;

	void ok_pressed_text(String);
	void ok_pressed();
public:

	Signal< Method1<double> > entered_number_signal;

	void show(String p_question,double p_min,double p_max,double p_step,double p_default=0);

	NumericInputDialog(Window *p_parent);
};


class StringInputDialog : public Window {

	MarginGroup *mg;
	LineEdit *le;

	void ok_pressed_text(String);
	void ok_pressed();
public:

	Signal< Method1<String> > entered_string_signal;

	void show(String p_question,String p_current);

	StringInputDialog(Window *p_parent);
};

class QuestionInputDialog : public Window {

	WindowBox *vb;
	Label *l;
	HBoxContainer* button_hbox;

	void button_pressed(int p_but);
public:

	void clear();

	Button* add_button(int p_id, String p_text);
	void show(String p_question);
	WindowBox *get_vb() { return vb; }

	Signal< Method1<int> > button_pressed_signal;

	QuestionInputDialog(Window* p_parent);
};

class OptionInputDialog : public Window {

	MarginGroup *question;
	ComboBox *combo;

	void option_select_callback();
public:

	Signal< Method2<int,String> > option_selected_signal;

	void clear();
	void add_option(String p_option);
	void show(String p_question);

	OptionInputDialog(Window* p_parent);
};


}

#endif
