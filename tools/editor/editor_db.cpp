//
// C++ Implementation: editor_db
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "editor_db.h"
#include "containers/center_container.h"
#include "editor_skin.h"
#include "iapi_db.h"
#include "iapi_persist.h"
#include "editor_main.h"


GUI::TreeItem *EditorDB::get_parent_node(String p_path,Table<String,GUI::TreeItem*>& item_paths,GUI::TreeItem *root,GUI::Tree *tree,GUI::Skin *p_skin,EditorDB *p_editor) {

	GUI::TreeItem *item=NULL;
	
	if (p_path=="") {
	
		item = root;
	} else if (item_paths.has(p_path)) {
	
		item = item_paths.get(p_path);		
	} else {
	
		//printf("path %s parent path %s - item name %s\n",p_path.ascii().get_data(),p_path.left( p_path.find_last("/") ).ascii().get_data(),p_path.right( p_path.find_last("/") ).ascii().get_data() );
		GUI::TreeItem *parent = get_parent_node( p_path.left( p_path.find_last("/") ),item_paths,root,tree,p_skin ,p_editor);
		item = tree->create_item( parent );
		item->selected_signal.connect( Method1<int>( Method2<int,GUI::TreeItem*>( p_editor, &EditorDB::set_current_path_callback),item ) );
		String name = (p_path.find("/")!=-1)?p_path.right( p_path.find_last("/") ):p_path;
		item->set_text(0, name );
		item->set_bitmap( 0, p_skin->get_bitmap( BITMAP_EDITOR_ICON_FOLDER ) );
		item_paths[p_path]=item;
	}
	
	return item;

}

void EditorDB::update_tree(String p_type) {

	tree->clear();
	
	IAPI_DB *db=IAPI_Persist::get_singleton()->get_db();
	if (!db) {
	
		GUI::TreeItem *it = tree->create_item(NULL);
		it->set_text(0,"No Database Configured.");
		return;
	}
	List<IAPI_DB::ListingElement> listing;
	
	db->get_listing(&listing,IAPI_DB::LISTING_TYPE,"");
	
	Table<String,GUI::TreeItem*> item_path;
	GUI::TreeItem *root = tree->create_item(NULL);
	root->selected_signal.connect( Method1<int>( Method2<int,GUI::TreeItem*>( this, &EditorDB::set_current_path_callback),root ) );
	
	root->set_text(0,"Database File System:");
	root->set_text(1," #Res. ID");
	
	/*
	root->set_bitmap(0,EditorMain::get_singleton()->get_editor_icons()->get( iapi->persist_as() ) );
	if (iapi->get_ID()==OBJ_INVALID_ID)
		root->set_text(1,"<built-in>");
	else
		root->set_text(1,"ID: "+String::num(iapi->get_ID()));
	*/
	
	
	for (List<IAPI_DB::ListingElement>::Iterator *I=listing.begin() ; I ; I=I->next()) {
	
		IAPI_DB::ListingElement &le=I->get();
		
		if (le.path.length() && le.path[0]=='/')
			le.path=le.path.right(0);
		String path=le.path.left( le.path.find_last("/") ) ;
		
		GUI::TreeItem * parent = get_parent_node(path,item_path,root,tree,get_window()->get_skin(),this );
		
		if (p_type!="" && le.type!=p_type)
			continue;
		
		GUI::TreeItem * item = tree->create_item( parent );
		item->selected_signal.connect( Method1<int>( Method2<int,GUI::TreeItem*>( this, &EditorDB::set_current_path_callback),item ) );
		
		String name = (le.path.find("/")!=-1)?le.path.right( le.path.find_last("/") ):le.path;
		
		printf("property type %s\n",le.type.ascii().get_data());
		
		item->set_text( 0, name );
		item->set_bitmap( 0, EditorMain::get_singleton()->get_editor_icons()->get( le.type ));
		item->set_text( 1, String::num(le.id) );
	}
		
		
		
}

void EditorDB::set_current_path_callback(int,GUI::TreeItem *p_item) {

	current_path=get_item_path(p_item);
	path->set_text(current_path);
}

String EditorDB::get_item_path(GUI::TreeItem *p_item) {

	if (!p_item->get_parent())
		return "/";
		
	String path = get_item_path(p_item->get_parent())+p_item->get_text(0);
	if (p_item->get_text(1)=="")
		path+="/"; // it's a directory
		
	return path;		
}

GUI::TreeItem *EditorDB::get_path_item(GUI::TreeItem * p_item,String p_current,String p_path) {

	
	if (!p_item->get_parent())
		p_current="/";
	else {
		p_current+=p_item->get_text(0);
		if (p_item->get_text(1)=="")
			p_current+="/"; // it's a directory		
	}
		
	if (p_current==p_path)
		return p_item;
		
	GUI::TreeItem *childs=p_item->get_childs();
	
	while(childs) {
	
		GUI::TreeItem * res = get_path_item(childs,p_current,p_path);
		if (res)
			return res;
			
		childs=childs->get_next();
	}
	
	return NULL;

}

GUI::TreeItem *EditorDB::get_path_item(String p_path) {

	ERR_FAIL_COND_V( ! tree->get_root_item(), NULL );
	return get_path_item( tree->get_root_item(),"",p_path );

}


void EditorDB::make_dir() {

	dir_name_input->show("Create Subdirectory","");
	
}

void EditorDB::activate(Mode p_mode,String p_file,String p_type) {

	current_path="/";
	path->set_text("/");

	if (p_mode==MODE_OPEN)
		action->set_text("Load");
	else
		action->set_text("Save");
		
	update_tree(p_type);
	mode=p_mode;
	
}

void EditorDB::save_file_callback(String p_path) {

	if ( p_path.find("/")!=-1 || p_path.find(".")==0) {
	
		message_box->show("Invalid File Name.");
		return;
	}
	
	String mkpath=current_path.left( current_path.find_last("/") )+"/";
	
	if ( get_path_item(mkpath+p_path+"/")) {
	
		message_box->show("Directory of the same name exists.");
		return;
	}
		
	activated_signal.call(mkpath+p_path);
	

}
void EditorDB::make_dir_callback(String p_dir) {


	if ( p_dir.find("/")!=-1 || p_dir.find(".")==0) {
	
		message_box->show("Invalid Directoty Name.");
		return;
	}
	

	String mkpath=current_path.left( current_path.find_last("/") )+"/";;
	
	if ( get_path_item(mkpath+p_dir+"/")) {
	
		message_box->show("Directory of the same name exists.");
		return;
	}
	if ( get_path_item(mkpath+p_dir)) {
	
		message_box->show("File of the same name exists.");
		return;
	}
	
	GUI::TreeItem *under = get_path_item(mkpath);
	
	ERR_FAIL_COND(!under);
	
	mkpath+=p_dir;

	printf("mkpath at %s, %p\n",mkpath.ascii().get_data(),under);
	
	GUI::TreeItem *item = tree->create_item( under );
	
	item->selected_signal.connect( Method1<int>( Method2<int,GUI::TreeItem*>( this, &EditorDB::set_current_path_callback),item ) );
	item->set_text(0, p_dir );
	item->set_bitmap( 0, bitmap( BITMAP_EDITOR_ICON_FOLDER ) );

}

void EditorDB::load_save_pressed() {

	
	String dir=current_path.left( current_path.find_last("/") )+"/";;
	String file=current_path.right( current_path.find_last("/") );

	switch(mode) {
	
		case MODE_OPEN: {
		
			if (file=="") {
				message_box->show("Picked a Directory.");
				break; // picked a dir
			}
			
			activated_signal.call(current_path);
		} break;
		case MODE_SAVE: {
		
			file_name_input->show("Enter file name to Save As:",file);
		} break;

	}
}

void EditorDB::set_in_window() {

	file_name_input = memnew( GUI::StringInputDialog( get_window() ) );
	file_name_input->entered_string_signal.connect( this, &EditorDB::save_file_callback );
	dir_name_input = memnew( GUI::StringInputDialog( get_window() ) );
	dir_name_input->entered_string_signal.connect( this, &EditorDB::make_dir_callback );
	message_box = memnew( GUI::MessageBox( get_window() ));
}

EditorDB::EditorDB() {
	
	GUI::HBoxContainer *hb = add( memnew( GUI::HBoxContainer ));
	path = hb->add( memnew( GUI::LineEdit("/")), 1 );
	path->set_editable(false);
	mkpath = hb->add( memnew( GUI::Button("Make Dir") ) );
	mkpath->pressed_signal.connect(this,&EditorDB::make_dir);
	
	GUI::ScrollBox* scroll = add( memnew( GUI::ScrollBox ),1 );
	scroll->set_expand_h(true);
	scroll->set_expand_v(true);
	tree = scroll->set( memnew( GUI::Tree(2)) );
	tree->set_column_min_width(1,80);
	tree->set_column_expand(1,false);
	
	action = add( memnew( GUI::CenterContainer) )->set( memnew( GUI::Button("OK") ) );
	action->pressed_signal.connect( this, &EditorDB::load_save_pressed );
	mode=MODE_OPEN;
	file_name_input=NULL;
	dir_name_input=NULL;
}


EditorDB::~EditorDB() {

	if (file_name_input)
		memdelete(file_name_input);
	if (dir_name_input)
		memdelete(dir_name_input);
	if (message_box)
		memdelete(message_box);
}


