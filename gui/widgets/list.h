//
// C++ Interface: list
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUILIST_H
#define PIGUILIST_H

#include "widgets/range_owner.h"

#include "base/window.h"
#include "widgets/line_edit.h"

namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/


/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class List : public RangeOwner {
	
	/* Using a Linked List for now, Memory "staticness" is a priority over speed,
	   May change to a table or something */
	
	enum {
		
		INCREMENTAL_SEARCH_TIMEOUT=1500 /* incremental search timeout, in msecs */
	};
	
	
	struct Element {
		
		String text;
		bool selected;
		
		Element *next;		
		
		Element() { selected=false; next=0; }
	};

	int cursor;
	int offset;
	Element *list;
	bool allow_multiple;
	Size size;
	bool editable;
	bool cursor_hint;
	int mouse_over_item;
	int number_elements;
	int number_min_integers;
	int max_string_len;

	Window *editor_window;
	LineEdit *editor;
	int editor_pos;
	
	/* timer for incremental search */
	bool isearch_enabled;
	TimerID isearch_timer;
	String isearch_word;
	
	virtual Size get_minimum_size_internal();
	virtual void draw(const Point& p_global,const Size& p_size,const Rect& p_exposed);
	virtual void mouse_button(const Point& p_pos, int p_button,bool p_press,int p_modifier_mask);
	virtual void mouse_doubleclick(const Point& p_pos, int p_modifier_mask);
	virtual void mouse_motion(const Point& p_pos, const Point& p_rel, int p_button_mask);
	virtual bool key(unsigned long p_unicode, unsigned long p_scan_code,bool p_press,bool p_repeat,int p_modifier_mask);
	virtual void resize(const Size& p_new_size);
	
	int get_line_size();
	int get_visible_lines();

	void set_offset(int p_offset);

	virtual void range_ptr_changed();
	virtual void value_changed(double p_new_val);


	void create_editor(int p_for_element);
	void editor_string_changed(String p_string);

	void set_in_window();
	
	void isearch_timeout_slot();
	
	void ensure_cursor_visible();
public:	

	
	virtual String get_type();
	

	
	int get_min_total_height(); //helper for combobox and menus..
	
	Signal< Method1<int> > cursor_changed_signal; //for multiselect lists
	Signal< Method1<int> > selected_signal;
	Signal< Method1<int> > activated_signal;
	Signal< Method2<int,String> > item_edited_signal;
	Signal<> selection_changed_signal;
	Signal<> enter_pressed;
			/* For multiple selections */

	int get_size(); ///< return amount of elements in list
	int get_selection_size(); ///< for multiple selections, amount of items selected
	int get_selected(int p_which=0); ///< Index of number of selected item, or default for the first
	String get_selected_string(int p_which=0); ///< Index of number of selected item, or default for the first
	
	int find_string_index(String p_string);
	void add_string(const String& p_str,int p_at=-1); ///< Add a string to the list at pos p_at, or end if p_at==-1
	void add_sorted_string(const String& p_str); ///< Add a string to the list in sorted order
	String get_string(int p_at);  
	void set_string(const String& p_str,int p_at); ///< Set string to the list at pos p_at, or end if
	
	void erase(int p_at); ///< Erase element at p_at
	
	void select(int p_at,bool p_exclusive=false); ///< Select an item , exclusive if multi mode is enabled or flag provided
	bool is_selected(int p_at);
	void unselect(int p_at=-1); ///< unselect an item, or all 
	
	void set_multi_mode(bool p_enable); ///< Enable multi mode (selection of multiple items)
	void set_cursor_hint(bool p_cursor_hint);
	void set_editable(bool p_editable);
	void set_cursor_pos(int p_pos);
	
	void clear(); ///< Clear the list

	void set_number_elements(int p_num); 
	void set_number_min_integers(int p_num);

	void set_max_string_length(int p_length);
	int get_max_string_length();
	
	void set_incremental_search(bool p_enabled);
	
	List();
	~List();

};

}

#endif
