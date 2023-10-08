//
// C++ Interface: popup_menu
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIPOPUP_MENU_H
#define PIGUIPOPUP_MENU_H

#include "base/window.h"
#include "widgets/menu_button.h"

namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class VBoxContainer;

class PopUpMenu : public Window {
	
	VBoxContainer *vbc;
	VBoxContainer *main_vbc;
	int id_count;
	
	void menu_button_selected(MenuButton*);
	void menu_button_toggled(MenuButton*,bool p_toggle);
	
	virtual void skin_changed();
	
public:
	
	Signal< Method2<int,void*> > selected_id_userdata_signal; ///< Selected, return ID and userdata, for all items
	Signal< Method3<int,bool,void*> > checked_id_userdata_signal; ///< Checked, return ID, status and userdata, for checkable items
	Signal< Method1<int> > selected_id_signal; ///< Selected, return ID , for all items
	Signal< Method2<int,bool> > checked_id_signal; ///< Checked, return ID, and status, for all
	Signal<> pre_show_signal; ///< Pre-show signal, called before popping up (becoming visible)
	
	
	
	void add_item(String p_text, int p_ID, void *p_userdata,int p_shortcut,bool p_shortcut_active); 
	void add_item(String p_text, void *p_userdata,int p_shortcut,bool p_shortcut_active);
	void add_item(BitmapID p_icon, String p_text, int p_ID, void *p_userdata,int p_shortcut,bool p_shortcut_active); 
	void add_item(BitmapID p_icon, String p_text, void *p_userdata,int p_shortcut,bool p_shortcut_active);
			
	void add_item(String p_text, const Method& p_method,int p_shortcut,bool p_shortcut_active); 
	void add_item(BitmapID p_icon, String p_text, const Method& p_method,int p_shortcut,bool p_shortcut_active);
	
	void add_item(String p_text, int p_ID, void *p_userdata=0); 
	void add_item(String p_text, void *p_userdata=0);
	void add_item(BitmapID p_icon, String p_text, int p_ID, void *p_userdata=0); 
	void add_item(BitmapID p_icon, String p_text, void *p_userdata=0);
			
	void add_item(String p_text, const Method& p_method); 
	void add_item(BitmapID p_icon, String p_text, const Method& p_method);
	
	
	void add_check_item(String p_text, int p_ID, bool p_checked=false,void *p_userdata=0);
	void add_check_item(String p_text, bool p_checked=false,void *p_userdata=0);
	void add_check_item(BitmapID p_icon, String p_text, int p_ID, bool p_checked=false,void *p_userdata=0);
	void add_check_item(BitmapID p_icon, String p_text,bool p_checked=false,void *p_userdata=0);

	void add_check_item(String p_text, const Method1<bool>& p_method, bool p_checked=false); 
	void add_check_item(BitmapID p_icon, String p_text, const Method1<bool>& p_method, bool p_checked=false);
	
	void add_separator();
	
	void clear(); ///< Clear popup (all options)			
	
	void popup(); ///< Popup at preset pos of parent window (use set_pos/set_size, then this )
	void popup(const Point &p_pos); ///< Popup at given position
			
	bool is_item_checked(int p_ID);
	String get_item_text(int p_ID);
	BitmapID get_item_icon(int p_ID);
	void* get_item_userdata(int p_ID);
	bool has_ID(int p_ID);
	
	PopUpMenu(Window *p_parent);
	~PopUpMenu();
};

}

#endif
