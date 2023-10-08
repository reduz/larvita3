//
// C++ Interface: text_box
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUILINE_EDIT_H
#define PIGUILINE_EDIT_H

#include "base/widget.h"


namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class LineEdit : public Widget {

	Size size;
	bool editable;
	bool pass;

	String undo_text;
	String text;

	int cursor_pos;
	int window_pos;
	int max_length; // 0 for no maximum

	struct Selection {

		int begin;
		int end;
		int cursor_start;
		bool enabled;
		bool creating;
		bool old_shift;
		bool doubleclick;
	} selection;

	void shift_selection_check_pre(bool);
	void shift_selection_check_post(bool);

	void selection_clear();
	void selection_fill_at_cursor();
	void selection_delete();
	void set_window_pos(int p_pos);

	void set_cursor_at_pixel_pos(int p_x);

	void clear_internal();
	void changed_internal();

	void copy_text();
	void cut_text();
	void paste_text();

	void mouse_button(const Point& p_pos, int p_button,bool p_press,int p_modifier_mask);
	void mouse_motion(const Point& p_pos, const Point& p_rel, int p_button_mask);
	bool key(unsigned long p_unicode, unsigned long p_scan_code,bool p_press,bool p_repeat,int p_modifier_mask);
	void draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed);
	void resize(const Size& p_new_size);
	Size get_minimum_size_internal();

	void window_hid() { lost_focus_signal.call(); }
	void focus_leave() { lost_focus_signal.call(); }
public:


	Signal< Method1< String > > text_changed_signal;
	Signal< Method1< String > > text_enter_signal;
	Signal<> lost_focus_signal;
	Signal<> non_editable_clicked_signal;
	

	void select_all();

	void delete_char();
	void set_text(String p_text);
	String get_text();
	void set_cursor_pos(int p_pos);
	int get_cursor_pos();
	void set_max_length(int p_max_length);
	void append_at_cursor(String p_text);
	void clear();

	void set_editable(bool p_editable);

	LineEdit(String p_text="", bool p_pass=false);
	~LineEdit();

};

}

#endif
