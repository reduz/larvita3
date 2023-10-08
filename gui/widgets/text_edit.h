
#ifndef GUI_TEXT_AREA_H
#define GUI_TEXT_AREA_H

#include <vector>
#include "widgets/range_owner.h"

namespace GUI {

class TextEdit : public RangeOwner  { // tolua_export


	struct Pos {
		int last_fit_x;
		int x,y;	    ///< cursor
		int x_ofs,row_ofs;
	} cursor;


	int max_chars;
	bool readonly;
	Size size;
	std::vector<String> text; //needs to be removed
	bool setting_row;
	bool wrap;

	int get_visible_rows();

	int get_char_count();

	int get_char_pos_for(int p_px,String p_pos);
	int get_pixel_pos_for(int p_char,String p_pos);
	void new_text_at_cursor(String p_char);
	void newline_at_cursor();
	void backspace_at_cursor();
	void delete_at_cursor();

	void reconfig_sizes();
	void adjust_viewport_to_cursor();
	void value_changed(double);

	void mouse_button(const Point& p_pos, int p_button,bool p_press,int p_modifier_mask);
//	void mouse_motion(const Point& p_pos, const Point& p_rel, int p_button_mask);
	bool key(unsigned long p_unicode, unsigned long p_scan_code,bool p_press,bool p_repeat,int p_modifier_mask);
	void draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed);
	void resize(const Size& p_new_size);
	Size get_minimum_size_internal();

	int get_row_height();
public:

	Signal<> text_changed_signal;
	void delete_char();
	void delete_line();

	void set_text(String p_text);
	String get_text();
	String get_line(int line);

	void set_line_spacing(int p_spacing) {};
	int get_line_spacing() {return int();}; // this'll be fun

	void set_col(int p_col);
	void set_row(int p_row);

	int get_col();
	int get_row();

	void set_readonly(bool p_readonly);

	void set_max_chars(int p_max_chars);
	void set_wrap(bool p_wrap);

	void append_at_cursor(String p_text);
	void clear();

	TextEdit();
	~TextEdit();
};


}

#endif
