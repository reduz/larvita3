//
// C++ Interface: editor_scene_tree
//
// Description:
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EDITOR_SCENE_TREE_H
#define EDITOR_SCENE_TREE_H

#include "widgets/tree.h"
#include "containers/box_container.h"
#include "bundles/menu_box.h"
#include "bundles/file_dialog.h"
#include "bundles/message_box.h"
#include "bundles/scroll_box.h"
#include "scene/nodes/node.h"
/**
	@author ,,, <red@lunatea>
*/
class EditorSceneTree : public GUI::VBoxContainer {

	enum MenuOption {

		OPTION_FILE_NEW,
		OPTION_FILE_OPEN,
		OPTION_FILE_SAVE,
		OPTION_FILE_SAVE_AS,
		OPTION_FILE_IMPORT,
		OPTION_FILE_QUIT,
		EDIT_SELECT_ALL,
		EDIT_DESELECT_ALL,
		EDIT_DUPLICATE_SELECTED,
		EDIT_DELETE_SELECTED,
		EDIT_REPARENT,
		EDIT_EXACT_TRANSFORM,
		CREATE_DUMMY,
		CREATE_MESH,
		CREATE_SPLINE,
		CREATE_CAMERA,
		CREATE_LIGHT,
		CREATE_COLLISION,
		CREATE_RIGID_BODY,
		CREATE_INFLUENCE_ZONE,
		CREATE_PARTICLE_SYSTEM,
		CREATE_PORTAL,
		CREATE_ROOM,
		CREATE_SKELETON,
		CREATE_ANIMATION_PLAYER,
		CREATE_SCENE,
		CREATE_SCENE_INSTANCE,
	};


	GUI::ScrollBox *scroll;
	GUI::Tree *tree;

	GUI::MessageBox *message;
	MenuOption current_option;

	GUI::FileDialog *file_dialog;

	void set_in_window();
	void menu_callback(int);
	void file_dialog_callback(String p_path);

	void populate_tree( IRef<Node> p_node, GUI::TreeItem *p_parent );


	WeakRef<Node> selected_node;

	void add_node_to_scene(IRef<Node> p_node);

	void node_selected_callback(int p_col,WeakRef<Node> p_selected_node);
	void node_edit_callback(int p_col,WeakRef<Node> p_selected_node);

	void scene_changed(IRef<Node>);

	bool first_update;
public:

	Signal< Method1<IAPIRef> > node_edit_signal;

	void update_scene_tree();

	Signal<> quit_signal;

	EditorSceneTree();
	~EditorSceneTree();

};

#endif
