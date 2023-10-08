//
// C++ Implementation: file_dialog
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "file_dialog.h"
#include "containers/box_container.h"
#include "containers/center_container.h"
#include "bundles/window_box.h"
#include "widgets/window_top.h"

namespace GUI {

bool FileDialog::ignore_filter() {

	return false;
}
FileDialog::FileMode FileDialog::get_mode() {

	return mode;
}
void FileDialog::show( FileMode p_mode ) {


	mode=p_mode;

	action->set_text( (mode==MODE_SAVE)?"Save":"Load" );

	file_list->set_multi_mode( p_mode==MODE_OPEN_MULTI );



	if (p_mode==MODE_OPEN_MULTI || p_mode==MODE_OPEN_DIR)
		file_group->hide();
	else
		file_group->show();

	file->clear();
	file_list->get_focus();

	if (mode==MODE_OPEN_DIR)
		wb->get_window_top()->set_text( "File Dialog - Pick Directory" );
	else
		wb->get_window_top()->set_text( (mode==MODE_SAVE)?"File Dialog - Save":"File Dialog - Load" );

	update_filter_list();
	update_file_list();

	Window::show();



}

void FileDialog::set_dir(String p_dir) {

	if (!fs->change_dir( p_dir ))
		update_file_list();
}

void FileDialog::drive_changed(int p_to) {

	if (updating_drive)
		return;

	String drive_str=fs->get_drive( p_to );
	fs->change_dir( drive_str );
	update_file_list();

}

void FileDialog::filter_type_changed(int p_to) {

	update_file_list();
}

void FileDialog::update_file_list() {


	String current_dir=fs->get_current_dir();


	if (fs->get_drive_count()==0) {
		drive->hide();

	} else {
		drive->clear();
		drive->show();
		updating_drive=true;

		int which_drive=-1;
		for (int i=0;i<fs->get_drive_count();i++) {


			String drive_str=fs->get_drive( i );
			drive->add_string( drive_str );

			if (current_dir.findn( drive_str )==0)
				which_drive=i;
		}

		if (which_drive>=0)
			drive->select(which_drive);
		updating_drive=false;
	}


	file_list->clear();
	fs->list_dir_begin();


	int current_filter=filter_box->get_selected()-1;
	int total=filter_box->get_size()-2;

	while(true) {

		bool is_dir;
		String f= fs->get_next( &is_dir );
		if (f=="")
			break;

		if (f!=".." && f[0]=='.' && fs->get_drive_count()==0)
			continue; //unix hidden file
		if (is_dir)
			f=" "+f+"/";
		else if (filter_list && current_filter<total && !ignore_filter()) {


			FilterList *fl=filter_list;
			int filter_idx=0;
			bool matches=false;
			while (fl) {

				if (current_filter==-1 || current_filter==filter_idx) {

					if (f.findn(fl->filter)>=0)
						matches=true;
				}
				fl=fl->next;
				filter_idx++;
			}

			if (!matches)
				continue;

		}

		if (!is_dir && mode==MODE_OPEN_DIR)
			continue;

		file_list->add_sorted_string( f );

	}

	fs->list_dir_end();

	path->set_text( current_dir );

	file_list->set_cursor_pos( 0 );
}

void FileDialog::file_selected(String p_string) {}


void FileDialog::file_selected_slot(int p_which) {

	String str=file_list->get_string( p_which );

	if (str.find("/")<(str.length()-1) && str.find("\\")<(str.length()-1)) {

		file->set_text( str );
		file_selected(str);
	}

}

void FileDialog::open_slot(int p_which) {


	String str=file_list->get_string( p_which );

	if (str.find("/")!=-1 || str.find("\\")!=-1) {

		if (str[0]==' ')
			str=str.substr( 1, str.length() -2 );
		fs->change_dir( str.utf8().get_data() );
		update_file_list();
	} else {

		ok_pressed();
	}
}

void FileDialog::update_filter_list() {

	if (!filter_list || mode==MODE_OPEN_DIR) {
		filter_group->hide();
		return;
	}

	String filter="All ( ";

	FilterList *f=filter_list;

	while (f) {

		filter+="*"+f->filter;
		if (f->next)
			filter+=",";

		f=f->next;
	}

	filter+=" )";

	filter_box->clear();

	filter_box->add_string( filter ); //default filter

	f=filter_list;

	while (f) {

		filter_box->add_string( f->description + " ( *"+f->filter+" )" );
		f=f->next;
	}

	filter_group->show();

	filter_box->add_string( "No Filter ( All Files )" );
}

void FileDialog::add_filter(String p_description,String p_extension) {

	p_extension.replace( "*","" );

	FilterList *f = GUI_NEW( FilterList );
	f->filter=p_extension;
	f->description=p_description;
	f->next=filter_list;
	filter_list=f;

	update_filter_list();
	update_file_list();
}
void FileDialog::clear_filter_list() {

	while (filter_list) {

		FilterList *aux=filter_list;
		filter_list=filter_list->next;
		GUI_DELETE( aux );
	}

	update_filter_list();
}

int FileDialog::get_file_count() {


	if (mode!=MODE_OPEN_MULTI && mode !=MODE_OPEN_DIR)
		return 0;

	return file_list->get_selection_size();
}
String FileDialog::get_path(int p_which) {

	if (mode!=MODE_OPEN_MULTI && mode !=MODE_OPEN_DIR)
		return "";

	String path=fs->get_current_dir()+"/"+file_list->get_selected_string( p_which );

	return path;
}

String FileDialog::get_file(int p_which) {

	if (mode!=MODE_OPEN_MULTI && mode !=MODE_OPEN_DIR)
		return "";

	String file=file_list->get_selected_string( p_which );

	return file;
}

String FileDialog::get_dir() {

	return fs->get_current_dir();
}

void FileDialog::ok_pressed_s(String) {

	ok_pressed();
}
void FileDialog::path_lineedit_changed(String p_new) {

	fs->change_dir( p_new );
	update_file_list();
}


void FileDialog::ok_pressed() {

	if (mode==MODE_OPEN) {

		String path=fs->get_current_dir()+"/"+file->get_text();

		if (!fs->file_exists( path ))
			return; //booh

		hide();
		file_activated_signal.call(path);

	} else if (mode==MODE_OPEN_MULTI) {

		//hide();
		multi_file_activated_signal.call();
		// ?
	} else if (mode==MODE_OPEN_DIR) {

		hide();
		dir_selected_signal.call(fs->get_current_dir());

	} else { //save

		String path=fs->get_current_dir()+"/"+file->get_text();
		if (fs->file_exists( path )) { //make sure it's ok!


		}
		hide();
		file_activated_signal.call(path);

	}
}

void FileDialog::set_custom_filesystem(FileSystem *p_custom_filesystem,bool p_own_it) {

	if (!p_custom_filesystem)
		return;

	if (own_filesystem) {
		GUI_DELETE( fs );
	}
	fs=p_custom_filesystem;
	own_filesystem=p_own_it;

	update_file_list();

}

void FileDialog::set_incremental_search(bool p_enabled) {

	file_list->set_incremental_search(p_enabled);
}
FileDialog::FileDialog(Window *p_parent) : Window(p_parent,MODE_POPUP,SIZE_TOPLEVEL_CENTER) {

        filter_list=0;
	WindowBox *vb = GUI_NEW( WindowBox("File Dialog") );
	set_root_frame( vb );
	wb=vb;

	MarginGroup *path_mg= vb->add( GUI_NEW( MarginGroup("Path") ),0);
	HBoxContainer *path_hbc = path_mg->add( GUI_NEW( HBoxContainer), 0 );

	path = path_hbc->add( GUI_NEW( LineEdit), 7);
	path->text_enter_signal.connect(this,&FileDialog::path_lineedit_changed );

	drive = path_hbc->add( GUI_NEW( ComboBox ), 1 );
	drive->selected_signal.connect( this, &FileDialog::drive_changed );

	HBoxContainer *file_hb = vb->add( GUI_NEW(MarginGroup("Directories & Files")),1)->add( GUI_NEW( HBoxContainer ), 1);
	file_list = file_hb ->add( GUI_NEW( List ), 1);
	VScrollBar *vsb=file_hb->add( GUI_NEW( VScrollBar), 0);
	vsb->set_range( file_list->get_range() );
	vsb->set_auto_hide( true );

	filter_group=vb->add( GUI_NEW(MarginGroup("Filter")),0);
	filter_box=filter_group->add( GUI_NEW( ComboBox), 0 );
	filter_box->selected_signal.connect( this, &FileDialog::filter_type_changed );

	filter_group->hide();

	extra_vb = vb->add(GUI_NEW( VBoxContainer));

	file_group=vb->add( GUI_NEW( MarginGroup("Selected file")),0);
	file=file_group->add( GUI_NEW( LineEdit), 0);
	file->text_enter_signal.connect(this,&FileDialog::ok_pressed_s );

	action=vb->add( GUI_NEW( CenterContainer),0 )->set( GUI_NEW( Button("Open")));
	action->pressed_signal.connect( this, &FileDialog::ok_pressed );

	mode=MODE_OPEN;

	/* instance filesystem */
	fs = FileSystem::instance_func();
	own_filesystem=true;

	file_list->selected_signal.connect( this, &FileDialog::file_selected_slot );
	file_list->activated_signal.connect( this, &FileDialog::open_slot );
	file_list->cursor_changed_signal.connect( this, &FileDialog::file_selected_slot );

	updating_drive=false;
}


FileDialog::~FileDialog()
{
	if (own_filesystem) {
		GUI_DELETE( fs );
	}
	clear_filter_list();
}


}
