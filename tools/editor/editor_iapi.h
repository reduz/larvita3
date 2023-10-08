//
// C++ Interface: editor_iapi
//
// Description:
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EDITOR_IAPI_H
#define EDITOR_IAPI_H

#include "widgets/tree.h"
#include "containers/box_container.h"
#include "bundles/menu_box.h"
#include "bundles/file_dialog.h"
#include "bundles/message_box.h"
#include "bundles/scroll_box.h"
#include "scene/nodes/node.h"
#include "editor/editor_variant.h"
#include "bundles/input_dialog.h"
#include "bundles/window_box.h"
#include "widgets/window_top.h"
#include "base/window.h"
#include "editor/editor_db.h"

/**
	@author ,,, <red@lunatea>
*/

class EditorCreationParams;


class EditorIAPI  : public GUI::VBoxContainer {

	enum MenuOption {

		OPTION_FILE_NEW,
		OPTION_FILE_OPEN,
		OPTION_FILE_SAVE,
		OPTION_FILE_SAVE_AS,
		OPTION_FILE_IMPORT,
		OPTION_FILE_QUIT,
	};

	class EditPopup : public GUI::Window {

		EditorVariant *variant_editor;
		WeakRef<IAPI> iapi;
		String path;
		GUI::TreeItem *item;

		void variant_changed(const Variant& p_variant);
	public:
		void edit(IAPIRef p_iapi,String p_path,GUI::TreeItem *p_item,GUI::Rect p_rect);

		EditPopup(Window *p_parent);
	};

	GUI::ScrollBox *scroll;
	GUI::Tree *tree;

	GUI::MessageBox *message;
	GUI::FileDialog *file_dialog;

	GUI::MenuBox *method_box;

	EditPopup *edit_popup;

	void edit_iapi_callback( IAPIRef p_callback );
	void item_iapi_edited(int col, String p_path);
	void method_call_slot( int p_idx );

	void item_edited(int col, GUI::TreeItem *item, String p_path,Variant::Type p_type);
	void item_popup_edited(int col, GUI::Rect p_rect,GUI::TreeItem *item, String p_path,Variant::Type p_type);

	DVector<IAPI::MethodInfo> methods;
	int edited_method;

	void update_methods();
	void update_tree();
	void set_in_window();

	void file_access_activated_callback(String p_string);
	void menu_callback(int);
	void new_iapi_callback(int,String p_type);

	IAPIRef iapi; // iapi being edited
	String edited_prop_path; // path of property currently being edited
	GUI::OptionInputDialog *iapi_select_dialog; // iapi to create (choos)
	bool no_fileops; // don't create fileops top menu
	/* stuff for editing creation params / method arguments */
	GUI::WindowBox * ecparams_wb;
	GUI::Window *ecparams_win;
	EditorCreationParams *ecparams;
	String ecparams_type;
	void create_with_params(const IAPI::CreationParams& p_params);

	/* window for database ops (open/save) */
	EditorDB *db_access;
	GUI::WindowBox *db_wb;
	GUI::Window *db_win;


	void create_ecparams();
	void iapi_changed_callback(const String&);
	void reload();
public:

	Signal< Method1<IAPIRef> > edit_iapi_signal;

	void set_iapi( IAPIRef p_iapi );

	EditorIAPI(bool p_no_fileops=false);
	~EditorIAPI();

};

#endif
