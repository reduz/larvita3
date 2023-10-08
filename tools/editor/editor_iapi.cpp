//
// C++ Implementation: editor_iapi
//
// Description:
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "editor_iapi.h"
#include "editor_skin.h"
#include "editor_main.h"
#include "iapi_persist.h"
#include "editor_creation_params.h"

void EditorIAPI::EditPopup::variant_changed(const Variant& p_variant) {

	ERR_FAIL_COND( iapi.is_null() );
	iapi->set(path,p_variant);
	switch (p_variant.get_type()) {

		case Variant::STRING:
		case Variant::VECTOR3:
		case Variant::QUAT:
		case Variant::PLANE:
		case Variant::COLOR:{

			item->set_text( 1, p_variant.get_string() );
		} break;
		case Variant::IOBJ: {

			if (p_variant.get_IOBJ().is_null()) {

				item->set_text( 1, "<null>" );
			} else {

				item->set_text( 1, "<"+p_variant.get_IOBJ()->get_type()+">");

			}
		} break;

		default: {}
	}

}


void EditorIAPI::EditPopup::edit(IAPIRef p_iapi,String p_path,GUI::TreeItem *p_item,GUI::Rect p_rect) {

	iapi=p_iapi;
	path=p_path;
	item=p_item;
	variant_editor->set_variant(iapi->get( p_path ),iapi->get_property_hint(p_path));
	set_pos( get_parent()->get_global_pos()+p_rect.pos );
	set_size( GUI::Size(1,1) );
	show();


}

EditorIAPI::EditPopup::EditPopup(Window *p_parent) : GUI::Window(p_parent,GUI::Window::MODE_POPUP,GUI::Window::SIZE_NORMAL) {

	variant_editor = memnew( EditorVariant );
	set_root_frame(variant_editor);
	variant_editor->changed_signal.connect( this, &EditorIAPI::EditPopup::variant_changed);
}


void EditorIAPI::item_edited(int col, GUI::TreeItem *item, String p_path,Variant::Type p_type) {

	static bool editing_item=false;;

	if (editing_item)
		return;
	editing_item=true;

	switch(p_type) {

		case Variant::BOOL: {

			iapi->set(p_path,item->is_checked(1));
		} break;
		case Variant::INT: {

			iapi->set(p_path,(int)item->get_range(1)->get());
		} break;
		case Variant::REAL: {

			iapi->set(p_path,(int)item->get_range(1)->get());
		} break;
		case Variant::STRING: {

			iapi->set(p_path,item->get_text(1));
			item->set_text( 1, iapi->get(p_path) );
		} break;

		default: ERR_PRINT("Invalid variant for callback.");
	}

	editing_item=false;

}



void EditorIAPI::edit_iapi_callback( IAPIRef p_iapi) {

	edit_iapi_signal.call( p_iapi );
}

void EditorIAPI::item_iapi_edited(int col, String p_path) {

	IAPIRef idata = iapi->get(p_path).get_IOBJ();
	if (idata.is_null())
		return; // may be unassigned

	get_window()->get_root()->update_signal.connect( Method( Method1<IAPIRef>( this, &EditorIAPI::edit_iapi_callback ), idata ) );
}


void EditorIAPI::method_call_slot( int p_idx ) {

	ERR_FAIL_INDEX( p_idx, methods.size() );
	IAPI::MethodInfo mi=methods[p_idx];

	if (mi.parameters.empty()) {


		Variant res = iapi->call(mi.name);
	} else {

		create_ecparams();
		ecparams->set(mi);
		ecparams_wb->get_window_top()->set_text("Method Parameters for: "+mi.name);
		ecparams_win->show();
		ecparams_type="method|"+mi.name;
		edited_method=p_idx;

	}


}

void EditorIAPI::item_popup_edited(int col, GUI::Rect p_rect,GUI::TreeItem *item, String p_path,Variant::Type p_type) {

	p_rect.pos-=get_window()->get_global_pos();
	edit_popup->edit( iapi, p_path, item, p_rect );
}


static GUI::TreeItem *get_parent_node(String p_path,Table<String,GUI::TreeItem*>& item_paths,GUI::TreeItem *root,GUI::Tree *tree,GUI::Skin *p_skin) {

	GUI::TreeItem *item=NULL;

	if (p_path=="") {

		item = root;
	} else if (item_paths.has(p_path)) {

		item = item_paths.get(p_path);
	} else {

//		printf("path %s parent path %s - item name %s\n",p_path.ascii().get_data(),p_path.left( p_path.find_last("/") ).ascii().get_data(),p_path.right( p_path.find_last("/") ).ascii().get_data() );
		GUI::TreeItem *parent = get_parent_node( p_path.left( p_path.find_last("/") ),item_paths,root,tree,p_skin );
		item = tree->create_item( parent );
		String name = (p_path.find("/")!=-1)?p_path.right( p_path.find_last("/") ):p_path;
		item->set_text(0, name );
		item->set_bitmap( 0, p_skin->get_bitmap( BITMAP_EDITOR_ICON_FOLDER ) );
		item_paths[p_path]=item;
	}

	return item;

}

void EditorIAPI::update_tree() {

	tree->clear();

	if (iapi.is_null())
		return;

	Table<String,GUI::TreeItem*> item_path;

	GUI::TreeItem *root = tree->create_item(NULL);

	root->set_text(0,iapi->get_type()); // todo, fetch name if ID exists in database
	root->set_bitmap(0,EditorMain::get_singleton()->get_editor_icons()->get( iapi->persist_as() ) );
	if (iapi->get_ID()==OBJ_INVALID_ID)
		root->set_text(1,"<built-in>");
	else
		root->set_text(1,"ID: "+String::num(iapi->get_ID()));


	List<IAPI::PropertyInfo> plist;
	iapi->get_property_list(&plist);

	for (List<IAPI::PropertyInfo>::Iterator *I=plist.begin() ; I ; I=I->next()) {

		IAPI::PropertyInfo& p = I->get();

		//make sure the property can be edited
		if ( ! (p.usage_flags&IAPI::PropertyInfo::USAGE_EDITING ) )
			continue;

		String path=p.path.left( p.path.find_last("/") ) ;
		//printf("property %s\n",p.path.ascii().get_data());
		GUI::TreeItem * parent = get_parent_node(path,item_path,root,tree,get_window()->get_skin() );

		GUI::TreeItem * item = tree->create_item( parent );

		String name = (p.path.find("/")!=-1)?p.path.right( p.path.find_last("/") ):p.path;

		item->set_text( 0, name );
		//printf("property %s type %i\n",p.path.ascii().get_data(),p.type);
		switch( p.type ) {

			case Variant::BOOL: {

				item->set_cell_mode( 1, GUI::CELL_MODE_CHECK );
				item->set_text(1,"On");
				item->set_checked( 1, iapi->get( p.path ) );
				item->set_bitmap( 0,bitmap( BITMAP_EDITOR_ICON_BOOL ) );
				item->set_editable(1,true);

				// C++ argument binding syntax sucks
				item->edited_signal.connect( Method1<int>( Method2<int,GUI::TreeItem*>( Method3<int,GUI::TreeItem*,String>( Method4<int,GUI::TreeItem*,String,Variant::Type>( this, &EditorIAPI::item_edited), p.type), p.path), item ) );
			} break;
			case Variant::REAL:
			case Variant::INT: {

				IAPI::PropertyHint ph = iapi->get_property_hint( p.path);

				if (ph.type==IAPI::PropertyHint::HINT_FLAGS) {

					item->set_cell_mode( 1, GUI::CELL_MODE_CUSTOM );
					item->set_text(1,"Flags..");
					item->set_editable(1,true);

					item->custom_popup_signal.connect( Method2<int,GUI::Rect>( Method3<int,GUI::Rect,GUI::TreeItem*>( Method4<int,GUI::Rect,GUI::TreeItem*,String>( Method5<int,GUI::Rect,GUI::TreeItem*,String,Variant::Type>( this, &EditorIAPI::item_popup_edited), p.type), p.path), item ) );
					break;
				}


				item->set_cell_mode( 1, GUI::CELL_MODE_RANGE );

				if (ph.type==IAPI::PropertyHint::HINT_RANGE) {

					int c = ph.hint.get_slice_count(",");
					if (c>=1) {

						item->get_range(1)->set_min( ph.hint.get_slice(",",0).to_double() );
					}
					if (c>=2) {

						item->get_range(1)->set_max( ph.hint.get_slice(",",1).to_double() );
					}

					if (p.type==Variant::REAL && c>=3) {

						item->get_range(1)->set_step( ph.hint.get_slice(",",2).to_double() );
					} else {

						item->get_range(1)->set_step( 1.0 );
					}
				}
				if (ph.type==IAPI::PropertyHint::HINT_ENUM) {

					int c = ph.hint.get_slice_count(",");

					item->get_range(1)->set_min( 0 );
					item->get_range(1)->set_max( c -1 );
					item->get_range(1)->set_step( 1 );
					item->set_text(1,ph.hint);
				}

				if (p.type==Variant::REAL) {
					item->set_bitmap( 0,bitmap( BITMAP_EDITOR_ICON_REAL ) );
					item->get_range(1)->set( iapi->get( p.path ).get_real() );

				} else {
					item->set_bitmap( 0,bitmap( BITMAP_EDITOR_ICON_INTEGER ) );
					item->get_range(1)->set( iapi->get( p.path ).get_int() );
				}


				item->set_editable(1,true);

				item->edited_signal.connect( Method1<int>( Method2<int,GUI::TreeItem*>( Method3<int,GUI::TreeItem*,String>( Method4<int,GUI::TreeItem*,String,Variant::Type>( this, &EditorIAPI::item_edited), p.type), p.path), item ) );

			} break;
			case Variant::STRING: {

				IAPI::PropertyHint ph = iapi->get_property_hint( p.path );

				switch (ph.type) {

					case IAPI::PropertyHint::HINT_NONE: {

						item->set_cell_mode( 1, GUI::CELL_MODE_STRING );
						item->set_editable(1,true);
						item->set_bitmap( 0,bitmap( BITMAP_EDITOR_ICON_STRING) );
						item->set_text(1,iapi->get(p.path));
						item->edited_signal.connect( Method1<int>( Method2<int,GUI::TreeItem*>( Method3<int,GUI::TreeItem*,String>( Method4<int,GUI::TreeItem*,String,Variant::Type>( this, &EditorIAPI::item_edited), p.type), p.path), item ) );

					} break;
					case IAPI::PropertyHint::HINT_FILE: {

						item->set_cell_mode( 1, GUI::CELL_MODE_CUSTOM );
						item->set_editable(1,true);
						item->set_bitmap( 0,bitmap( BITMAP_EDITOR_ICON_FILE) );
						item->custom_popup_signal.connect( Method2<int,GUI::Rect>( Method3<int,GUI::Rect,GUI::TreeItem*>( Method4<int,GUI::Rect,GUI::TreeItem*,String>( Method5<int,GUI::Rect,GUI::TreeItem*,String,Variant::Type>( this, &EditorIAPI::item_popup_edited), p.type), p.path), item ) );

					} break;

				}

			} break;
			case Variant::INT_ARRAY: {

				item->set_cell_mode( 1, GUI::CELL_MODE_CUSTOM );
				item->set_editable( 1, true );
				item->set_text(1,"[IntArray]");
				item->set_bitmap( 0,bitmap( BITMAP_EDITOR_ICON_ARRAY_INT) );


				item->custom_popup_signal.connect( Method2<int,GUI::Rect>( Method3<int,GUI::Rect,GUI::TreeItem*>( Method4<int,GUI::Rect,GUI::TreeItem*,String>( Method5<int,GUI::Rect,GUI::TreeItem*,String,Variant::Type>( this, &EditorIAPI::item_popup_edited), p.type), p.path), item ) );

			} break;
			case Variant::REAL_ARRAY: {

				item->set_cell_mode( 1, GUI::CELL_MODE_CUSTOM );
				item->set_editable( 1, true );
				item->set_text(1,"[RealArray]");
				item->set_bitmap( 0,bitmap( BITMAP_EDITOR_ICON_ARRAY_FLOAT) );


				item->custom_popup_signal.connect( Method2<int,GUI::Rect>( Method3<int,GUI::Rect,GUI::TreeItem*>( Method4<int,GUI::Rect,GUI::TreeItem*,String>( Method5<int,GUI::Rect,GUI::TreeItem*,String,Variant::Type>( this, &EditorIAPI::item_popup_edited), p.type), p.path), item ) );

			} break;
			case Variant::STRING_ARRAY: {

				item->set_cell_mode( 1, GUI::CELL_MODE_CUSTOM );
				item->set_editable( 1, true );
				item->set_text(1,"[StringArray]");
				item->set_bitmap( 0,bitmap( BITMAP_EDITOR_ICON_ARRAY_STRING) );


				item->custom_popup_signal.connect( Method2<int,GUI::Rect>( Method3<int,GUI::Rect,GUI::TreeItem*>( Method4<int,GUI::Rect,GUI::TreeItem*,String>( Method5<int,GUI::Rect,GUI::TreeItem*,String,Variant::Type>( this, &EditorIAPI::item_popup_edited), p.type), p.path), item ) );

			} break;
			case Variant::BYTE_ARRAY: {

				item->set_cell_mode( 1, GUI::CELL_MODE_CUSTOM );
				item->set_editable( 1, true );
				item->set_text(1,"[ByteArray]");
				item->set_bitmap( 0,bitmap( BITMAP_EDITOR_ICON_ARRAY_DATA) );


				item->custom_popup_signal.connect( Method2<int,GUI::Rect>( Method3<int,GUI::Rect,GUI::TreeItem*>( Method4<int,GUI::Rect,GUI::TreeItem*,String>( Method5<int,GUI::Rect,GUI::TreeItem*,String,Variant::Type>( this, &EditorIAPI::item_popup_edited), p.type), p.path), item ) );

			} break;
			case Variant::VECTOR3: {

				item->set_cell_mode( 1, GUI::CELL_MODE_CUSTOM );
				item->set_editable( 1, true );
				item->set_text(1,iapi->get(p.path).get_string());
				item->set_bitmap( 0,bitmap( BITMAP_EDITOR_ICON_VECTOR) );


				item->custom_popup_signal.connect( Method2<int,GUI::Rect>( Method3<int,GUI::Rect,GUI::TreeItem*>( Method4<int,GUI::Rect,GUI::TreeItem*,String>( Method5<int,GUI::Rect,GUI::TreeItem*,String,Variant::Type>( this, &EditorIAPI::item_popup_edited), p.type), p.path), item ) );

			} break;
			case Variant::MATRIX4: {

				item->set_cell_mode( 1, GUI::CELL_MODE_CUSTOM );
				item->set_editable( 1, true );
				item->set_text(1,"[Matrix4]");
				item->set_bitmap( 0,bitmap( BITMAP_EDITOR_ICON_MATRIX) );


				item->custom_popup_signal.connect( Method2<int,GUI::Rect>( Method3<int,GUI::Rect,GUI::TreeItem*>( Method4<int,GUI::Rect,GUI::TreeItem*,String>( Method5<int,GUI::Rect,GUI::TreeItem*,String,Variant::Type>( this, &EditorIAPI::item_popup_edited), p.type), p.path), item ) );

			} break;
			case Variant::PLANE: {

				item->set_cell_mode( 1, GUI::CELL_MODE_CUSTOM );
				item->set_editable( 1, true );
				item->set_text(1,iapi->get(p.path).get_string());
				item->set_bitmap( 0,bitmap( BITMAP_EDITOR_ICON_PLANE) );


				item->custom_popup_signal.connect( Method2<int,GUI::Rect>( Method3<int,GUI::Rect,GUI::TreeItem*>( Method4<int,GUI::Rect,GUI::TreeItem*,String>( Method5<int,GUI::Rect,GUI::TreeItem*,String,Variant::Type>( this, &EditorIAPI::item_popup_edited), p.type), p.path), item ) );

			} break;
			case Variant::RECT3: {

				item->set_cell_mode( 1, GUI::CELL_MODE_CUSTOM );
				item->set_editable( 1, true );
				item->set_text(1,"[Rect3 (AABB)]");
				item->set_bitmap( 0,bitmap( BITMAP_EDITOR_ICON_RECT3) );


				item->custom_popup_signal.connect( Method2<int,GUI::Rect>( Method3<int,GUI::Rect,GUI::TreeItem*>( Method4<int,GUI::Rect,GUI::TreeItem*,String>( Method5<int,GUI::Rect,GUI::TreeItem*,String,Variant::Type>( this, &EditorIAPI::item_popup_edited), p.type), p.path), item ) );

			} break;
			case Variant::QUAT: {

				item->set_cell_mode( 1, GUI::CELL_MODE_CUSTOM );
				item->set_editable( 1, true );
				item->set_text(1,iapi->get(p.path).get_string());
				item->set_bitmap( 0,bitmap( BITMAP_EDITOR_ICON_QUAT) );


				item->custom_popup_signal.connect( Method2<int,GUI::Rect>( Method3<int,GUI::Rect,GUI::TreeItem*>( Method4<int,GUI::Rect,GUI::TreeItem*,String>( Method5<int,GUI::Rect,GUI::TreeItem*,String,Variant::Type>( this, &EditorIAPI::item_popup_edited), p.type), p.path), item ) );

			} break;
			case Variant::COLOR: {

				item->set_cell_mode( 1, GUI::CELL_MODE_CUSTOM );
				item->set_editable( 1, true );
				item->set_text(1,iapi->get(p.path).get_string());
				//item->set_custom_color(1,GUI::Color(iapi->get(p.path).get_color().r,iapi->get(p.path).get_color().g,iapi->get(p.path).get_color().b));
				item->set_bitmap( 0,bitmap( BITMAP_EDITOR_ICON_COLOR) );


				item->custom_popup_signal.connect( Method2<int,GUI::Rect>( Method3<int,GUI::Rect,GUI::TreeItem*>( Method4<int,GUI::Rect,GUI::TreeItem*,String>( Method5<int,GUI::Rect,GUI::TreeItem*,String,Variant::Type>( this, &EditorIAPI::item_popup_edited), p.type), p.path), item ) );

			} break;
			case Variant::IMAGE: {
				item->set_cell_mode( 1, GUI::CELL_MODE_CUSTOM );
				item->set_editable( 1, true );
				item->set_text(1,"[Image]");
//				item->set_bitmap( 0,bitmap( BITMAP_EDITOR_ICON_IMAGE) );


				item->custom_popup_signal.connect( Method2<int,GUI::Rect>( Method3<int,GUI::Rect,GUI::TreeItem*>( Method4<int,GUI::Rect,GUI::TreeItem*,String>( Method5<int,GUI::Rect,GUI::TreeItem*,String,Variant::Type>( this, &EditorIAPI::item_popup_edited), p.type), p.path), item ) );

			} break;
			case Variant::IOBJ: {

				item->set_cell_mode( 1, GUI::CELL_MODE_CUSTOM );
				item->set_editable( 1, true );
				String type;
				bool notnil=false;
				if (iapi->get( p.path ).get_type() == Variant::NIL || iapi->get( p.path ).get_IOBJ().is_null()) {
					item->set_text(1,"<null>");

				} else {
					item->set_text(1,"<ObjectRef>");
					notnil=true;

				}
				IAPI::PropertyHint ph= iapi->get_property_hint( p.path );

				if (ph.type==IAPI::PropertyHint::HINT_IOBJ_TYPE) {
					//printf("prop %s , type %s\n",p.path.ascii().get_data(),ph.hint.ascii().get_data());
					GUI::BitmapID bmpid= EditorMain::get_singleton()->get_editor_icons()->get( ph.hint );
					if (bmpid<0)
						break;
					item->set_bitmap( 0, bmpid );
					if (notnil)
						item->set_text(1,"<"+ph.hint+">");
				}

				item->double_click_signal.connect( Method1<int>( Method2<int,String>( this, &EditorIAPI::item_iapi_edited ), p.path ) );

				item->custom_popup_signal.connect( Method2<int,GUI::Rect>( Method3<int,GUI::Rect,GUI::TreeItem*>( Method4<int,GUI::Rect,GUI::TreeItem*,String>( Method5<int,GUI::Rect,GUI::TreeItem*,String,Variant::Type>( this, &EditorIAPI::item_popup_edited), p.type), p.path), item ) );
			} break;
			default: {};
		}
	}


}

void EditorIAPI::update_methods() {

	method_box->clear();
	methods.clear();
	if (iapi.is_null()) {
		method_box->hide();
		return;
	}

	List< IAPI::MethodInfo > method_list;
	iapi->get_method_list( &method_list );
	List< IAPI::MethodInfo >::Iterator *I=method_list.begin();

	if (I==NULL) {

		method_box->hide();
	} else {

		method_box->show();
	}
	int i=0;
	while (I) {

		IAPI::MethodInfo &m =I->get();
		String method_str;
		method_str+=Variant::get_type_string(m.return_type)+" ";
		method_str+=iapi->persist_as()+" :: "+m.name+"(";
		List<IAPI::MethodInfo::ParamInfo>::Iterator *J=m.parameters.begin();
		bool first=true;
		while(J) {

			if (!first)
				method_str+=", ";
			else
				first=false;

			IAPI::MethodInfo::ParamInfo& p=J->get();
			method_str+=Variant::get_type_string(p.type)+" "+p.name;

			J=J->next();

		}

		method_str+=")";
		//method_box->add_item( bitmap(BITMAP_EDITOR_ICON_FUNC), method_str, i );
		method_box->add_item( bitmap(BITMAP_EDITOR_ICON_FUNC), m.name, i );
		methods.push_back(m);

		I=I->next();

		i++;
	}


}

void EditorIAPI::new_iapi_callback(int,String p_type) {

	IAPI::MethodInfo minfo= IAPI_Persist::get_singleton()->get_creation_param_hints(p_type);

	if (minfo.parameters.empty()) {

		IAPIRef iref = IAPI_Persist::get_singleton()->instance_type( p_type );
		edit_iapi_signal.call( iref );
		return;
	} else {

		create_ecparams();
		ecparams->set(minfo);
		ecparams_wb->get_window_top()->set_text("Creation Parameters for: "+p_type);
		ecparams_win->show();
		ecparams_type=p_type;
	}

}

void EditorIAPI::create_with_params(const IAPI::CreationParams& p_params) {


	printf("create with type %s\n",ecparams_type.ascii().get_data());
	if (ecparams_type.find("method|")==0) {

		ecparams_type=ecparams_type.right(ecparams_type.find("|"));

		IAPI::MethodInfo mi=methods[edited_method];

		List<Variant> params;

		for (int i=0;i<mi.parameters.size();i++) {

			params.push_back( p_params.get(mi.parameters[i].name) );

		}

		iapi->call(mi.name, params );
		ecparams_win->hide();
	} else {
		ecparams_win->hide();
		IAPIRef iref = IAPI_Persist::get_singleton()->instance_type( ecparams_type, p_params );
		edit_iapi_signal.call( iref );
	}

}

void EditorIAPI::file_access_activated_callback(String p_string) {

	switch(db_access->get_mode()) {

		case EditorDB::MODE_OPEN: {

			IAPIRef tmpiapi = IAPI_Persist::get_singleton()->load(p_string);
			edit_iapi_signal.call(tmpiapi);
		} break;
		case EditorDB::MODE_SAVE: {

			IAPI_Persist::get_singleton()->save(iapi,p_string);
			GUI::TreeItem * root = tree->get_root_item();
			if (!root)
				break;
			if (iapi->get_ID()==OBJ_INVALID_ID)
				root->set_text(1,"<built-in>");
			else
				root->set_text(1,"ID: "+String::num(iapi->get_ID()));

		} break;
	}

	db_win->hide();
}

void EditorIAPI::menu_callback(int p_option) {

	switch ( p_option ) {
		case OPTION_FILE_NEW: {

			if (!iapi_select_dialog) {

				iapi_select_dialog= memnew( GUI::OptionInputDialog(get_window()->get_root() ) );
				List<String> iapi_types;
				IAPI_Persist::get_singleton()->get_registered_types(&iapi_types);
				List<String>::Iterator *I=iapi_types.begin();
				while(I) {

					iapi_select_dialog->add_option(I->get());
					I=I->next();
				}

				iapi_select_dialog->option_selected_signal.connect(this, &EditorIAPI::new_iapi_callback);
			}

			iapi_select_dialog->show("Choose Object Type:               ");

		} break;
		case OPTION_FILE_OPEN: {

			create_ecparams();
			db_access->activate( EditorDB::MODE_OPEN, "","" );
			db_wb->get_window_top()->set_text("Databace Access: Load Object.");
			db_win->show();
		} break;
		case OPTION_FILE_SAVE: {

			if (iapi.is_null())
				break;

			if (iapi->get_ID()!=OBJ_INVALID_ID) {

				IAPI_Persist::get_singleton()->save( iapi );
				break;
			}
		} ; // fall through
		case OPTION_FILE_SAVE_AS: {

			if (iapi.is_null())
				break;
			create_ecparams();
			db_access->activate( EditorDB::MODE_SAVE, "", iapi->get_type() );
			db_wb->get_window_top()->set_text("Databace Access: Save Object.");
			db_win->show();

		} break;
		case OPTION_FILE_IMPORT: {

		} break;
		case OPTION_FILE_QUIT: {

		} break;
	}

}

void EditorIAPI::set_iapi( IAPIRef p_iapi ) {

	if (!iapi.is_null())
		iapi->property_changed_signal.disconnect(this,&EditorIAPI::iapi_changed_callback);

	iapi=p_iapi;

	if (!iapi.is_null())
		iapi->property_changed_signal.connect(this,&EditorIAPI::iapi_changed_callback);
	update_tree();
	update_methods();
}

void EditorIAPI::create_ecparams() {

	if (ecparams_wb)
		return;

	ecparams_win = memnew( GUI::Window(get_window()->get_root(),GUI::Window::MODE_POPUP,GUI::Window::SIZE_TOPLEVEL_CENTER ) );

	ecparams_wb = memnew( GUI::WindowBox("Creation Parameters:") );
	ecparams_win->set_root_frame( ecparams_wb );
	ecparams = ecparams_wb->add( memnew( EditorCreationParams ),1 );
	ecparams->submit_params_signal.connect(this,&EditorIAPI::create_with_params);

	db_wb = memnew( GUI::WindowBox("Database Access:") );
	db_win = memnew( GUI::Window(get_window()->get_root(), GUI::Window::MODE_POPUP,GUI::Window::SIZE_TOPLEVEL_CENTER ) );
	db_win->set_root_frame( db_wb );
	db_access = db_wb->add( memnew( EditorDB ) ,1);
	db_access->activated_signal.connect(this, &EditorIAPI::file_access_activated_callback);

}

void EditorIAPI::set_in_window() {

	if (!no_fileops)
		add( memnew( GUI::Label("Object Editor", GUI::Label::ALIGN_CENTER) ) );
	GUI::HBoxContainer *hbc = add( memnew( GUI::HBoxContainer ) );
	GUI::MenuBox *file_menu = hbc->add( memnew( GUI::MenuBox("", bitmap( BITMAP_EDITOR_ICON_FILE ) ) ) );

	file_menu->add_item("New Object",OPTION_FILE_NEW);
	file_menu->add_item("Open",OPTION_FILE_OPEN);
	file_menu->add_item("Save",OPTION_FILE_SAVE);
	file_menu->add_item("Save As..",OPTION_FILE_SAVE_AS);
	file_menu->add_separator();
	file_menu->add_item("Quit",OPTION_FILE_QUIT);
	file_menu->item_selected_signal.connect( this, &EditorIAPI::menu_callback );

	//GUI::MenuBox *edit_menu = hbc->add( memnew( GUI::MenuBox("", bitmap( BITMAP_EDITOR_ICON_EDIT ) ) ),1 );
	hbc->add( memnew( GUI::Widget) , 1 );
	if (no_fileops)
		hbc->hide();
	method_box = hbc->add( memnew( GUI::MenuBox( "Methods..",bitmap(BITMAP_EDITOR_ICON_FUNC))) );
	method_box->item_selected_signal.connect( this, &EditorIAPI::method_call_slot );
	method_box->pre_show_signal.connect( this, &EditorIAPI::update_methods);
	scroll = add( memnew( GUI::ScrollBox ),1 );
	scroll->set_expand_h(true);
	scroll->set_expand_v(true);
	tree = scroll->set( memnew( GUI::Tree(2)) );

	edit_popup = memnew( EditPopup( get_window() ) );
	iapi_select_dialog=NULL;

	ecparams_wb=NULL;
	ecparams_win=NULL;
	ecparams=NULL;
	db_access=NULL;
	db_wb=NULL;
	db_win=NULL;

}

void EditorIAPI::iapi_changed_callback(const String&) {

	get_window()->get_root()->update_signal.connect(this,&EditorIAPI::reload);

}
void EditorIAPI::reload() {

	set_iapi(iapi);
}

EditorIAPI::EditorIAPI(bool p_no_fileops) {

	no_fileops=p_no_fileops;

}


EditorIAPI::~EditorIAPI() {

	if (iapi_select_dialog)
		memdelete( iapi_select_dialog );
	if (ecparams_win)
		memdelete(ecparams_win);
	if (db_win)
		memdelete( db_win );
}


