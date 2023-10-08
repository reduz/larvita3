//
// C++ Implementation: editor_main
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "editor_main.h"
#include "containers/box_container.h"
#include "widgets/menu_button.h"
#include "widgets/separator.h"
#include "bundles/menu_box.h"
#include "editor_skin.h"

EditorMain *EditorMain::singleton=NULL;

EditorMain *EditorMain::get_singleton() {

	return singleton;
}

void EditorMain::quit_request_callback() {
	
	quit=true;	
}

bool EditorMain::quit_request() {
	
	return quit;	
}

EditorIAPIIcons *EditorMain::get_editor_icons() {

	return editor_icons;
}

void EditorMain::navigate_back() {

	navigate_history_stack(history_idx-1);
}
void EditorMain::navigate_forward() {

	navigate_history_stack(history_idx+1);

}

void EditorMain::goto_iapi() {

	history_idx=0;
	history_stack.clear();
	history_stack.push_back( IAPIRef() );
	stack->raise_frame( editor_iapi );
	editor_iapi->set_iapi( IAPIRef() );
	rebuild_history_buttons();
	
}
void EditorMain::goto_scene_tree() {

	history_idx=0;
	history_stack.clear();
	history_stack.push_back( IAPIRef() );
	navigate_history_stack(0);
}

void EditorMain::navigate_history_stack(int p_where) {

	if (p_where<0 || p_where>=history_stack.size())
		return;
		
	history_idx = p_where;
	IAPIRef iapi= history_stack.get( history_idx );
	
	
	if (iapi.is_null()) { // null iapi, goto scene tree
	
		stack->raise_frame( editor_scene_tree );
		
	} else {
	
		IAPIRef iapi = history_stack.get(p_where);
		stack->raise_frame( editor_iapi );
		editor_iapi->set_iapi( iapi );
	}
	
	rebuild_history_buttons();
}

void EditorMain::request_iapi_edit(IAPIRef p_iapi) {

	stack->raise_frame(editor_iapi);
	editor_iapi->set_iapi(p_iapi);
	history_stack.resize( history_idx + 1 );
	history_stack.push_back(p_iapi);
	history_idx++;
	rebuild_history_buttons();
}


void EditorMain::rebuild_history_buttons() {


	go_back_btn->set_icon( get_skin()->get_bitmap( (history_idx==0)?BITMAP_EDITOR_ICON_BACK_NO:BITMAP_EDITOR_ICON_BACK ));
	go_forward_btn->set_icon( get_skin()->get_bitmap( (history_idx==history_stack.size()-1)?BITMAP_EDITOR_ICON_FORWARD_NO:BITMAP_EDITOR_ICON_FORWARD ));
			
}

EditorViewport *EditorMain::get_viewport() {
	
	return editor_viewport;	
}

EditorMain::EditorMain(GUI::Painter *p_painter,GUI::Timer *p_timer,GUI::Skin *p_skin) :GUI::Window(p_painter,p_timer,p_skin) {
	
	singleton=this;
	
	set_no_local_updates(true);

	
	GUI::HBoxContainer *hbc = memnew( GUI::HBoxContainer );
	set_root_frame( hbc );
	p_skin->set_stylebox( GUI::SB_ROOT_CONTAINER, GUI::StyleBox() );
		
	editor_viewport = hbc->add( memnew( EditorViewport ),5 );
	
		
	GUI::VBoxContainer * vbc_editor_controls = hbc->add( memnew( GUI::VBoxContainer) ,2 );
	vbc_editor_controls->set_stylebox_override( p_skin->get_stylebox( GUI::SB_POPUP_BG ) );
	
	GUI::HBoxContainer *ctrl_hb = vbc_editor_controls->add( memnew( GUI::HBoxContainer ) );
	
	GUI::Button *goto_scene_btn = ctrl_hb->add( memnew( GUI::MenuButton(  p_skin->get_bitmap( BITMAP_EDITOR_ICON_TREE ) ) ) );
	goto_scene_btn->pressed_signal.connect(this,&EditorMain::goto_scene_tree);
	GUI::Button *goto_iapi_btn = ctrl_hb->add( memnew( GUI::MenuButton(  p_skin->get_bitmap( BITMAP_EDITOR_ICON_IAPI ) ) ) );
	goto_iapi_btn->pressed_signal.connect(this,&EditorMain::goto_iapi);
	
	ctrl_hb->add( memnew( GUI::Widget ),1 );	

	go_back_btn = ctrl_hb->add( memnew( GUI::MenuButton(  p_skin->get_bitmap( BITMAP_EDITOR_ICON_BACK_NO ) ) ) );
	go_back_btn->pressed_signal.connect(this,&EditorMain::navigate_back);
	go_forward_btn = ctrl_hb->add( memnew( GUI::MenuButton(  p_skin->get_bitmap( BITMAP_EDITOR_ICON_FORWARD_NO ) ) ) );
	go_forward_btn->pressed_signal.connect(this,&EditorMain::navigate_forward);
	
	history_stack.push_back(IAPIRef());
	history_idx=0;
	
	vbc_editor_controls->add( memnew( GUI::HSeparator ) );
	stack = vbc_editor_controls->add( memnew( GUI::StackContainer ),1);
	
	editor_scene_tree = stack->add( memnew( EditorSceneTree ) );
	editor_scene_tree->quit_signal.connect(this, &EditorMain::quit_request_callback );
	editor_scene_tree->node_edit_signal.connect( this, &EditorMain::request_iapi_edit );
	
	editor_iapi = stack->add( memnew( EditorIAPI ) );
	stack->set_minimum_size( GUI::Size(250,0) );
	editor_iapi->edit_iapi_signal.connect( this, &EditorMain::request_iapi_edit );
	quit=false;
	editor_db = stack->add( memnew( EditorDB ) );

	editor_icons = memnew( EditorIAPIIcons(p_skin) );
	
	
}


EditorMain::~EditorMain()
{

	memdelete(editor_icons);
}


