

#include "container.h"
#include <stdio.h>

namespace GUI {




struct ContainerPrivate {

	StyleBox style_override;
	Container::Element * element_list;
	Container::Element * element_list_end;
	Size minimum_size;
	Size size_cache; //size cache
	bool resizing; /// flag to tell we are currently resizing
	bool recursive_resize_attempt; ///something decided on size, while we were resizing it..
	bool on_destructor;
	
	ContainerPrivate() { element_list = 0; element_list_end=0; resizing=false; recursive_resize_attempt=false; on_destructor=false; }

};

void Container::set_stylebox_override(const StyleBox& p_sb) {

	_cp->style_override=p_sb;
}
String Container::get_type() {

	return "Container";
}


/* Internal API */
Container::Element *Container::get_element_list() {
	
	return _cp->element_list;
}

Container::Element* Container::get_element_list_end() {
	
	return _cp->element_list_end;
	
}

Container::Element *Container::create_new_element() {

	return GUI_NEW( Element );
}

Container::Element* Container::add_frame_internal( Frame * p_frame, bool p_front ) {
	
	
	if (p_frame->get_parent()) {
		PRINT_ERROR("Child already has a parent!");
		return 0;
	}
	/** Check wether we are adding it twice */
	
	Element * list = get_element_list();
	
	
	while(list) {
		
		if (list->frame == p_frame ) {
				
			PRINT_ERROR("Trying to add frame twice");
			return 0;
		}
		
		list = list->next;
	}
	
	/** Add the element */
	
	Element * new_elem = create_new_element(); // containers may want to add stuff for each element
	new_elem->frame=p_frame;
	
	if (_cp->element_list) { //if exists
		
		if (p_front) {
			
			_cp->element_list_end->next=new_elem;
			new_elem->prev=_cp->element_list_end;
			_cp->element_list_end=new_elem;
		} else {
			_cp->element_list->prev=new_elem;
			new_elem->next=_cp->element_list;
			_cp->element_list=new_elem;
		}
			
		
	} else { //if it doesnt
		
		_cp->element_list_end=new_elem;
		_cp->element_list=new_elem;
	}
	
	
	p_frame->set_parent( this );
		
	if (get_window()) {
		p_frame->set_window( get_window() );
		get_window()->set_tree_size_changed();
	}
	
	
	return new_elem;
}


void Container::set_window(Window *p_window) {
	
	if (p_window==get_window())
		return; //pointless
	
	Frame::set_window(p_window);
	
	Element * list = get_element_list();
	
	while(list) {
	
		list->frame->set_window(p_window);
		list=list->next;		
	}
}

void Container::skin_changed() {
	
	Element * list = get_element_list();
	
	while(list) {
	
		list->frame->skin_changed();
		list=list->next;
	}
	
	
}

bool Container::is_child( Frame *p_frame) {
	
	Element * list = get_element_list();
	
	while(list) {
		
		if (list->frame == p_frame ) {
			
			return true;
		}
		
		list = list->next;	
		
	}
	
	return false;
}

void Container::remove_frame( Frame * p_frame ) {
	
	if (_cp->on_destructor)
		return;
		
	Element * list = get_element_list();
	
	while(list) {
		
		if (list->frame == p_frame ) {
				
			if (!list->prev && !list->next) {
				
				_cp->element_list=0;
				_cp->element_list_end=0;
			} else {
				
				//deleting doubly linked list is always more messy :D				
				if (!list->prev) 
					_cp->element_list=list->next;
				else
					list->prev->next=list->next;
				
				if (!list->next) 
					_cp->element_list_end=list->prev;
				else
					list->next->prev=list->prev;
				
			}
					
			GUI_DELETE( list );
			return;
		}
		
		list = list->next;	
		
	}
	
	PRINT_ERROR("Trying to delete unexisting frame");
}


/* Default Input Event Handlers */

Frame* Container::find_closest_focusable_to(Point p_point,Direction p_dir,int &mindist) {
	
	Frame *found=0;
	
	Element * list = get_element_list();
	

	Point margin_ofs=get_margin_offset();
	
	while(list) {
		
		if (!list->frame->is_visible()) {
			
			list=list->next;
			continue;
		}
		
		Rect child_rect=list->rect;
		child_rect.pos+=margin_ofs;
		
		if (list->frame->get_focus_mode()==FOCUS_ALL ) {
			
			
			bool test=true;
			Point against;
			
			switch(p_dir) {
				
				case LEFT: {
				
					against=child_rect.pos+Size(child_rect.size.x,0);
					if (against.x>p_point.x)
						test=false;
				} break;
				case RIGHT: {
					
					against=child_rect.pos;
					if (against.x<p_point.x)
						test=false;
					
				} break;
				case UP: {
					
					against=child_rect.pos+Size(0,child_rect.size.y);
					if (against.y>p_point.y)
						test=false;
					
				} break;
				case DOWN: {
					against=child_rect.pos;
					if (against.y<p_point.y)
						test=false;
					
				} break;
			}			
			
			if (test) {
				int dist=(against.x-p_point.x)*(against.x-p_point.x)+(against.y-p_point.y)*(against.y-p_point.y);
				
				if (dist<mindist) {
					mindist=dist;
					found=list->frame;
				}
			}	
					
		}
		
		Frame *subfound=list->frame->find_closest_focusable_to(p_point-child_rect.pos,p_dir,mindist);
		if (subfound)
			found=subfound;
		
		list = list->next;
		
	}
	
	
	return found;
	
}
Frame* Container::get_child_at_pos(const Point& p_pos,const Size &p_size,Point *p_local_pos) {
	
	//Element * list = get_element_list();
	Element * list = _cp->element_list_end;

	Point margin_ofs=get_margin_offset();
	
	while(list) {
	
		if (!list->frame->is_visible()) {
			
			list=list->prev;
			continue;
		}
		
		Rect child_rect=list->rect;
		child_rect.pos+=margin_ofs;
		
		if ( child_rect.has_point( p_pos ) ) {
			
	
			Frame *child=list->frame->get_child_at_pos( p_pos-child_rect.pos, child_rect.size,p_local_pos);
			if (child)
				return child;
			
			
		}
		
		list = list->prev;
		
	}
	

	return 0; //nothing found
}

void Container::mouse_button(const Point& p_pos, int p_button,bool p_press,int p_modifier_mask) {
	
	/* Nothing should happen */
	
}

void Container::mouse_doubleclick(const Point& p_pos,int p_modifier_mask) {
	
	
}
void Container::mouse_motion(const Point& p_pos, const Point& p_rel, int p_button_mask) {
	
	/* Nothing should happen */
	
}

bool Container::key(unsigned long p_unicode, unsigned long p_scan_code,bool p_press,bool p_repeat,int p_modifier_mask) {
	
	/* Nothing should happen */
	return false; //dont eat
}




void Container::draw_tree(const Point& p_global,const Size& p_size,const Rect& p_exposed) {


	draw(p_global, p_size, p_exposed);
	
	//if what is asked to draw is not inside what
	Rect exposed=p_exposed;

	Element * list = get_element_list();
	
	Point margin_ofs=get_margin_offset();
	
	while(list) {
		
		if (!list->frame->is_visible()) {
			
			list=list->next;
			continue;
		}
		
		Rect element_rect=list->rect;
		
		element_rect.pos+=margin_ofs;
		
		// add margin

		if ( exposed.intersects_with( element_rect) ) {

						
			/* PAINTER LOCAL RECT */
			
			get_window()->get_painter()->push_local_rect( element_rect );
						
			/* SET PAINTER CLIP RECT (if requested */
		
			bool clipped=false;
			if (list->frame->is_clipping()) {
				get_window()->get_painter()->push_clip_rect( Rect( Point(), element_rect.size) );
				clipped=true;
			}
			
			/* ELEMENT */
			Rect element_exposed=element_rect.clip(exposed);
			element_exposed.pos-=element_rect.pos;

			if (!element_exposed.has_no_area()) {
				list->frame->draw_tree(p_global+element_rect.pos,element_rect.size,element_exposed);
					
			} 
			
			get_window()->get_painter()->pop_local_rect();
			if (clipped)
				get_window()->get_painter()->pop_clip_rect();
			
		} else {
			

		}
		
		list = list->next;
	}

}


const StyleBox& Container::container_stylebox() {
	
	if (_cp->style_override.mode!=StyleBox::MODE_NONE)
		return _cp->style_override;
	
	if (!get_parent()) { /* Is Root Container */
		
		return stylebox( (get_window()->get_mode() == Window::MODE_POPUP) ? SB_POPUP_BG : SB_ROOT_CONTAINER );
	} else { /* is regular container */
		
		return stylebox( SB_CONTAINER );
	}

}


void Container::draw(const Point& p_pos,const Size &p_size,const Rect& p_exposed) {
	
	get_window()->get_painter()->draw_stylebox( container_stylebox() , Point(), p_size,p_exposed );
	
}

void Container::set_minimum_size(const Size & p_size ) {
	
	_cp->minimum_size=p_size;
	// emit some sort of resized change or something
}
Size Container::get_minimum_size() {
	
	if (!get_window())
		return _cp->minimum_size;
		
	
	Size minsize=get_minimum_size_internal();
	
	if (get_window()->get_painter()) {
	
		minsize+=get_window()->get_painter()->get_stylebox_min_size( container_stylebox() );
	};
	
	if (_cp->minimum_size.width > minsize.width )
		minsize.width=_cp->minimum_size.width;
	
	if (_cp->minimum_size.height > minsize.height )
		minsize.height=_cp->minimum_size.height;
	
	return minsize;
}

void Container::resize_tree(const Size& p_new_size) {
	
	// this limit avoids infinite loop
	// this is needed, because sometimes,
	// a widget will be resized, while we
	// are resizing it..
	// (like a scrollbar disapearing when a list is too big/small)
	// so we need to try and resize again..
	// however, the user may do something too complex for this too handle,
	// and may cause this to recurse forever, so the limit exists.
	
	Frame::resize_tree(p_new_size); //call back frame
	
	int max_recursive_attempt_limit=5; 
	
	do {
		_cp->recursive_resize_attempt=false;
		Size new_size=p_new_size;
		
		
		if (get_window()->get_painter()) {
		
			new_size-=get_window()->get_painter()->get_stylebox_min_size( container_stylebox() );
		};
		
		_cp->resizing=true;
		resize_internal( new_size );
		_cp->resizing=false;
		_cp->size_cache=p_new_size;
	} while (_cp->recursive_resize_attempt && max_recursive_attempt_limit--);
	
	
}


void Container::check_minimum_size() {

	if (!get_window())
		return;
		
	Size ms=get_minimum_size();
	
	if (ms.width>_cp->size_cache.width || ms.height>_cp->size_cache.height) {
	
		if (get_parent()) {
		
			get_window()->frame_request_resize(get_parent());
			get_parent()->check_minimum_size();	
		} else {
		
			get_window()->frame_request_resize(0);		
			get_window()->set_tree_size_changed();
		}		
	} else {
	
		get_window()->frame_request_resize(this);
		get_window()->set_tree_size_changed();
	}
}

Point Container::get_margin_offset() {
	
	if (get_window()->get_painter()) {
		
		
		return Point(
			get_window()->get_painter()->get_stylebox_margin( container_stylebox(), MARGIN_LEFT ),
			get_window()->get_painter()->get_stylebox_margin( container_stylebox(), MARGIN_TOP) 
		);
		
	} 
		
	return Point();
		
}

Frame *Container::next_in_focus( Frame *p_from ) {

	
	Element * list = get_element_list_end();

	bool found_current=p_from?false:true;

	while(list) {

		if (!list->frame->is_visible()) {
			
			list=list->prev;
			continue;
		}
		
		if (found_current) {

			Frame *f=list->frame->next_in_focus(); // just give me wathever first found
			if (f)
				return f;
		}
		
		if (list->frame==p_from) {

			found_current=true;
		}
		
		list = list->prev;
	} //failed searching this, search previous
	
	if ( p_from && get_parent() ) {


		return get_parent()->next_in_focus( this );
	}
	
	if ( p_from && !get_parent()) {
		
		return next_in_focus();
	}

	return 0;	

}

Frame *Container::prev_in_focus( Frame *p_from ) {


	Element * list = get_element_list();

	bool found_current=p_from?false:true;


	while(list) {

		if (!list->frame->is_visible()) {
			
			list=list->next;
			continue;
		}
		
		if (found_current) {

			Frame *f=list->frame->prev_in_focus(); // just give me wathever first found
			if (f)
				return f;
		}
		
		if (list->frame==p_from) {

			found_current=true;
		}
		
		list = list->next;
	} //failed searching this, search previous
	
	if ( p_from && get_parent() ) {

		
		return get_parent()->prev_in_focus( this );
	}
	
	if ( p_from && !get_parent()) {
		
		return prev_in_focus();
	}

	return 0;
}


Point Container::get_child_pos( Frame * p_child ) {

	Element * list = get_element_list();

	while(list) {

		if (!list->frame->is_visible()) {
			
			list=list->next;
			continue;
		}
		
		if (list->frame==p_child) {


			return list->rect.pos + get_margin_offset();
		}

		list=list->next;
	}

	return Point();

}

Size Container::get_child_size( Frame * p_child ) {
	
	Element * list = get_element_list();
	
	while(list) {
		
		if (!list->frame->is_visible()) {
			
			list=list->next;
			continue;
		}
		
		if (list->frame==p_child) {
			
			
			return list->rect.size;
		}
		
		list=list->next;
	}
	
	return Size();
	
}

ChildIterator Container::first_child() {
	
	Element * e=get_element_list();	
	
	if (!e)
		return ChildIterator();
	ChildIterator i;
	i.private_data=e;
	i.frame=e->frame;
	
	return i;
}
ChildIterator Container::next_child(const ChildIterator& p_child) {
	
	if (p_child.frame==0)
		return ChildIterator();
	

	Element * e=(Element*)p_child.private_data;
	
	if (!e)
		return ChildIterator();
	
	if (!e->next)
		return ChildIterator();
	
	ChildIterator i;
	i.private_data=e->next;
	i.frame=e->next->frame;
	
	return i;
	
}

Container::Container() {
	
	
	_cp = GUI_NEW( ContainerPrivate );
	
}

Container::~Container() {

	
	_cp->on_destructor=true;
	while (_cp->element_list) {

		Element *e=_cp->element_list;
		_cp->element_list=_cp->element_list->next;
		GUI_DELETE( e->frame );
		GUI_DELETE( e );
	}
	_cp->on_destructor=false;
	
	GUI_DELETE( _cp );
}

} // end of namespace
