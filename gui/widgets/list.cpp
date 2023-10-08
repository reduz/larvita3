//
// C++ Implementation: list
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "list.h"
#include "base/painter.h"


#include "containers/box_container.h"
namespace GUI {



String List::get_type() {

	return "List";
}


void List::range_ptr_changed() {

	get_range()->set_min(0);
	get_range()->set_max(get_size());
	get_range()->set_page(get_visible_lines());
	get_range()->set_step(1);
	get_range()->set(offset);
}
void List::value_changed(double p_new_val) {

	offset=(int)p_new_val;
	update();
}

void List::set_offset(int p_offset) {

	offset=p_offset;
	get_range()->set( offset );

}

int List::get_line_size() {

	if (!get_painter())
		return 0;

	return get_painter()->get_font_height( font( FONT_LIST ) )+constant( C_LIST_SPACING );

}

int List::get_visible_lines() {

	if (!get_painter())
		return 0;

	
	int space=size.height;
	space-=get_painter()->get_stylebox_margin( stylebox( SB_LIST_NORMAL ) , MARGIN_TOP );
	space-=get_painter()->get_stylebox_margin( stylebox( SB_LIST_NORMAL ) , MARGIN_BOTTOM );

	int line_size=get_line_size();

	if (line_size==0)
		return 0;
	return space/line_size;

}

	
int List::get_min_total_height() {
	
	int h=0;
	
	h+=get_painter()->get_stylebox_margin( stylebox( SB_LIST_NORMAL ) , MARGIN_TOP );
	h+=get_painter()->get_stylebox_margin( stylebox( SB_LIST_NORMAL ) , MARGIN_BOTTOM );
	
	h+=get_line_size()*get_size();
	
	return h;
}
	
Size List::get_minimum_size_internal() {
	
	Size min=get_painter()->get_stylebox_min_size( stylebox( SB_LIST_NORMAL ) );

	return min;
	

}

void List::draw(const Point& p_global,const Size& p_size,const Rect& p_exposed) {


	get_painter()->draw_stylebox( stylebox( SB_LIST_NORMAL ), Point() , p_size, p_exposed );
	if (has_focus())
		get_painter()->draw_stylebox( stylebox( SB_LIST_FOCUS ), Point() , p_size, p_exposed);
	

	Element *l=list;
	int idx=0;
	int margin_top=get_painter()->get_stylebox_margin( stylebox( SB_LIST_NORMAL ) , MARGIN_TOP );
	int margin_left=get_painter()->get_stylebox_margin( stylebox( SB_LIST_NORMAL ) , MARGIN_LEFT );
	int area_width=size.width-margin_left-get_painter()->get_stylebox_margin( stylebox( SB_LIST_NORMAL ) , MARGIN_RIGHT );
	int text_margin=margin_left+constant( C_LIST_HMARGIN );
	int text_width=area_width-constant( C_LIST_HMARGIN )*2;
	int font_ascent=get_painter()->get_font_ascent( font( FONT_LIST ) )+constant( C_LIST_SPACING )/2;

	for (;l!=0;idx++,l=l->next) {

		if (idx<offset)
			continue;

		int i=idx-offset;
		
		if (i>=get_visible_lines())
			break;

		int ofs=margin_top+i*get_line_size();

		String text=l->text;

		if (number_elements>=0) {

			String numberer=String::num( idx+number_elements );
			while (numberer.length()<number_min_integers)
				numberer="0"+numberer;
			text=numberer+" "+text;

		}
		if (l->selected) {

			get_painter()->draw_stylebox( stylebox( SB_LIST_SELECTED ), Point( margin_left, ofs ), Size( area_width, get_line_size() ), p_exposed );
			get_painter()->draw_text( font( FONT_LIST ) , Point( text_margin, ofs+font_ascent ) , text, color(COLOR_LIST_FONT_SELECTED),text_width );

		} else {

			get_painter()->draw_text( font( FONT_LIST ) , Point( text_margin, ofs+font_ascent ) , text , color(COLOR_LIST_FONT),text_width);

		}

		if ((allow_multiple && idx==cursor && has_focus()) || (!allow_multiple && cursor_hint && mouse_over_item==idx))
			get_painter()->draw_stylebox( stylebox( SB_LIST_CURSOR ), Point( text_margin, ofs+constant( C_LIST_SPACING )/2 ), Size( text_width, get_line_size()-constant( C_LIST_SPACING ) ), p_exposed );



	}


		
		

	
	
}

void List::mouse_doubleclick(const Point& p_pos, int p_modifier_mask) {
	

	
	int y=p_pos.y;
	y-=get_painter()->get_stylebox_margin( stylebox( SB_LIST_NORMAL ) , MARGIN_TOP );
	

	if (y<0)
		return;

	int element=y/get_line_size();


	
	if (element>=get_visible_lines())
		return;

	element+=offset;
	
	activated_signal.call(element);
	
}

void List::mouse_button(const Point& p_pos, int p_button,bool p_press,int p_modifier_mask) {
	
	if (!p_press)
		return;

	if (p_button==BUTTON_WHEEL_UP && p_press) {
		
		get_range()->set( get_range()->get() - 4 );
		

	}
	if (p_button==BUTTON_WHEEL_DOWN && p_press) {
		
		get_range()->set( get_range()->get() + 4 );
		
		//get_range()->step_increment();
	}
		
	if (p_button!=BUTTON_LEFT)
		return;

	int y=p_pos.y;
	y-=get_painter()->get_stylebox_margin( stylebox( SB_LIST_NORMAL ) , MARGIN_TOP );

	if (y<0)
		return;

	int element=y/get_line_size();

	if (element>=get_visible_lines())
		return;

	element+=offset;
	
	if (allow_multiple && p_modifier_mask&KEY_MASK_CTRL) {

		if (is_selected(element))

			unselect( element );
		else

			select( element );

	} else {

		if (editable && is_selected(element)) {


			create_editor( element );

		} else {
			
			select( element, true );
		}
	}

	cursor=element;
	if (allow_multiple)
		cursor_changed_signal.call(cursor);
		
			
	
}

void List::create_editor(int p_for_element) {

	Point ofs;
	ofs.y+=get_painter()->get_stylebox_margin( stylebox( SB_LIST_NORMAL ) , MARGIN_TOP );
	ofs.y+=(p_for_element-offset)*get_line_size();
	ofs.x+=get_painter()->get_stylebox_margin( stylebox( SB_LIST_NORMAL ) , MARGIN_LEFT );

	
	Size sz;

	sz.width=size.width-ofs.x-get_painter()->get_stylebox_margin( stylebox( SB_LIST_NORMAL ) , MARGIN_RIGHT );
	sz.height=get_line_size();
			
	ofs+=get_global_pos();

	editor_window->set_size( sz );
	
	ofs.y-=(editor_window->get_size().height-get_line_size())/2;
	editor_window->set_pos( ofs );
	
	
	editor->set_text( get_string( p_for_element ) );
	
	editor_window->show();
	

	editor->text_enter_signal.connect( this, &List::editor_string_changed );
	editor_pos=p_for_element;

}

void List::set_cursor_pos(int p_pos) {
	
	if (allow_multiple) {
		
		if (p_pos<0 || p_pos>=get_size())
			return;
		cursor=p_pos;
		
		if (cursor>=(offset+get_visible_lines()))
			set_offset((cursor-get_visible_lines())+1);
		if (cursor<offset)
			set_offset(cursor);

	} else
		select(p_pos);
		
	update();
}

void List::editor_string_changed(String p_string) {

	if (!editor_window)
		return;

	if (p_string.length()>max_string_len)
		p_string=p_string.substr( 0, max_string_len );
		   
	set_string( p_string , editor_pos );
	
	editor_window->hide();
	
	item_edited_signal.call( editor_pos, p_string );

}
void List::mouse_motion(const Point& p_pos, const Point& p_rel, int p_button_mask) {
	
	if (cursor_hint) {
		int y=p_pos.y;
		y-=get_painter()->get_stylebox_margin( stylebox( SB_LIST_NORMAL ) , MARGIN_TOP );
	
		if (y<0)
			return;
	
		int element=y/get_line_size();
		element+=offset;
		if (mouse_over_item!=element)
			update();
		mouse_over_item=element;
	}

	
}

void List::ensure_cursor_visible() {
	int selected=get_selected();
	
	if (allow_multiple) {
		if (cursor>=(offset+get_visible_lines()))
			set_offset((cursor-get_visible_lines())+1);
		if (cursor<offset)
			set_offset(cursor);

	} else {
		if (selected>=(offset+get_visible_lines()))
			set_offset((selected-get_visible_lines())+1);
		if (selected<offset)
			set_offset(selected);
	}
			

}

void List::isearch_timeout_slot() {
	
	
	if (isearch_timer==INVALID_TIMER_ID)
		return;
	
	isearch_word="";
	get_window()->get_timer()->remove_timer(isearch_timer);	
	isearch_timer=INVALID_TIMER_ID;
}

bool List::key(unsigned long p_unicode, unsigned long p_scan_code,bool p_press,bool p_repeat,int p_modifier_mask) {
	

	if (!p_press)
		return false;
	
	switch (p_scan_code) {

		case KEY_UP: {
			

			if (allow_multiple) {

				if (cursor>0) {

					cursor--;

					if (cursor<offset)
						set_offset(cursor);
						
					cursor_changed_signal.call(cursor);
					

					update();
				}
				

			} else {


				int selected=get_selected();
				
				if (selected>0) {

					selected--;
					if (selected<offset)
						set_offset(selected);
					select( selected );

					update();

				}

			}
				
			
				
		} break;
		case KEY_DOWN: {
	

			if (allow_multiple) {

				if (cursor<(get_size()-1)) {

					cursor++;

					if (cursor>=(offset+get_visible_lines()))
						set_offset((cursor-get_visible_lines())+1);

					cursor_changed_signal.call(cursor);
					
					update();
				}
				

			} else {

				int selected=get_selected();
				
				if (selected<(get_size()-1)) {

					selected++;
					
					if (selected>=(offset+get_visible_lines()))
						set_offset((selected-get_visible_lines())+1);
					
					select( selected );

					
					update();

				}



			}
			
		} break;
		case KEY_HOME: {
				
			if (allow_multiple) {
			
				cursor=0;
				set_offset(0);
				update();
				cursor_changed_signal.call(cursor);
				
			} else {

				set_offset(0);
				select(0);
				update();

			}
			
		} break;
		case KEY_END: {
	
			if (allow_multiple) {

				cursor=get_size()-1;

				if (cursor>=(offset+get_visible_lines()))
					set_offset((cursor-get_visible_lines())+1);

				update();
				cursor_changed_signal.call(cursor);
				
			} else {

				int selected=get_size()-1;
				

				if (selected>=(offset+get_visible_lines()))
					set_offset((selected-get_visible_lines())+1);

				select( selected );

				update();

			}
			

		} break;

		case KEY_PAGEUP: {

			if (allow_multiple) {


				cursor-=get_visible_lines()/2;
				if (cursor<0)
					cursor=0;

				if (cursor<offset)
					set_offset(cursor);

				update();

				cursor_changed_signal.call(cursor);

			} else {


				int selected=get_selected();
				selected-=get_visible_lines()/2;

				if (selected<0)
					selected=0;
				if (selected<offset)
					set_offset(selected);
				select( selected );

				update();


			}

		} break;
		
		
		case KEY_RETURN: {
			
			if (!editable) {
				
				//if nothing is pre-selected, pressing enter will select
				
				if (allow_multiple && get_selection_size()==0) {
					
					select( cursor );
				}
				
				enter_pressed.call();
				activated_signal.call(allow_multiple?cursor:get_selected());
				break;
			}
			
			if (allow_multiple) {

				create_editor( cursor );

			} else {

				create_editor( get_selected() );
			}
			editor->select_all();
			
		} break;

		case KEY_PAGEDOWN: {

			if (allow_multiple) {


				cursor+=get_visible_lines();

				int maxv=(get_size()-1);
				
				if (cursor>maxv)
					cursor=maxv;
					
				if (cursor>=(offset+get_visible_lines()))
					set_offset((cursor-get_visible_lines())+1);

				update();
				cursor_changed_signal.call(cursor);

			} else {

				int selected=get_selected();
				selected+=get_visible_lines();
				
				int maxv=(get_size()-1);
				
				if (selected>maxv)
					selected=maxv;
				

				if (selected>=(offset+get_visible_lines()))
					set_offset((selected-get_visible_lines())+1);

				select( selected );

				update();


			}

		} break;
		
		case KEY_SPACE: {
	
			
			if (allow_multiple) {

				if (is_selected(cursor))

					unselect( cursor );
				else

					select( cursor );

			}
		} break;
		
		default: {
			
			if (p_unicode && isearch_enabled && get_window()->get_timer()) {
				
				Element *l=list;
				isearch_word+=p_unicode;
				
				int count=0;
				bool match=false;
				while (l) {

					if (l->text.strip_edges().findn(isearch_word)==0) {
						/* matches */
						if (isearch_timer!=INVALID_TIMER_ID) {	
							
							get_window()->get_timer()->remove_timer(isearch_timer);
							
						}
						isearch_timer=get_window()->get_timer()->create_timer( Method(this,&List::isearch_timeout_slot), INCREMENTAL_SEARCH_TIMEOUT );
						
						if (allow_multiple) {
							
							cursor=count;
						} else {
							
							select( count );
						}
						
						ensure_cursor_visible();
						match=true;
						break;
					}
					l=l->next;
					count++;
				}
				
				if (!match)
					isearch_word=""; /* word not found, reset isearch */
			}
			
			return false;
		} break;
			
	}
	
	return true;

}
void List::resize(const Size& p_new_size) {
	
	
	size=p_new_size;
	get_range()->set_page( get_visible_lines() );
}
	
	
int List::get_selection_size() {
	
	Element *l=list;
	int count=0;

	while (l) {


		if (l->selected)
			count++;


		l=l->next;

	}
	
	return count;
	
}
int List::get_selected(int p_which) {
	
	Element *l=list;
	int idx=0;
	int count=0;

	while (l) {


		if (l->selected) {

			if (count==p_which)
				return idx;
			count++;

		}


		l=l->next;
		idx++;

	}
	
	return -1; //invalid
	
	
}
String List::get_selected_string(int p_which) {
	
	Element *l=list;

	int count=0;

	while (l) {


		if (l->selected) {

			if (count==p_which)
				return l->text;
			count++;

		}


		l=l->next;


	}
	
	return ""; //invalid
	
	
}
	
	
void List::add_sorted_string(const String& p_str) {
		

	Element *n = GUI_NEW( Element );

	n->text=p_str;
	n->selected=false;

	Element **l=&list;

	while(true) {


		if (!(*l) || p_str.nocasecmp_to( (*l)->text)<0 ) {

			
			n->next=(*l);
			*l=n;
			update();
			get_range()->set_max( get_size() );
			return;

		}

		l=&(*l)->next;

	} 
	
	
}


void List::add_string(const String& p_str,int p_at) {


	Element *n = GUI_NEW( Element );

	n->text=p_str;
	n->selected=false;

	if (!list || p_at==0) {
		//if list is empty
		n->next=list;
		list=n;
		update();
		get_range()->set_max( get_size() );
		return;

	}

	Element *l=list;
	int count=1;

	while (l) {


		if (count==p_at || !l->next) {

			n->next=l->next;
			l->next=n;
			update();
			get_range()->set_max( get_size() );
			return;

		}

		l=l->next;
		count++;

	}

}

int List::find_string_index(String p_string) {
	
	
	Element *l=list;

	int idx=0;

	while (l) {


		if (l->text==p_string) {

			return idx;

		}


		l=l->next;
		idx++;

	}
	
	return -1;
}
	

void List::erase(int p_at) {
	
	
	Element **lpre=&list;
	Element *l=list;

	int idx=0;

	while (l) {


		if (idx==p_at) {

			Element *aux=l;
			*lpre=l->next;
			GUI_DELETE( aux );
			if (cursor>=idx && cursor>0)
				cursor--;
			
			update();
			get_range()->set_max( get_size() );
			return;

		}


		lpre=&l->next;
		l=l->next;
		idx++;


	}

	
}

void List::select(int p_at,bool p_exclusive) {
	
	Element *l=list;

	int idx=0;

	int selected=-1;
	while (l) {


		if (idx==p_at) {

			l->selected=true;
			selected=idx;

		} else if (!allow_multiple || p_exclusive) {

			l->selected=false;
			
		}
		l=l->next;
		idx++;

	}	

	if (selected!=-1)
		selected_signal.call( selected );
	if (allow_multiple)
		selection_changed_signal.call();
	update();
			
}


String List::get_string(int p_at) {

	Element *l=list;

	int idx=0;

	while (l) {


		if (idx==p_at) {

			return l->text;
			break;

		}
		l=l->next;
		idx++;

	}

	return ""; //not found!

}
void List::set_string(const String& p_str,int p_at) {

	Element *l=list;

	int idx=0;

	while (l) {


		if (idx==p_at) {

			l->text=p_str;
			break;

		}
		l=l->next;
		idx++;

	}
	
	update();


}


void List::unselect(int p_at) {
	
	
	Element *l=list;

	int idx=0;

	while (l) {


		if (idx==p_at) {

			l->selected=false;
			update();
			selection_changed_signal.call();
			return;

		}

		l=l->next;
		idx++;

	}
	
}

bool List::is_selected(int p_at) {
	
	Element *l=list;

	int idx=0;

	while (l) {


		if (idx==p_at) {

			return l->selected;

		}
		
		l=l->next;
		idx++;

	}
	
	return false;
			
}

int List::get_size() {
	
	Element *l=list;

	int count=0;

	while (l) {

		l=l->next;
		count++;
	}
	
	return count;
			
}



void List::set_multi_mode(bool p_enable) {
	

    allow_multiple=p_enable;
    update();
	
}

void List::set_editable(bool p_editable) {

	editable=p_editable;
}

void List::clear() {
	
	while(list) {

		Element *aux=list;
		list=list->next;
		GUI_DELETE( aux );
		
	}

	
	offset=0;
	list=0;
	cursor=0;
	set_offset(offset);
	get_range()->set_max( get_size() );
}

class VBC : public VBoxContainer{
public:
	const StyleBox & get_stylebox() { return stylebox(SB_LIST_EDITOR_BG); }
	
};


void List::set_in_window() {

	
	editor_window = GUI_NEW( Window(get_window(),Window::MODE_POPUP) );

	VBoxContainer *vbc = GUI_NEW( VBC );
	editor_window->set_root_frame( vbc );
	vbc->set_stylebox_override( stylebox(SB_LIST_EDITOR_BG) );
	editor= vbc->add(GUI_NEW( LineEdit() ),1);
	editor->get_focus();
	editor->set_max_length( max_string_len );
}

void List::set_cursor_hint(bool p_cursor_hint) {
	
	cursor_hint=p_cursor_hint;
	update();
}

void List::set_number_elements(int p_num) {


	number_elements=p_num;
}
void List::set_number_min_integers(int p_num) {

	if (p_num<0)
		return;
	if (p_num>32)
		return;
	
	number_min_integers=p_num;
	
	
}

void List::set_incremental_search(bool p_enabled) {
	
	isearch_enabled=p_enabled;
}

void List::set_max_string_length(int p_length) {

	if (p_length<=0)
		return;
	max_string_len=p_length;
	if (editor)
		editor->set_max_length( p_length );
}
int List::get_max_string_length() {

	return max_string_len;
}

List::List() {
	
	list=0;
	offset=0;
	allow_multiple=false;
	cursor=0;
	editable=false;
	editor=0;
	editor_window=0;
	mouse_over_item=-1;
	cursor_hint=false;
	number_elements=-1;
	number_min_integers=0;
	max_string_len=128;
	
	set_focus_mode( FOCUS_ALL );

	get_range()->set_min(0);
	get_range()->set_max(get_size());
	get_range()->set_step(1);
	get_range()->set(0);
	
	isearch_enabled=true;
	isearch_timer=INVALID_TIMER_ID;
	
	
}


List::~List() {

	while(list) {

		Element *aux=list;
		list=list->next;
		GUI_DELETE( aux );
		
	}
	GUI_DELETE( editor_window );
}



}
