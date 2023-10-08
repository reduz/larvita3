//
// C++ Implementation: editor_scene_tree
//
// Description:
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "editor_scene_tree.h"

#include "editor_skin.h"
#include "widgets/separator.h"
#include "widgets/menu_button.h"
#include "scene/main/scene_main_loop.h"
#include "editor/editor_main.h"

#include "scene/nodes/animation_node.h"
#include "scene/nodes/particle_system_node.h"


#ifdef COLLADA_ENABLED
#include "FCollada/collada.h"
#endif


void EditorSceneTree::node_selected_callback(int p_col,WeakRef<Node> p_selected_node) {

	selected_node=p_selected_node;
}

void EditorSceneTree::node_edit_callback(int p_col,WeakRef<Node> p_selected_node) {

	node_edit_signal.call( p_selected_node.get_iref() );
}

void EditorSceneTree::populate_tree( IRef<Node> p_node, GUI::TreeItem *p_parent ) {

	GUI::TreeItem *item=tree->create_item(p_parent);

	item->set_cell_mode(0,GUI::CELL_MODE_STRING);
	item->set_text(0, p_node->get_name() );

	WeakRef<Node> noderef = p_node;

	item->selected_signal.connect( Method1<int>( Method2< int,WeakRef<Node> >( this, &EditorSceneTree::node_selected_callback ),noderef) );
	item->double_click_signal.connect( Method1<int>( Method2< int,WeakRef<Node> >( this, &EditorSceneTree::node_edit_callback ),noderef) );

	GUI::BitmapID bitmap = EditorMain::get_singleton()->get_editor_icons()->get( p_node->get_type() );
	item->set_bitmap(0, bitmap );
	for (int i=0;i<p_node->get_child_count();i++) {

		IRef<Node> child = p_node->get_child_by_index(i);
		populate_tree( child, item );
	}
}


void EditorSceneTree::update_scene_tree() {

	if (SceneMainLoop::get_singleton() && first_update) {

		SceneMainLoop::get_singleton()->node_enter_scene_signal.connect(this,&EditorSceneTree::scene_changed);
		SceneMainLoop::get_singleton()->node_exit_scene_signal.connect(this,&EditorSceneTree::scene_changed);
		first_update=false;

	}

	IRef<Node> root = SceneMainLoop::get_singleton()->get_scene_tree();
	if (root.is_null())
		return;


	tree->clear();
	populate_tree( root, NULL );



}

void EditorSceneTree::scene_changed(IRef<Node>) {

	if (!get_window())
		return;
	get_window()->get_root()->update_signal.connect( this, &EditorSceneTree::update_scene_tree );
}

void EditorSceneTree::file_dialog_callback(String p_path) {

	switch(current_option) {

		case OPTION_FILE_IMPORT :{
			#ifdef COLLADA_ENABLED
			if (p_path.findn(".dae")==-1)
				break;
			IRef<SceneNode> scene = Collada::import_scene(p_path);

			if (scene.is_null()) {

				message->show("Error Importing Collada");
				break;
			}
			SceneMainLoop::get_singleton()->set_scene_tree( scene );

			update_scene_tree();
			#endif
		} break;
		default: {};
	}
}

void EditorSceneTree::menu_callback(int p_option) {

	current_option=(MenuOption)p_option;
	printf("option is %i\n",p_option);

	switch (current_option) {
		case OPTION_FILE_NEW: {

			SceneMainLoop::get_singleton()->set_scene_tree( IRef<SceneNode>() );
		} break;
		case OPTION_FILE_OPEN: {

		} break;
		case OPTION_FILE_SAVE: {

		} break;
		case OPTION_FILE_SAVE_AS: {

		} break;
		case OPTION_FILE_IMPORT: {

			file_dialog->clear_filter_list();
			file_dialog->add_filter("Collada",".dae");
			file_dialog->show( GUI::FileDialog::MODE_OPEN );
		} break;
		case OPTION_FILE_QUIT: {

			quit_signal.call();
		} break;
		case CREATE_DUMMY: {

		} break;
		case CREATE_MESH: {

		} break;
		case CREATE_SPLINE: {

		} break;
		case CREATE_CAMERA: {

		} break;
		case CREATE_LIGHT: {

		} break;
		case CREATE_COLLISION: {

		} break;
		case CREATE_RIGID_BODY: {

		} break;
		case CREATE_INFLUENCE_ZONE: {

		} break;
		case CREATE_PARTICLE_SYSTEM: {

			IRef<ParticleSystemNode> ps;
			ps.create();
			add_node_to_scene(ps);

		} break;
		case CREATE_PORTAL: {

		} break;
		case CREATE_ROOM: {

		} break;
		case CREATE_SKELETON: {

		} break;
		case CREATE_ANIMATION_PLAYER: {

			IRef<AnimationNode> an;
			an.create();
			add_node_to_scene(an);
		} break;
		case CREATE_SCENE: {

		} break;
		case CREATE_SCENE_INSTANCE: {

		} break;
		default: {};
	};

}


void EditorSceneTree::add_node_to_scene(IRef<Node> p_node) {

	IRef<SceneNode> scene_tree = SceneMainLoop::get_singleton()->get_scene_tree();

	if (scene_tree.is_null())
		return;

	scene_tree->add_child(p_node);

}

void EditorSceneTree::set_in_window() {

	add( memnew( GUI::Label("Scene Editor", GUI::Label::ALIGN_CENTER) ) );

	GUI::HBoxContainer *hbc_menus = add( memnew( GUI::HBoxContainer ) );
	GUI::MenuBox *file_menu = hbc_menus->add( memnew( GUI::MenuBox("", bitmap( BITMAP_EDITOR_ICON_FILE ) ) ) );

	file_menu->add_item("New Scene",OPTION_FILE_NEW);
	file_menu->add_item("Open",OPTION_FILE_OPEN);
	file_menu->add_item("Save",OPTION_FILE_SAVE);
	file_menu->add_item("Save As",OPTION_FILE_SAVE_AS);
	file_menu->add_separator();
	file_menu->add_item("Import",OPTION_FILE_IMPORT);
	file_menu->add_separator();
	file_menu->add_item("Quit",OPTION_FILE_QUIT);
	file_menu->item_selected_signal.connect( this, &EditorSceneTree::menu_callback );

	GUI::MenuBox *edit_menu = hbc_menus->add( memnew( GUI::MenuBox("", bitmap( BITMAP_EDITOR_ICON_EDIT ) ) ) );

	edit_menu->add_item("Select All",EDIT_SELECT_ALL);
	edit_menu->add_item("Deselect All",EDIT_DESELECT_ALL);
	edit_menu->add_item("Duplicate Selected",EDIT_DUPLICATE_SELECTED);
	edit_menu->add_item("Delete Selected",EDIT_DELETE_SELECTED);
	edit_menu->add_separator();
	edit_menu->add_item("Exact Transform",EDIT_EXACT_TRANSFORM);
	edit_menu->add_separator();
	edit_menu->add_item("Reparent",EDIT_REPARENT);
	edit_menu->item_selected_signal.connect( this, &EditorSceneTree::menu_callback );


	GUI::MenuBox *view_menu = hbc_menus->add( memnew( GUI::MenuBox("", bitmap( BITMAP_EDITOR_ICON_VIEW ) ) ) );

	hbc_menus->add( memnew( GUI::Widget) , 1 ); // spacer


	GUI::MenuBox *create_menu = hbc_menus->add( memnew( GUI::MenuBox("", bitmap( BITMAP_EDITOR_ICON_ADD ) ) ) );
	create_menu->add_item(bitmap( BITMAP_EDITOR_ICON_DUMMY ),"Dummy", CREATE_DUMMY);
	create_menu->add_item(bitmap( BITMAP_EDITOR_ICON_MESH ),"Mesh", CREATE_MESH);
	create_menu->add_item(bitmap( BITMAP_EDITOR_ICON_PARTICLES ),"Particles", CREATE_PARTICLE_SYSTEM);
	create_menu->add_item(bitmap( BITMAP_EDITOR_ICON_SPLINE ),"Spline", CREATE_SPLINE);
	create_menu->add_item(bitmap( BITMAP_EDITOR_ICON_LIGHT ),"Light", CREATE_LIGHT);
	create_menu->add_item(bitmap( BITMAP_EDITOR_ICON_CAMERA ),"Camera", CREATE_CAMERA);
	create_menu->add_item(bitmap( BITMAP_EDITOR_ICON_COLLISION ),"Collision (Static)", CREATE_COLLISION);
	create_menu->add_item(bitmap( BITMAP_EDITOR_ICON_RIGID_BODY ),"Rigid Body", CREATE_RIGID_BODY);
	create_menu->add_item(bitmap( BITMAP_EDITOR_ICON_INFLUENCE_ZONE ),"Influence Zone", CREATE_INFLUENCE_ZONE);
	create_menu->add_item(bitmap( BITMAP_EDITOR_ICON_SKELETON ),"Skeleton", CREATE_SKELETON);
	create_menu->add_item(bitmap( BITMAP_EDITOR_ICON_ANIMATION_NODE ),"Animation Player", CREATE_ANIMATION_PLAYER);
	create_menu->add_item(bitmap( BITMAP_EDITOR_ICON_PORTAL ),"Portal", CREATE_PORTAL);
	create_menu->add_item(bitmap( BITMAP_EDITOR_ICON_ROOM ),"Room", CREATE_ROOM);
	create_menu->add_item(bitmap( BITMAP_EDITOR_ICON_SCENE ),"Scene", CREATE_SCENE);
	create_menu->add_item(bitmap( BITMAP_EDITOR_ICON_SCENE_INSTANCE ),"Scene Instance", CREATE_SCENE_INSTANCE);
	create_menu->item_selected_signal.connect( this, &EditorSceneTree::menu_callback );

	GUI::Button *del_button = hbc_menus->add( memnew( GUI::MenuButton(bitmap( BITMAP_EDITOR_ICON_DEL ) ) ) );

	file_dialog = memnew( GUI::FileDialog(get_window() ) );
	file_dialog->file_activated_signal.connect( this, &EditorSceneTree::file_dialog_callback );
	message = memnew( GUI::MessageBox(get_window()));

	scroll = add( memnew( GUI::ScrollBox ),1 );
	scroll->set_expand_h(true);
	scroll->set_expand_v(true);
	tree = scroll->set( memnew( GUI::Tree(1)) );

}


EditorSceneTree::EditorSceneTree() {


	first_update=true;
}


EditorSceneTree::~EditorSceneTree()
{
}


