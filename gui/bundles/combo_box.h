//
// C++ Interface: combo_box
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUICOMBO_BOX_H
#define PIGUICOMBO_BOX_H


#include "containers/box_container.h"


namespace GUI {

class List;
class ArrowButton;
class LineEdit;

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class ComboBox : public HBoxContainer {
public:	
	
	enum Mode {
		
		MODE_SELECT, ///< Select items from the list
		MODE_EDIT, ///< Edit items in the list (edit selected one), when pressing enter
		MODE_ADD, ///< Append item to list, when pressing enter
		MODE_HINTER, ///< Line edit is editable, but does not edit/append items on the list on enter
	};
	
	
private:	
	
	class HBCCombo : public HBoxContainer{
	public:
		virtual const StyleBox & stylebox(int p_which);		
	};
	
	HBCCombo *hbc;
	Mode mode;
	LineEdit *line_edit;
	ArrowButton *arrow_button;
	Size size;
	
	bool setting_line_edit;
	/* popup */
	
	Window *popup;
	List *list;

	void set_in_window();
	void button_clicked();
	virtual void resize(const Size& p_new_size);	

	void list_selection_changed_slot(int p_which);
	void line_edit_enter_pressed(String p_string);
	
	virtual void skin_changed();
	
public:


	Signal< Method1<int> > selected_signal; /// Something was selected
	
	LineEdit *get_line_edit();
	
	int get_size(); ///< return amount of elements in list
	int get_selected(); ///< Index of number of selected item, or default for the first
	String get_string(); ///< Index of number of selected item, or default for the first
	
	int find_string_index(String p_string);
	
	void add_string(const String& p_str,int p_at=-1); ///< Add a string to the list at pos p_at, or end if p_at==-1
	void add_sorted_string(const String& p_str); ///< Add a string to the list in sorted order
	
	String get_string(int p_at);
	void set_string(const String& p_str,int p_at); ///< Set string to the list at pos p_at, or end if
	
	void erase(int p_at);
	
	void select(int p_at); 

	void clear(); ///< Clear the list
	
	ComboBox(Mode p_mode=MODE_SELECT);
	~ComboBox();

};

}

#endif
