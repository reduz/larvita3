//
// C++ Interface: tree
//
// Description:
//
//
// Author: red,,, <red@hororo>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef GUITREE_H
#define GUITREE_H


#include "base/widget.h"
#include "base/range.h"
#include "widgets/line_edit.h"
#include "widgets/slider.h"
#include "bundles/popup_menu.h"

namespace GUI {

/**
	@author red,,, <red@hororo>
*/
class Tree;

enum TreeCellMode {
	CELL_MODE_STRING, ///< just a string
	CELL_MODE_CHECK, ///< string + check
	CELL_MODE_RANGE, ///< Contains a range
	CELL_MODE_BITMAP, ///< Contains a bitmap, not editable
	CELL_MODE_CUSTOM, ///< Contains a custom value, show a string, and an edit button
};

class TreeItem {
public:

private:
friend class Tree;


	struct Cell {

		TreeCellMode mode;
		union _Data {
			BitmapID bitmap;
			bool checked;
			RangeBase *range;
		};
		_Data data;
		String string;
		Color color;
		bool custom_color;
		bool owned_range;
		bool selected;
		bool selectable;
		bool editable;

		Cell() {

			mode=CELL_MODE_STRING;
			data.bitmap=INVALID_BITMAP_ID;
			owned_range=false;
			custom_color=false;
			selected=false;
			selectable=true;
			editable=false;
		}

		~Cell() {

			if (owned_range && mode==CELL_MODE_RANGE && data.range)
				GUI_DELETE( data.range );
		}
	};

	Cell *cells; // tree->columns to know the amount of cells

	bool collapsed; // wont show childs

	TreeItem *parent; // parent item
	TreeItem *next; // next in list
	TreeItem *childs; //child items
	Tree *tree; //tree (for reference)

	TreeItem(Tree *p_tree);



public:

	Signal< Method1<int> > double_click_signal;
	Signal< Method1<int> > selected_signal;
	Signal< Method1<int> > deselected_signal;
	Signal< Method1<int> > changed_signal; ///< Called when changing editability/color/etc
	Signal< Method1<int> > edited_signal; ///< Called when changing contents
	Signal< Method2<int, Rect> > custom_popup_signal;
	Signal<> collapsed_signal;

	/* cell mode */
	void set_cell_mode( int p_column, TreeCellMode p_mode );
	TreeCellMode get_cell_mode( int p_column );

	/* check mode */
	void set_checked(int p_column,bool p_checked);
	bool is_checked(int p_column);

	void set_text(int p_column,String p_text);
	String get_text(int p_column);

	void set_bitmap(int p_column,BitmapID p_bitmap);
	BitmapID get_bitmap(int p_column);

	/* range works for mode number or mode combo */
	void set_range(int p_column,RangeBase *p_range,bool p_own_range=false);
	RangeBase *get_range(int p_column);

	void set_collapsed(bool p_collapsed);

	//String get_text();
	//BitmapID get_bitmap();
	bool is_collapsed();

	TreeItem *get_next();
	TreeItem *get_parent();
	TreeItem *get_childs();



	void remove_child(TreeItem *p_item);

	void set_selectable(int p_column,bool p_selectable);

	bool is_selected(int p_column);
	void select(int p_column);
	void deselect(int p_column);

	void set_editable(int p_column,bool p_editable);
	bool is_editable(int p_column);
	void set_custom_color(int p_column,const Color& p_color);
	void clear_custom_color(int p_column);

	~TreeItem();
};


class Tree : public Widget {
public:

	enum ColumnMode {
		COLUMN_EXPAND=-1 ///< Column expands
	};
	enum SelectMode {
            SELECT_SINGLE,
            SELECT_ROW,
            SELECT_MULTI
	};

private:


	void item_changed(int p_column,TreeItem *p_item);
	void item_range_changed(double p_value, TreeItem* p_item, int p_column);

	int get_item_height(TreeItem *root);
	void tree_changed_callback();
	Size get_minimum_size_internal();

	int draw_item(const Point& p_pos,const Rect& p_exposed,TreeItem *p_item);
	virtual void draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed);

	int compute_item_height(TreeItem *p_item);
	int propagate_mouse_event(const Point &p_pos,int x_ofs,int y_ofs,bool p_doubleclick,TreeItem *p_item,int p_button,int p_mod_state); //return true on handled
	virtual void mouse_button(const Point& p_pos, int p_button,bool p_press,int p_modifier_mask);
	virtual void mouse_doubleclick(const Point& p_pos,int p_modifier_mask);


	void item_selected(int p_column,TreeItem *p_item);
	void item_deselected(int p_column,TreeItem *p_item);

	void select_single_item(TreeItem *p_selected,TreeItem *p_current,int col);

	int *column_min_size;
	bool *column_expand;
	int columns;

	int* column_width_caches;
	void compute_column_size_caches();


	Window *line_edit_window;
	LineEdit *line_edit;
	HSlider* slider;
	PopUpMenu *popup_menu;

	TreeItem *popup_edited_item;

    bool hide_root;
    SelectMode select_mode;

	int popup_edited_item_col;
	void popup_select_slot(int p_option);
	void line_edit_enter_slot(String p_text);

	void set_in_window();
	void resize(const Size& p_new_size);

	void draw_item_text(String p_text,BitmapID p_bitmap,bool p_tool,Rect p_rect,Color p_color);

friend class TreeItem;

	TreeItem *root;

public:

	void clear();

	TreeItem *create_item(TreeItem *p_parent=0); /// create item as toplevel (replacing it if it exists), or alternatively, create item with another parent

	TreeItem* get_root_item();

	void set_column_min_width(int p_column,int p_min_width);
	void set_column_expand(int p_column,bool p_expand);
	
	int get_column_width(int p_column) const;

	void set_hide_root(bool p_eanbled);

	bool get_selected( TreeItem** p_item, int *p_col=0);

	void set_select_mode(SelectMode p_mode);
	Tree(int p_columns=1);
	~Tree();
};

}


#endif
