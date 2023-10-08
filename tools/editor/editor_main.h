//
// C++ Interface: editor_main
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EDITOR_MAIN_H
#define EDITOR_MAIN_H

#include "base/window.h"
#include "editor/editor_viewport.h"
#include "editor/editor_scene_tree.h"
#include "editor/editor_iapi.h"
#include "editor/editor_iapi_icons.h"
#include "editor/editor_db.h"
#include "containers/stack_container.h"
#include "vector.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class EditorMain : public GUI::Window {
	

	int history_idx;
	Vector<IAPIRef> history_stack;
	void rebuild_history_buttons();
	void navigate_history_stack(int p_where);
	void navigate_back();
	void navigate_forward();
	void goto_iapi();
	void goto_scene_tree();
	GUI::Button *go_back_btn;
	GUI::Button *go_forward_btn;
	
	

	EditorSceneTree *editor_scene_tree;
	EditorIAPI *editor_iapi;
	EditorViewport *editor_viewport;
	EditorDB *editor_db;
	GUI::StackContainer *stack;
	
	bool quit;
	
	void quit_request_callback();
	
	static EditorMain *singleton;
	
	EditorIAPIIcons *editor_icons;
	
	void request_iapi_edit(IAPIRef p_iapi); 
public:
	
	EditorViewport *get_viewport();
	
	EditorIAPIIcons *get_editor_icons();
	
	static EditorMain *get_singleton();
	
	bool quit_request();
	
	EditorMain(GUI::Painter *p_painter,GUI::Timer *p_timer,GUI::Skin *p_skin);
	~EditorMain();

};

#endif
