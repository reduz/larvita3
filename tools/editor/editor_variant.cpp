//
// C++ Implementation: editor_variant
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "editor_variant.h"
#include "widgets/line_edit.h"
#include "widgets/scroll_bar.h"
#include "widgets/label.h"
#include "containers/grid_container.h"
#include "editor/editor_skin.h"
#include "bundles/window_box.h"
#include "widgets/check_button.h"
#include "iapi_persist.h"
#include "editor/editor_db.h"

void EditorVariant::iobj_create() {
		
	List<String> suppliers;
	
	if (hint.type==IAPI::PropertyHint::HINT_IOBJ_TYPE) {
		
		IAPI_Persist::get_singleton()->get_suppliers( hint.hint,& suppliers );
		
		ERR_FAIL_COND(suppliers.size()==0);		
		
	} else {
		
		IAPI_Persist::get_singleton()->get_registered_types(&suppliers);				
	}
	if (suppliers.size()==1) {
			
		local=IObjRef(IAPI_Persist::get_singleton()->instance_type(suppliers[0]));
		changed_signal.call(local);
		return;
	} else {
			
		if (!iapi_option) {
			iapi_option= memnew( GUI::OptionInputDialog(get_window()->get_root() ) );
		}
				
		iapi_option->clear();
			
		for (int i=0;i<suppliers.size();i++) {
				
			iapi_option->add_option( suppliers[i] );
		}
			
		iapi_option->show("Choose Object Type:      ");
			
	}
	
}

void EditorVariant::iobj_load_callbak(String p_path) {

	local=IObjRef(IAPI_Persist::get_singleton()->load(p_path));
	edb_win->hide();
	changed_signal.call(local);

}
void EditorVariant::iobj_load() {
	
	if (!edb_win) {
	
		GUI::WindowBox * edb_wb = memnew( GUI::WindowBox("Database Access:") );
		edb_win = memnew( GUI::Window(get_window()->get_root(), GUI::Window::MODE_POPUP,GUI::Window::SIZE_TOPLEVEL_CENTER ) );
		edb_win->set_root_frame( edb_wb );
		edb = edb_wb->add( memnew( EditorDB ) ,1);	
		edb->activated_signal.connect(this, &EditorVariant::iobj_load_callbak);
	}
	
	edb_win->show();
	String edbhint;
	if (hint.type==IAPI::PropertyHint::HINT_IOBJ_TYPE)
		edbhint=hint.hint;
		
	edb->activate(EditorDB::MODE_OPEN,"",edbhint);
}

void EditorVariant::iobj_clear() {
	
	local=IObjRef();
	changed_signal.call(local);	
}


void EditorVariant::vector_set_x(String p_x) {

	Vector3 v = local.get_vector3();
	v.set_x( p_x.to_double() );
	local=v;
	changed_signal.call(local);

}
void EditorVariant::vector_set_y(String p_y) {

	Vector3 v = local.get_vector3();
	v.set_y( p_y.to_double() );
	local=v;
	changed_signal.call(local);

}
void EditorVariant::vector_set_z(String p_z) {

	Vector3 v = local.get_vector3();
	v.set_z( p_z.to_double() );
	local=v;
	changed_signal.call(local);

}

void EditorVariant::plane_set_nx(String p_x) {

	Plane p = local.get_plane();
	p.normal.set_x( p_x.to_double() );
	local=p;
	changed_signal.call(local);

}
void EditorVariant::plane_set_ny(String p_y) {

	Plane p = local.get_plane();
	p.normal.set_y( p_y.to_double() );
	local=p;
	changed_signal.call(local);

}
void EditorVariant::plane_set_nz(String p_z) {

	Plane p = local.get_plane();
	p.normal.set_z( p_z.to_double() );
	local=p;
	changed_signal.call(local);

}
void EditorVariant::plane_set_d(String p_d) {

	Plane p = local.get_plane();
	p.d=p_d.to_double() ;
	local=p;
	changed_signal.call(local);

}

void EditorVariant::matrix_set(String p_v,int p_row,int p_col) {

	Matrix4 m = local.get_matrix4();
	m.set(p_col,p_row,p_v.to_double());
	local=m;
	changed_signal.call(local);
}

void EditorVariant::array_item_edited_callback(int p_row,String p_item) {

	switch(local.get_type()) {
	
		case Variant::BYTE_ARRAY: {
		
			local.array_set_byte( p_row, p_item.to_int() );
		} break;
		case Variant::INT_ARRAY: {
		
			local.array_set_int( p_row, p_item.to_int() );
		} break;
		case Variant::REAL_ARRAY: {
		
			local.array_set_real( p_row, p_item.to_double() );
		} break;
		case Variant::STRING_ARRAY: {
		
			local.array_set_string( p_row, p_item );
		} break;
	}
	
	array_edit->set_string(local.array_get(p_row).get_string(), p_row );
	
	changed_signal.call(local);
}

void EditorVariant::color_edited_callback() {

	local=Color( color_picker->get_color().r, color_picker->get_color().g, color_picker->get_color().b,color_picker->get_alpha());
	changed_signal.call(local);
}



void EditorVariant::file_opened(String p_file) {


//	DirAccess *da = DirAccess::create();
//	String currentda->get_current_dir();

	fdialog->hide();
	local=p_file;
	changed_signal.call(local);
	
}

void EditorVariant::file_dialog_show() {

	if (!fdialog) {
	
		ERR_FAIL_COND( !get_window() );
		fdialog = memnew( GUI::FileDialog(get_window()->get_root() ) );
		fdialog->file_activated_signal.connect( this, &EditorVariant::file_opened);
		
	}
	
	
	fdialog->clear_filter_list();
	
	if (local.get_type()==Variant::STRING && hint.type==IAPI::PropertyHint::HINT_FILE) {
		
		if (hint.hint.size()) {
			for(int i=0;i<hint.hint.get_slice_count(";");i++) {
			
				fdialog->add_filter("",hint.hint.get_slice(";",i) );		
			}
		}
		
		fdialog->show(GUI::FileDialog::MODE_OPEN);
	}

	
}

void EditorVariant::flag_toggled(bool p_toggle,int p_which) {

	if (hint.type!=IAPI::PropertyHint::HINT_FLAGS)
		return;
		
	int val=local.get_int();
	
	if (p_toggle) {
	
		val|=(1<<p_which);
	} else {
		val&=~(1<<p_which);
	}
	local=val;
	changed_signal.call(local);
}

void EditorVariant::set_variant( const Variant& p_variant, const IAPI::PropertyHint& p_hint ) {

	if (base)
		memdelete( base );
	base=NULL;
	
	local=p_variant;
	hint=p_hint;
	
	
	switch (local.get_type()) {
	
		case Variant::INT: {
	
			switch(hint.type) {
			
				case IAPI::PropertyHint::HINT_FLAGS: {
								
					GUI::VBoxContainer *flags_vb = add( memnew( GUI::VBoxContainer ) );
					
					int buttons=hint.hint.get_slice_count(",");
					
					for(int i=0;i<buttons;i++) {
					
						String h=hint.hint.get_slice(",",i).strip_edges();
						if (h=="")
							continue;
							
						GUI::CheckButton *b = flags_vb->add( memnew( GUI::CheckButton(h) ) );
						b->toggled_signal.connect( Method1<bool>( Method2<bool,int>( this, &EditorVariant::flag_toggled ), i ) );
						if (local.get_int()&(1<<i))
							b->set_checked(true);
						
					}
					base=flags_vb;
				
				} break;
				
			}	
	
		} break;
		case Variant::STRING: {
		
			switch(hint.type) {
			
				case IAPI::PropertyHint::HINT_FILE: {
								
					GUI::Button *browse=add(memnew( GUI::Button("Browse..") ));
					browse->pressed_signal.connect( this, &EditorVariant::file_dialog_show );
					base=browse;
				
				} break;
				
			}
		
		} break;
	
		case Variant::VECTOR3: {
		
			GUI::HBoxContainer *hbc = add( memnew( GUI::HBoxContainer ) );
			hbc->add( memnew( GUI::Label("X:") ) );
			GUI::LineEdit *le = hbc->add( memnew( GUI::LineEdit( String::num( local.get_vector3().get_x() ) ) ) );
			le->text_enter_signal.connect( this, &EditorVariant::vector_set_x );
			hbc->add( memnew( GUI::Label(" Y:") ) );
			le = hbc->add( memnew( GUI::LineEdit( String::num( local.get_vector3().get_y() ) ) ) );	le->text_enter_signal.connect( this, &EditorVariant::vector_set_y );
			hbc->add( memnew( GUI::Label(" Z:") ) );
			le = hbc->add( memnew( GUI::LineEdit( String::num( local.get_vector3().get_z() ) ) ) );
			le->text_enter_signal.connect( this, &EditorVariant::vector_set_z );
			
			base=hbc;
		} break;
		case Variant::PLANE: {
		
			GUI::HBoxContainer *hbc = add( memnew( GUI::HBoxContainer ) );
			hbc->add( memnew( GUI::Label("A:") ) );
			GUI::LineEdit *le = hbc->add( memnew( GUI::LineEdit( String::num( local.get_plane().normal.x ) ) ) );
			le->text_enter_signal.connect( this, &EditorVariant::plane_set_nx );
			
			hbc->add( memnew( GUI::Label(" B:") ) );
			le = hbc->add( memnew( GUI::LineEdit( String::num( local.get_plane().normal.y ) ) ) );
			le->text_enter_signal.connect( this, &EditorVariant::plane_set_ny );
			
			hbc->add( memnew( GUI::Label(" C:") ) );
			le = hbc->add( memnew( GUI::LineEdit( String::num( local.get_plane().normal.z ) ) ) );
			le->text_enter_signal.connect( this, &EditorVariant::plane_set_nz );
			
			hbc->add( memnew( GUI::Label(" D:") ) );
			le = hbc->add( memnew( GUI::LineEdit( String::num( local.get_plane().d ) ) ) );
			le->text_enter_signal.connect( this, &EditorVariant::plane_set_d );
			
			base=hbc;
		} break;
		case Variant::MATRIX4: {
		
			GUI::GridContainer * gc = add( memnew( GUI::GridContainer(4) ) );
					
			for (int r=0;r<4;r++) {
			
				for (int c=0;c<4;c++) {
			
					GUI::LineEdit *le = gc->add( memnew( GUI::LineEdit( String::num( local.get_matrix4().get(c,r) ) ) ),false,false );
					
					le->text_enter_signal.connect( Method1<String>( Method2<String,int>( Method3<String,int,int>( this, &EditorVariant::matrix_set ), c), r ) );
			
				}
			}
		
			base=gc;
		} break;
		case Variant::BYTE_ARRAY:
		case Variant::INT_ARRAY:
		case Variant::REAL_ARRAY:
		case Variant::STRING_ARRAY:
		case Variant::VARIANT_ARRAY: {
			
			GUI::HBoxContainer * hbc = add( memnew( GUI::HBoxContainer ) );
			array_edit = hbc->add( memnew( GUI::List ), 1);
			hbc->add( memnew( GUI::VScrollBar ), 1)->set_range( array_edit->get_range() );			
			array_edit->set_minimum_size( GUI::Size( constant(C_EDITOR_VARIANT_LIST_WIDTH), constant(C_EDITOR_VARIANT_LIST_HEIGHT)) );
			array_edit->set_number_elements(0);
			array_edit->set_editable(true);
			
			for (int i=0;i<local.size();i++) {
										
				array_edit->add_string( local.array_get(i).get_string() );
			}
			array_edit->item_edited_signal.connect( this, &EditorVariant::array_item_edited_callback);
			base=hbc;
		} break;
		case Variant::COLOR: {
		
			color_picker = add( memnew( GUI::ColorPicker(true) ) );
			color_picker->color_changed_signal.connect( this, &EditorVariant::color_edited_callback );
			color_picker->set_color( GUI::Color (local.get_color().r,local.get_color().r,local.get_color().r),local.get_color().a);
			
			base=color_picker;
			
		
		} break;
		case Variant::IOBJ: {
		
			GUI::HBoxContainer *hbc = add( memnew( GUI::HBoxContainer ) );
			
			GUI::Button *createobj = hbc->add( memnew( GUI::Button("New") ) );
			createobj->pressed_signal.connect(this,&EditorVariant::iobj_create);
			GUI::Button *loadobj = hbc->add( memnew( GUI::Button("Load") ) );
			loadobj->pressed_signal.connect(this,&EditorVariant::iobj_load);
			GUI::Button *clearobj = hbc->add( memnew( GUI::Button("Clear") ) );
			clearobj->pressed_signal.connect(this,&EditorVariant::iobj_clear);
			
			base=hbc;
		
		} break;
		
	}
}

EditorVariant::EditorVariant() {

	base=NULL;
	fdialog=NULL;
	iapi_option=NULL;
	edb_win=NULL;
}
EditorVariant::~EditorVariant() {

	if (fdialog)
		memdelete(fdialog);
	if (iapi_option)
		memdelete(iapi_option);
	if (edb_win)
		memdelete(edb_win);
}

