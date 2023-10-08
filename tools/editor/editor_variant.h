//
// C++ Interface: editor_variant
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EDITOR_VARIANT_H
#define EDITOR_VARIANT_H

/**
	@author ,,, <red@lunatea>
*/

#include "containers/box_container.h"
#include "widgets/list.h"
#include "bundles/color_picker.h"
#include "bundles/file_dialog.h"
#include "bundles/input_dialog.h"
#include "variant.h"
#include "iapi.h"

class EditorDB;

class EditorVariant : public GUI::VBoxContainer {

	Variant local;
	IAPI::PropertyHint hint;

	void array_item_edited_callback(int p_row,String p_item);
	void color_edited_callback();
	
	GUI::Frame *base;
	
	void vector_set_x(String p_x);
	void vector_set_y(String p_y);
	void vector_set_z(String p_z);

	void plane_set_nx(String p_x);
	void plane_set_ny(String p_y);
	void plane_set_nz(String p_z);
	void plane_set_d(String p_d);

	void matrix_set(String,int p_row,int p_col);

	void file_opened(String p_file);
	void file_dialog_show();

	void flag_toggled(bool,int p_which);

	GUI::List *array_edit;
	GUI::ColorPicker *color_picker;
	GUI::FileDialog *fdialog;
	GUI::OptionInputDialog * iapi_option;
	EditorDB *edb;
	GUI::Window *edb_win;
			
	void iobj_create();
	void iobj_load();
	void iobj_load_callbak(String p_path);
	void iobj_clear();
	
public:

	Signal< Method1<const Variant&> > changed_signal;	
	void set_variant( const Variant& p_variant, const IAPI::PropertyHint& p_hint=IAPI::PropertyHint() );
	
	EditorVariant();
	~EditorVariant();

};

#endif
