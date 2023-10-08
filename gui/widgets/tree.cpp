//Tre
// C++ Implementation: tree
//
// Description:
//
//
// Author: red,,, <red@hororo>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "tree.h"
#include "base/painter.h"
#include "base/skin.h"
#include "containers/box_container.h"
#include <stdio.h>

namespace GUI {


/* cell mode */
void TreeItem::set_cell_mode( int p_column, TreeCellMode p_mode ) {

	if (p_column<0 || p_column >=tree->columns)
		return;

	Cell &c = cells[ p_column ];

	if (c.mode == p_mode )
		return;
	if (c.mode == CELL_MODE_RANGE && c.data.range && c.owned_range) {

		GUI_DELETE( c.data.range );
		c.data.range=NULL;
		c.owned_range=false;
	}

	switch(p_mode) {
		case CELL_MODE_RANGE: {

			c.data.range = GUI_NEW( Range );
			c.owned_range=true;
			c.string = "";
		} break;
		case CELL_MODE_CHECK : {
			c.data.checked=false;
		} break;
		case CELL_MODE_BITMAP: {
			c.data.bitmap=INVALID_BITMAP_ID;
		} break;
		default:{ c.data.bitmap=INVALID_BITMAP_ID; }
	}

	c.mode = p_mode;

	changed_signal.call(p_column);
}

TreeCellMode TreeItem::get_cell_mode( int p_column ) {

	if (p_column<0 || p_column >=tree->columns)
		return CELL_MODE_STRING;

	return cells[ p_column ].mode;

}

/* check mode */
void TreeItem::set_checked(int p_column,bool p_checked) {

	if (p_column<0 || p_column >=tree->columns)
		return;

	Cell &c = cells[ p_column ];

	if (c.mode != CELL_MODE_CHECK )
		return;

	c.data.checked=p_checked;

	changed_signal.call(p_column);

}
bool TreeItem::is_checked(int p_column) {

	if (p_column<0 || p_column >=tree->columns)
		return false;

	Cell &c = cells[ p_column ];

	return c.data.checked;

}

void TreeItem::set_text(int p_column,String p_text) {

	if (p_column<0 || p_column >=tree->columns)
		return;

	Cell &c = cells[ p_column ];

	c.string=p_text;
	changed_signal.call(p_column);

}
String TreeItem::get_text(int p_column) {

	if (p_column<0 || p_column >=tree->columns)
		return "";
	Cell &c = cells[ p_column ];
	return c.string;
}

void TreeItem::set_bitmap(int p_column,BitmapID p_bitmap) {

	if (p_column<0 || p_column >=tree->columns)
		return;

	Cell &c = cells[ p_column ];

	if (c.mode!=CELL_MODE_STRING && c.mode!=CELL_MODE_BITMAP && c.mode!=CELL_MODE_CUSTOM )
		return;

	c.data.bitmap=p_bitmap;

	changed_signal.call(p_column);
}
BitmapID TreeItem::get_bitmap(int p_column) {

	if (p_column<0 || p_column >=tree->columns)
		return INVALID_BITMAP_ID;

	Cell &c = cells[ p_column ];

	if (c.mode!=CELL_MODE_STRING && c.mode!=CELL_MODE_BITMAP && c.mode!=CELL_MODE_CUSTOM )
		return INVALID_BITMAP_ID;

	return c.data.bitmap;
}

/* range works for mode number or mode combo */
void TreeItem::set_range(int p_column,RangeBase *p_range,bool p_own_range)  {

	if (p_column<0 || p_column >=tree->columns)
		return;

	Cell &c = cells[ p_column ];

	if (c.mode!=CELL_MODE_RANGE)
		return;

	if (c.owned_range && c.data.range)
		GUI_DELETE( c.data.range );
	c.data.range=p_range;
	c.owned_range=p_own_range;
	
	changed_signal.call(p_column);
}

RangeBase *TreeItem::get_range(int p_column)   {

	if (p_column<0 || p_column >=tree->columns)
		return NULL;

	Cell &c = cells[ p_column ];

	if (c.mode!=CELL_MODE_RANGE)
		return NULL;

	return c.data.range;
}

void TreeItem::set_collapsed(bool p_collapsed) {

	if (p_collapsed==collapsed)
		return;
	collapsed=p_collapsed;
	collapsed_signal.call();

	tree->check_minimum_size();
	tree->update();
}

bool TreeItem::is_selected(int p_column) {

	if (p_column<0 || p_column >=tree->columns)
		return false;

	Cell &c = cells[ p_column ];

	return c.selected;

}

bool TreeItem::is_collapsed() {


	return collapsed;

}

TreeItem *TreeItem::get_next() {


	return next;
}

TreeItem *TreeItem::get_parent() {

	return parent;
}
TreeItem *TreeItem::get_childs() {


	return childs;
}



void TreeItem::remove_child(TreeItem *p_item) {

	TreeItem **c=&childs;

	while (*c) {

		if ( (*c) == p_item ) {

			TreeItem *aux = *c;

			*c=(*c)->next;

			aux->parent = 0;
			return;
		}

		c=&(*c)->next;
	}

	PRINT_ERROR("Unable to delete child (unexistent)");

}

void TreeItem::select(int p_column) {

	if (p_column<0 || p_column >=tree->columns)
		return;

	Cell &c = cells[ p_column ];

	if (!c.selectable)
		return;

    switch( tree->select_mode ) {

        case Tree::SELECT_SINGLE: {

            tree->select_single_item(this,tree->root,p_column);

        } break;
        default: {
            c.selected=true;
            selected_signal.call( p_column );
        } break;
	}
}

void TreeItem::deselect(int p_column) {

	if (p_column<0 || p_column >=tree->columns)
		return;

	Cell &c = cells[ p_column ];

	if (!c.selectable)
		return;

	c.selected=false;

	deselected_signal.call( p_column );
}

void TreeItem::set_selectable(int p_column,bool p_selectable) {

	if (p_column<0 || p_column >=tree->columns)
		return;

	Cell &c = cells[ p_column ];

	c.selectable=p_selectable;
		return;

}

void TreeItem::set_custom_color(int p_column,const Color& p_color) {

	if (p_column<0 || p_column >=tree->columns)
		return;

	Cell &c = cells[ p_column ];

	c.custom_color=true;
	c.color=p_color;

	changed_signal.call(p_column);

}

void TreeItem::clear_custom_color(int p_column) {

	if (p_column<0 || p_column >=tree->columns)
		return;

	Cell &c = cells[ p_column ];

	c.custom_color=false;

	changed_signal.call(p_column);

}

void TreeItem::set_editable(int p_column,bool p_editable) {

	if (p_column<0 || p_column >=tree->columns)
		return;
	Cell &c = cells[ p_column ];

	c.editable=p_editable;
	changed_signal.call(p_column);

}
bool TreeItem::is_editable(int p_column) {

	if (p_column<0 || p_column >=tree->columns)
		return false;
	Cell &c = cells[ p_column ];

	return c.editable;
}

TreeItem::TreeItem(Tree *p_tree) {

	tree=p_tree;
	cells = GUI_NEW_ARRAY( Cell, tree->columns );
	collapsed=false;

	parent=0; // parent item
	next=0; // next in list
	childs=0; //child items


}

TreeItem::~TreeItem() {

	TreeItem *c=childs;
	while (c) {

		TreeItem *aux=c;
		c=c->get_next();
		aux->parent=0; // so it wont try to recursively autoremove from me in here
		GUI_DELETE( aux );
	}

	if (parent)
		parent->remove_child(this);

	if (tree && tree->root==this) {

		tree->root=0;
	}

	if (tree && tree->popup_edited_item==this)
		tree->popup_edited_item=NULL;

	GUI_DELETE_ARRAY( cells );
}

// -=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=


int Tree::get_item_height(TreeItem *p_item) {

	int height=compute_item_height(p_item);
	height+=constant( C_TREE_VSPACING );

	if (!p_item->collapsed) { /* if not collapsed, check the childs */

		TreeItem *c=p_item->childs;

		while (c) {

			height += get_item_height( c );

			c=c->next;
		}
	}

	return height;
}

void Tree::draw_item_text(String p_text,BitmapID p_bitmap,bool p_tool,Rect p_rect,Color p_color) {


	if (p_bitmap!=INVALID_BITMAP_ID) {
		Size bmsize = get_painter()->get_bitmap_size(p_bitmap);
		get_painter()->draw_bitmap(p_bitmap, p_rect.pos + Size(0,(p_rect.size.y-bmsize.y)/2));
		p_rect.pos.x+=bmsize.x+constant( C_TREE_HSPACING );
		p_rect.size.x-=bmsize.x+constant( C_TREE_HSPACING );

	}

	if (p_tool)
		p_rect.size.x-=p_rect.size.y/2;

	p_rect.pos.y+=(p_rect.size.y-get_painter()->get_font_height( font(FONT_TREE) ))/2 +get_painter()->get_font_ascent( font(FONT_TREE) );
	get_painter()->draw_text( font(FONT_TREE), p_rect.pos, p_text, p_color,p_rect.size.x );

}

int Tree::draw_item(const Point& p_pos,const Rect& p_exposed,TreeItem *p_item) { //return height


	//   printf("p_x is %i p_x is %i\n",p_pos.x,p_pos.y);

	if (p_pos.y > (p_exposed.pos.y + p_exposed.size.y))
		return -1; //draw no more!

	Point pos=p_pos;

	int htotal=0;

	int label_h=compute_item_height( p_item );

	/* Calculate height of the label part */


	label_h+=constant( C_TREE_VSPACING );

	/* Draw label, if height fits */

	Point guide_from;

    	bool skip=(p_item==root && hide_root);
   // printf("skip (%p == %p && %i) %i\n",p_item,root,hide_root,skip);

	if (!skip && (pos.y+label_h)>p_exposed.pos.y) {

      //  printf("entering\n");
		int height=label_h;

		Point guide_space=Point( constant( C_TREE_GUIDE_WIDTH ) , height );

		if (p_item->childs) { //has childs, draw the guide box

			Point guide_box_size = Point( constant(C_TREE_GUIDE_BOX_SIZE),constant(C_TREE_GUIDE_BOX_SIZE));

			Direction dir;

			if (p_item->collapsed) {

				dir=DOWN;
			} else {
				dir=RIGHT;

			}

			get_painter()->draw_arrow(pos+((guide_space-guide_box_size)/2),guide_box_size ,dir,color(COLOR_TREE_GUIDES));


		}

		//draw separation.
//		if (p_item->get_parent()!=root || !hide_root)
		pos.x+=constant( C_TREE_GUIDE_WIDTH );

		int font_ascent=get_painter()->get_font_ascent( font( FONT_TREE ) );

		int ofs = pos.x;
		for (int i=0;i<columns;i++) {

			int w = get_column_width(i);

			if (i==0) {

                if (p_item->cells[0].selected && select_mode==SELECT_ROW) {
                    Rect row_rect = Rect( Point( 0, p_pos.y ), Size( get_size_cache().width, label_h ));
                    get_painter()->draw_stylebox(stylebox( SB_TREE_SELECTED ),row_rect.pos,row_rect.size );
                }

				w-=pos.x;
				if (w<=0) {

					ofs=get_column_width(0);
					continue;
				}
			}

			ofs+=constant( C_TREE_HSPACING );
			w-=constant( C_TREE_HSPACING );

			Rect item_rect = Rect( Point( ofs, pos.y ), Size( w, label_h ));

			if (p_item->cells[i].selected && select_mode!=SELECT_ROW) {

				get_painter()->draw_stylebox(stylebox( SB_TREE_SELECTED ),item_rect.pos,item_rect.size );
			}

			get_painter()->push_clip_rect( item_rect );

			Color col=p_item->cells[i].custom_color?p_item->cells[i].color:color( p_item->cells[i].selected?COLOR_TREE_FONT_SELECTED:COLOR_TREE_FONT );
			Point text_pos=item_rect.pos;
			text_pos.y+=(item_rect.size.y-get_painter()->get_font_height( font(FONT_TREE)))/2 + font_ascent;

			switch (p_item->cells[i].mode) {

				case CELL_MODE_STRING: {

					draw_item_text(p_item->cells[i].string,p_item->cells[i].data.bitmap,false,item_rect,col);
				} break;
				case CELL_MODE_CHECK: {

					Point check_ofs=item_rect.pos;
					check_ofs.y+=(item_rect.size.y-constant(C_TREE_CHECK_SIZE))/2;

					if (p_item->cells[i].data.checked) {

						get_painter()->draw_rect(check_ofs,Size(constant(C_TREE_CHECK_SIZE),constant(C_TREE_CHECK_SIZE)),color(COLOR_TREE_GUIDES));
						get_painter()->draw_fill_rect(check_ofs+Point(2,2),Size(constant(C_TREE_CHECK_SIZE),constant(C_TREE_CHECK_SIZE))-Size(4,4),color(COLOR_TREE_GUIDES));

					} else {
						get_painter()->draw_rect(check_ofs,Size(constant(C_TREE_CHECK_SIZE),constant(C_TREE_CHECK_SIZE)),color(COLOR_TREE_GUIDES));

					}

					int check_w = constant(C_TREE_CHECK_SIZE)+constant( C_TREE_HSPACING );

					text_pos.x+=check_w;

					
					get_painter()->draw_text( font(FONT_TREE), text_pos, p_item->cells[i].string, col,item_rect.size.x-check_w );

				} break;
				case CELL_MODE_RANGE: {

					if (!p_item->cells[i].data.range)
						break;
					if (p_item->cells[i].string!="") {

						if (!p_item->cells[i].editable)
							break;

						int option = (int)p_item->cells[i].data.range->get();

						String s = p_item->cells[i].string;
						s=s.get_slice(",",option);

						int arrow_w = item_rect.size.y/2;

						get_painter()->draw_text( font(FONT_TREE), text_pos, s, col,item_rect.size.x-arrow_w );

						//?
						Point arrow_pos=item_rect.pos;
						arrow_pos.x+=item_rect.size.x-arrow_w;

						get_painter()->draw_arrow( arrow_pos,Size(arrow_w,item_rect.size.y), DOWN, color(COLOR_TREE_GUIDES));

					} else {

						RangeBase *r=p_item->cells[i].data.range;
						if (!r)
							break;

						int updown_w = item_rect.size.y/2;

						get_painter()->draw_text( font(FONT_TREE), text_pos, r->get_as_text(), col,item_rect.size.x- (updown_w * 2) );

						if (!p_item->cells[i].editable)
							break;

						Point updown_pos=item_rect.pos;
						updown_pos.x+=item_rect.size.x-updown_w;

						get_painter()->draw_arrow( updown_pos,Size(updown_w,updown_w), UP, color(COLOR_TREE_GUIDES));
						get_painter()->draw_arrow( updown_pos+Point(0,updown_w),Size(updown_w,updown_w), DOWN, color(COLOR_TREE_GUIDES));

						// slider arrow
						Point arrow_pos = item_rect.pos;
						arrow_pos.x += item_rect.size.x - updown_w * 2;
						int arrow_w = item_rect.size.y/2;
						get_painter()->draw_arrow( arrow_pos, Size(arrow_w, item_rect.size.y), DOWN, color(COLOR_TREE_GUIDES));

					}

				} break;
				case CELL_MODE_BITMAP: {

					Size bitmap_size = get_painter()->get_bitmap_size( p_item->cells[i].data.bitmap);

					Point bitmap_ofs = (item_rect.size-bitmap_size)/2;
					bitmap_ofs+=item_rect.pos;

					get_painter()->draw_bitmap( p_item->cells[i].data.bitmap, bitmap_ofs, col );

				} break;
				case CELL_MODE_CUSTOM: {

					int arrow_w = item_rect.size.y/2;

					draw_item_text(p_item->cells[i].string,p_item->cells[i].data.bitmap,p_item->cells[i].editable,item_rect,col);

					if (!p_item->cells[i].editable)
						break;

					Point arrow_pos=item_rect.pos;
					arrow_pos.x+=item_rect.size.x-arrow_w;

					get_painter()->draw_arrow( arrow_pos,Size(arrow_w,item_rect.size.y), DOWN, color(COLOR_TREE_GUIDES));

				} break;
			}

			get_painter()->pop_clip_rect();

			if (i==0) {

				ofs=get_column_width(0);
			} else {

				ofs+=w;
			}

		}
		//separator
		//get_painter()->draw_fill_rect( Point(0,pos.y),Size(get_size_cache().width,1),color( COLOR_TREE_GRID) );

		pos=p_pos; //reset pos

	}


	if (!skip) {
		pos.x+=constant( C_TREE_GUIDE_WIDTH );
		htotal+=label_h;
		pos.y+=htotal;
     	   
	}


	if (!p_item->collapsed) { /* if not collapsed, check the childs */

		TreeItem *c=p_item->childs;

		while (c) {

			int child_h=draw_item(pos, p_exposed, c );

			if (child_h<0)
				return -1; // break, stop drawing, no need to anymore

			htotal+=child_h;
			pos.y+=child_h;
			c=c->next;
		}
	}


	return htotal;


}

int Tree::compute_item_height(TreeItem *p_item) {

    if (p_item==root && hide_root)
        return 0;
	int height=get_painter()->get_font_height( font( FONT_TREE) );

	for (int i=0;i<columns;i++) {

		switch(p_item->cells[i].mode) {

			case CELL_MODE_CHECK: {


				if (height<constant(C_TREE_CHECK_SIZE))
					height=constant(C_TREE_CHECK_SIZE);



			} break;
			case CELL_MODE_BITMAP: {

				BitmapID bitmap = p_item->cells[i].data.bitmap;
				if (bitmap!=INVALID_BITMAP_ID) {

					Size  s = get_painter()->get_bitmap_size( bitmap );
					if (s.height > height )
						height=s.height;
				}

			} break;
			default: {}
		}
	}


	height += constant( C_TREE_VSPACING );

	return height;

}

int Tree::propagate_mouse_event(const Point &p_pos,int x_ofs,int y_ofs,bool p_doubleclick,TreeItem *p_item,int p_button,int p_mod_state) {

	int item_h=compute_item_height( p_item )+constant( C_TREE_VSPACING );

	bool skip=(p_item==root && hide_root);
	

	if (!skip && p_pos.y<item_h) {
		// check event!

		if (p_pos.x >=0 && p_pos.x < (constant(C_TREE_GUIDE_WIDTH)) ) {


			if (p_item->childs)
				p_item->set_collapsed( ! p_item->is_collapsed() );

			return -1; //handled!
		}
		
		int x=p_pos.x+x_ofs;
		/* find clicked column */
		int col=-1;
		int col_ofs=0;
		for (int i=0;i<columns;i++) {

			int cw=get_column_width(i);
			if (x>cw) {
				col_ofs+=cw;
				x-=cw;
				continue;
			}

			col=i;
			break;
		}

		

		if (col==-1)
			return -1;
		else if (col==0)
			x=p_pos.x-constant(C_TREE_GUIDE_WIDTH)-constant(C_TREE_HSPACING);;

		TreeItem::Cell &c = p_item->cells[col];


		bool already_selected=c.selected;

		if (p_button==1) {
			/* process selection */

			if (p_doubleclick) {

				p_item->double_click_signal.call(col);
				return -1;
			}

			if (select_mode==SELECT_MULTI && p_mod_state&KEY_MASK_CTRL && c.selectable) {

				if (!c.selected) {

					c.selected=true;
					p_item->selected_signal.call(col);
				} else {

					c.selected=false;
					p_item->deselected_signal.call(col);
				}

				update();
			} else {

				if (c.selectable)
					select_single_item( p_item, root, col );
			}
		}

		if (!c.editable)
			return -1; // if cell is not editable, don't bother

		/* editing */

		bool bring_up_editor=c.selected && already_selected;
		String editor_text=c.string;

		switch (c.mode) {

			case CELL_MODE_STRING: {
				//nothing in particular
			} break;
			case CELL_MODE_CHECK: {

				bring_up_editor=false; //checkboxes are not edited with editor
				x-=constant(C_TREE_HSPACING);
				if (x>=0 && x<= constant(C_TREE_CHECK_SIZE) ) {

					c.data.checked = !c.data.checked;
					p_item->edited_signal.call(col);

				}

			} break;
			case CELL_MODE_RANGE: {

				if (!c.data.range) {
					bring_up_editor=false;
					break;
				}

				if (c.string!="") {

					//if (x >= (get_column_width(col)-item_h/2)) {

					popup_menu->clear();
					for (int i=0;i<c.string.get_slice_count(",");i++) {

						String s = c.string.get_slice(",",i);
						popup_menu->add_item(s,i);

					}

					popup_menu->set_size(Size(get_column_width(col),0));
					popup_menu->popup( get_global_pos() + Point(col_ofs,y_ofs+item_h));
					popup_edited_item=p_item;
					popup_edited_item_col=col;
					//}
					bring_up_editor=false;
				} else {
					
					if (x >= (get_column_width(col)-item_h/2)) {
					
						/* touching the combo */
						bool up=p_pos.y < (item_h /2);
	
						if (p_button==BUTTON_LEFT) {
							c.data.range->set( c.data.range->get() + (up?1.0:-1.0) * c.data.range->get_step() );
						} else if (p_button==BUTTON_RIGHT) {
	
							c.data.range->set( up?c.data.range->get_max():c.data.range->get_min()  );
						} else if (p_button==BUTTON_WHEEL_UP) {
	
							c.data.range->set( c.data.range->get() +  c.data.range->get_step() );
	
						} else if (p_button==BUTTON_WHEEL_DOWN) {
	
							c.data.range->set( c.data.range->get() -  c.data.range->get_step() );
	
						}
	
						p_item->edited_signal.call(col);
	
						bring_up_editor=false;

					} else if (x >= (get_column_width(col)- ( item_h/2 * 2))) {
						// scroll bar here
						
						//printf("scroll bar popup!\n");
						slider->set_range(c.data.range);
						line_edit->hide();
						slider->show();
						line_edit_window->set_pos(get_global_pos() + Point(col_ofs,y_ofs + item_h) );
						line_edit_window->set_size( Size(get_column_width(col),0));
						line_edit_window->show();
						
						bring_up_editor=false;

					} else {

						editor_text=c.data.range->get_as_text();
					};	

				}

			} break;
			case CELL_MODE_BITMAP: {
				bring_up_editor=false;
			} break;
			case CELL_MODE_CUSTOM: {
				if (x >= (get_column_width(col)-item_h/2)) {
					p_item->custom_popup_signal.call(col, Rect(get_global_pos() + Point(col_ofs,y_ofs+item_h), Size(get_column_width(col),item_h)));
				} else {
					if (already_selected)
						p_item->edited_signal.call(col);
				};
				bring_up_editor=false;
				return -1;
			} break;

		};

		if (!bring_up_editor || p_button!=BUTTON_LEFT)
			return -1;

		popup_edited_item=p_item;
		popup_edited_item_col=col;
		line_edit_window->set_pos(get_global_pos() + Point(col_ofs,y_ofs) );
		line_edit_window->set_size( Size(get_column_width(col),item_h));
		line_edit->clear();
		line_edit->set_text( editor_text );
		slider->hide();
		line_edit->show();
		line_edit_window->show();

		return -1; //select
	} else {

		Point new_pos=p_pos;
		if (!skip) {
			x_ofs+=constant( C_TREE_GUIDE_WIDTH );
			new_pos.x-=constant( C_TREE_GUIDE_WIDTH );
			y_ofs+=item_h;
			new_pos.y-=item_h;			
		}
		

		if (!p_item->collapsed) { /* if not collapsed, check the childs */

			TreeItem *c=p_item->childs;

			while (c) {

				int child_h=propagate_mouse_event( new_pos,x_ofs,y_ofs,p_doubleclick,c,p_button,p_mod_state);

				if (child_h<0)
					return -1; // break, stop propagating, no need to anymore

				new_pos.y-=child_h;
				y_ofs+=child_h;
				c=c->next;
				item_h+=child_h;
			}
		}



	}

	return item_h; // nothing found

}

void Tree::line_edit_enter_slot(String p_text) {


	line_edit_window->hide();

	if (!popup_edited_item)
		return;

	if (popup_edited_item_col<0 || popup_edited_item_col>columns)
		return;

	TreeItem::Cell &c=popup_edited_item->cells[popup_edited_item_col];
	switch( c.mode ) {

		case CELL_MODE_STRING: {

			c.string=p_text;
			popup_edited_item->edited_signal.call( popup_edited_item_col );
		} break;
		case CELL_MODE_RANGE: {

			if (!c.data.range)
				break;

			c.data.range->set( p_text.to_double() );
			popup_edited_item->edited_signal.call( popup_edited_item_col );
		} break;
		default: { PRINT_ERROR("INVALID MODE"); }
	}

}
void Tree::popup_select_slot(int p_option) {

	if (!popup_edited_item)
		return;

	if (popup_edited_item_col<0 || popup_edited_item_col>columns)
		return;

	if (!popup_edited_item->cells[popup_edited_item_col].data.range)
		return;
	popup_edited_item->cells[popup_edited_item_col].data.range->set( p_option );
	popup_edited_item->edited_signal.call( popup_edited_item_col );
	update();
}


void Tree::mouse_doubleclick(const Point& p_pos,int p_modifier_mask) {

	if (!root)
		return;

	Point pos = p_pos - get_painter()->get_stylebox_offset( stylebox( SB_TREE_NORMAL ) );
	propagate_mouse_event(pos,0,0,true,root,BUTTON_LEFT,p_modifier_mask);

}

void Tree::mouse_button(const Point& p_pos, int p_button,bool p_press,int p_modifier_mask) {


	if (!p_press)
		return;

	if (!root)
		return;

	if (p_button==BUTTON_WHEEL_UP || p_button==BUTTON_WHEEL_DOWN) {
		return;
	};

	Point pos = p_pos - get_painter()->get_stylebox_offset( stylebox( SB_TREE_NORMAL ) );
	propagate_mouse_event(pos,0,0,false,root,p_button,p_modifier_mask);

}

void Tree::draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed) {


	get_painter()->draw_stylebox( stylebox( SB_TREE_NORMAL ), Point() , p_size, p_exposed );
	if (has_focus())
		get_painter()->draw_stylebox( stylebox( SB_TREE_FOCUS ), Point() , p_size, p_exposed);

	if (!root)
		return;

	draw_item(get_painter()->get_stylebox_offset( stylebox( SB_TREE_FOCUS ) ) , p_exposed,root);

	int ofs=0;
	int from_y=p_exposed.pos.y+get_painter()->get_stylebox_margin( stylebox(SB_TREE_NORMAL), MARGIN_TOP);
	int size_y=p_exposed.size.height-get_painter()->get_stylebox_min_size(stylebox(SB_TREE_NORMAL)).height;


	for (int i=0;i<(columns-1);i++) {

		ofs+=get_column_width(i);
		get_painter()->draw_fill_rect( Point(ofs+constant( C_TREE_HSPACING )/2, from_y), Size( 1, size_y ),color( COLOR_TREE_GRID) );
	}
}


void Tree::resize(const Size& p_new_size) {

	int available=p_new_size.x;
	int expanding_count=0;

	for (int i=0;i<columns;i++) {

		available-=column_min_size[i];
		if (column_expand[i])
			expanding_count++;
	}

	if (available<0) {

		PRINT_ERROR("aviable space less than needed");
		return;
	}

	for (int i=0;i<columns;i++) {

		int new_size=column_min_size[i];
		if (column_expand[i])
			new_size+=available/expanding_count;

		column_width_caches[i]=new_size;
	}
}

Size Tree::get_minimum_size_internal() {

	int w=0;

	for (int i=0;i<columns;i++) {

		w+=column_min_size[i];
	}

	int h = root?get_item_height(root):0;

	return Size(w,h);
}

void Tree::tree_changed_callback() {

	update();
}



TreeItem *Tree::create_item(TreeItem *p_parent) {

	TreeItem *ti = GUI_NEW( TreeItem(this) );

	if (!ti) {

		PRINT_ERROR("Can't create new TreeItem");
		return 0;
	}

	if (p_parent) {

		/* Always append at the end */

		TreeItem *last=0;
		TreeItem *c=p_parent->childs;

		while(c) {

			last=c;
			c=c->next;
		}

		if (last) {

			last->next=ti;
		} else {

			p_parent->childs=ti;
		}
		ti->parent=p_parent;

	} else {

		if (root)
			ti->childs=root;

		root=ti;

	}

	ti->changed_signal.connect( Method1<int>(Method2<int,TreeItem*>(this,&Tree::item_changed),ti));

	ti->selected_signal.connect( Method1<int>(Method2<int,TreeItem*>(this,&Tree::item_selected),ti));

	ti->deselected_signal.connect( Method1<int>(Method2<int,TreeItem*>(this,&Tree::item_deselected),ti));

	return ti;

	check_minimum_size();
}

TreeItem* Tree::get_root_item() {

	return root;
};

void Tree::item_range_changed(double p_value, TreeItem* p_item, int p_column) {
	
	p_item->edited_signal.call( p_column );
};

void Tree::item_changed(int p_column,TreeItem *p_item) {

	if (p_item->get_cell_mode(p_column) == CELL_MODE_RANGE) {
		
		RangeBase* range = p_item->get_range(p_column);
		range->value_changed_signal.connect( 
			Method1<double>( Method2<double, TreeItem*>( 
				Method3<double, TreeItem*, int>( this, &Tree::item_range_changed ), p_column), p_item) );
	};
	
	update(); //just redraw all for now, could be optimized for asking a redraw with exposure for the item
	check_minimum_size();
}
void Tree::select_single_item(TreeItem *p_selected,TreeItem *p_current,int p_col) {

	TreeItem::Cell &selected_cell=p_selected->cells[p_col];

	for (int i=0;i<columns;i++) {

		TreeItem::Cell &c=p_current->cells[i];

		if (select_mode==SELECT_ROW) {
	
			if (p_selected==p_current) {
	
				if (!c.selected) {
		
					c.selected=true;
									
					if (p_col==i)
						p_current->selected_signal.call(p_col);
				}
		
			} else {
		
				if (c.selected) {
		
					c.selected=false;
					p_current->deselected_signal.call(p_col);
				}
	
			}
	
		} else if (select_mode==SELECT_SINGLE) {
	
		if (&selected_cell==&c) {
	
			if (!selected_cell.selected) {
	
			selected_cell.selected=true;
			p_selected->selected_signal.call(p_col);
			}
		} else {
	
			c.selected=false;
			p_current->deselected_signal.call(p_col);
		}
		}

	}

	TreeItem *c=p_current->childs;

	while (c) {

		select_single_item(p_selected,c,p_col);
		c=c->next;
	}

}

void Tree::item_selected(int p_column,TreeItem *p_item) {

	update();
}
void Tree::item_deselected(int p_column,TreeItem *p_item) {

	update();
}


void Tree::set_select_mode(SelectMode p_mode) {

    select_mode=p_mode;
}

void Tree::clear() {

	if (root) {
		GUI_DELETE( root );
		root = NULL;
	};

	check_minimum_size();
	if (slider)
		slider->set_range(NULL);
};


void Tree::compute_column_size_caches() {


}

void Tree::set_hide_root(bool p_enabled) {

    hide_root=p_enabled;
    update();
}

void Tree::set_in_window() {

	popup_menu = GUI_NEW( PopUpMenu( get_window() ) );
	popup_menu->hide();
	popup_menu->selected_id_signal.connect( this, &Tree::popup_select_slot );
	line_edit_window = GUI_NEW( Window(get_window(),Window::MODE_POPUP,Window::SIZE_NORMAL) );
	VBoxContainer *vbc = GUI_NEW( VBoxContainer );
	line_edit_window->set_root_frame( vbc );
	line_edit = vbc->add( GUI_NEW( LineEdit ) );
	slider = vbc->add( GUI_NEW( HSlider ) );
	line_edit_window->hide();
	line_edit->text_enter_signal.connect(this,&Tree::line_edit_enter_slot);
	vbc->set_stylebox_override( stylebox(SB_LIST_EDITOR_BG) );
}

void Tree::set_column_min_width(int p_column,int p_min_width) {

	if (p_column<0 || p_column>=columns)
		return;
	if (p_min_width<1)
		return;
	column_min_size[p_column]=p_min_width;
	compute_column_size_caches();
	update();

}
void Tree::set_column_expand(int p_column,bool p_expand) {

	if (p_column<0 || p_column>=columns)
		return;
	column_expand[p_column]=p_expand;
	compute_column_size_caches();
	update();

}

bool Tree::get_selected( TreeItem** p_item, int *p_col) {
	
	if (!p_item)
		return false;
	if (!p_col)
		return false;
		
	if (!root)
		return false;
				
	
	while(true) {
	
		
		if (!*p_item) {
			*p_item=root;
			*p_col=0;
		} else {
			(*p_col)++;
			
			if (*p_col>=columns || select_mode==SELECT_ROW) {
				
				if ((*p_item)->childs) {
					
					*p_item=(*p_item)->childs;
					
				} else if ((*p_item)->next) {
				
					*p_item=(*p_item)->next;			
				} else {
				
					while(!(*p_item)->next) {
				
						*p_item=(*p_item)->parent;
						if (*p_item==NULL)
							return false;
					}
					
					*p_item=(*p_item)->next;
				}
				
				*p_col=0;
			}
		}
	
		if ((*p_item)->cells[*p_col].selected)
			return true;
	}
	
	return false;
}

int Tree::get_column_width(int p_column) const {
	
	if (p_column<0 || p_column>columns)
		return -1;
	
	return column_width_caches[p_column];
}
Tree::Tree(int p_columns) {


	if (p_columns<1)
		p_columns=1;

	if (p_columns>255)
		p_columns=255;

	columns=p_columns;
	column_min_size=GUI_NEW_ARRAY( int, columns );
	column_expand=GUI_NEW_ARRAY( bool, columns );
	column_width_caches=GUI_NEW_ARRAY( int, columns );

	for (int i=0;i<columns;i++) {

		column_expand[i]=true;
		column_min_size[i]=10;
		column_width_caches[i]=10;
	}

	hide_root=false;
	select_mode=SELECT_SINGLE;
	root=0;
	popup_menu=NULL;
	popup_edited_item=NULL;
	slider = NULL;
}


Tree::~Tree() {

	if (root) {
		GUI_DELETE( root );
	}
	GUI_DELETE_ARRAY( column_min_size );
	GUI_DELETE_ARRAY( column_expand );
	GUI_DELETE_ARRAY( column_width_caches );

	if (popup_menu)
		GUI_DELETE(popup_menu);
}


}
