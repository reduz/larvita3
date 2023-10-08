//
// C++ Interface: file_dialog
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIFILE_DIALOG_H
#define PIGUIFILE_DIALOG_H


#include "widgets/list.h"
#include "base/window.h"
#include "widgets/button.h"
#include "bundles/combo_box.h"
#include "widgets/line_edit.h"
#include "widgets/scroll_bar.h"
#include "base/file_system.h"
#include "bundles/margin_group.h"

namespace GUI {

class WindowBox;
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class FileDialog : public Window {
public:

	enum FileMode {

		MODE_OPEN,
		MODE_OPEN_MULTI,
		MODE_OPEN_DIR,
		MODE_SAVE,
	};
private:


	WindowBox *wb;
	VBoxContainer *extra_vb;

	ComboBox *drive;
	LineEdit* path;
	List *file_list;
	MarginGroup *file_group;
	LineEdit* file;
	Button *action;
	MarginGroup *filter_group;
	ComboBox *filter_box;

	FileMode mode;


	void ok_pressed();
	void ok_pressed_s(String);
	void path_lineedit_changed(String);
	void list_enter_pressed();

	void file_selected_slot(int p_which);
	void open_slot(int p_which);
	FileSystem *fs;

	struct FilterList {

		String description;
		String filter;
		FilterList *next;

		FilterList() { next=0; }
	};

	FilterList *filter_list;

	void drive_changed(int p_to);
	void filter_type_changed(int p_to);
	void update_filter_list();
	bool updating_drive;

	bool own_filesystem;

protected:

	virtual bool ignore_filter();
	virtual void file_selected(String p_string); ///called when cursor is over a file, not when selecting it
public:
	void update_file_list();
	Signal< Method1<String> > file_on_cursor_signal;
	Signal< Method1<String> > file_activated_signal;
	Signal< Method1<String> > dir_selected_signal;
	Signal<> multi_file_activated_signal;

	/* for multi mode */
	int get_file_count();
	String get_path(int p_index); /// Return the full path file+dir
	String get_file(int p_index); /// Return the file+
	String get_dir();

	VBoxContainer *get_extra_vb() { return extra_vb; }

	WindowBox *get_window_box() { return wb; }
	
	void add_filter(String p_description,String p_extension);
	void clear_filter_list();

	FileMode get_mode();
	void show( FileMode p_mode );

	void set_dir(String p_dir);

	void set_incremental_search(bool p_enabled);
	void set_custom_filesystem(FileSystem *p_custom_filesystem,bool p_own_it=false);
	FileDialog(Window *p_parent);
	~FileDialog();

};

}

#endif
