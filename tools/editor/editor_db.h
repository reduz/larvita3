//
// C++ Interface: editor_db
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EDITOR_DB_H
#define EDITOR_DB_H

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

#include "widgets/tree.h"
#include "containers/box_container.h"
#include "bundles/menu_box.h"
#include "bundles/file_dialog.h"
#include "bundles/message_box.h"
#include "bundles/input_dialog.h"
#include "bundles/scroll_box.h"
#include "scene/nodes/node.h"
#include "editor/editor_variant.h"

/**
	@author ,,, <red@lunatea>
 */
class EditorDB  : public GUI::VBoxContainer {
public:
	
	enum Mode {
		MODE_OPEN,
		MODE_SAVE		
	};
	
private:


	GUI::TreeItem *get_parent_node(String p_path,Table<String,GUI::TreeItem*>& item_paths,GUI::TreeItem *root,GUI::Tree *tree,GUI::Skin *p_skin,EditorDB *p_editor);

	Mode mode;
	void update_tree(String p_type);
	GUI::Button *action;
	GUI::Button *mkpath;
	GUI::LineEdit *path;
	GUI::Tree *tree;
	
	String current_path;
	String get_item_path(GUI::TreeItem *p_item);
	
	GUI::TreeItem *get_path_item(GUI::TreeItem * p_item,String p_current,String p_path);
	GUI::TreeItem *get_path_item(String p_path);
	
	GUI::StringInputDialog *file_name_input;
	GUI::StringInputDialog *dir_name_input;
	GUI::MessageBox *message_box;
	
	void set_current_path_callback(int,GUI::TreeItem *p_item);
	void make_dir();
	
	void save_file_callback(String p_path);
	void make_dir_callback(String p_dir);
	
	void load_save_pressed();
	
	void set_in_window();
	
public:
	
	Mode get_mode() const { return mode; }
	
	
	Signal< Method1< String > > activated_signal;	
	
	void activate(Mode p_mode,String p_file,String p_type);
	EditorDB();
	~EditorDB();
};

#endif
