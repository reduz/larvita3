
#ifndef PIGUI_CONTAINER_H
#define PIGUI_CONTAINER_H


#include "base/frame.h"
#include "base/window.h"

namespace GUI {

struct ContainerPrivate;

class ChildIterator {
friend class Container;	
	void *private_data;
	Frame *frame;
public:	
	
	Frame* operator->() ///< Overload to access the child iterator, as ->data
	{ return frame; }
	Frame* operator*() ///< Overload to access the child iterator as *
	{ return frame; }
	Frame *get() { return frame; }
	
	bool empty() ///< Child iterator is empty
	{ return frame==0; }
	bool end() ///< Iteration has reached end
	{ return frame==0; }
	ChildIterator() { frame=0; private_data=0; }
};


class Container : public Frame {

public:

	struct Element {
	
		Rect rect;
		Frame * frame;
		Element *next;		
		Element *prev;		
		
		void set_rect( Rect p_rect ) {
			
			bool restraints = (!frame->can_fill_vertical() || !frame->can_fill_horizontal());
			
			if (!restraints) {
					
				rect=p_rect;
				return;
			}
			
			Size minsize = frame->get_minimum_size();
			
			if (!frame->can_fill_vertical()) {
				
				rect.pos.y=p_rect.pos.y+(p_rect.size.y-minsize.y)/2;
				rect.size.y=minsize.y;
			} else {
				
				rect.pos.y=p_rect.pos.y;
				rect.size.y=p_rect.size.y;
			}
			
			if (!frame->can_fill_horizontal()) {

				rect.pos.x=p_rect.pos.x+(p_rect.size.x-minsize.x)/2;
				rect.size.x=minsize.x;
			} else {
				
				rect.pos.x=p_rect.pos.x;
				rect.size.x=p_rect.size.x;
			}				
		}
		
		Element() { frame = 0; next =0; prev =0; }
		virtual ~Element() {}
	};

private:

	ContainerPrivate *_cp;
	
	virtual void draw(const Point& p_pos,const Size &p_size,const Rect& p_exposed); ///< Draw the container
	
	void draw_tree(const Point& p_global,const Size& p_size,const Rect& p_exposed);
	
	const StyleBox& container_stylebox();
	
	
	Point get_margin_offset();

	Frame* find_closest_focusable_to(Point p_point,Direction p_dir,int &mindist);	

	Frame* get_child_at_pos(const Point& p_point,const Size &p_size,Point *p_local_pos=0);	
	virtual void set_window(Window *p_window);
	virtual String get_type();

friend struct ContainerPrivate;	

protected:	


	virtual void skin_changed();	

	/* Internal API */
	Element* get_element_list();	
	Element* get_element_list_end();	
	Element* add_frame_internal( Frame * p_frame, bool p_front=false );

	
	/* Can be overrided */

	
	virtual Size get_minimum_size_internal()=0;
	virtual void resize_internal(const Size& p_new_size)=0;
	
	virtual Element *create_new_element();
	
	virtual void resize_tree(const Size& p_new_size); /// OVerride WITH CARE (call this back in the override as Container::resize_tree(). It is better to just override resize()

	
	/* Default Input Event Handlers */	
	
	void mouse_button(const Point& p_pos, int p_button,bool p_press,int p_modifier_mask); 
	void mouse_motion(const Point& p_pos, const Point& p_rel, int p_button_mask);
	void mouse_doubleclick(const Point& p_pos,int p_modifier_mask);

	bool key(unsigned long p_unicode, unsigned long p_scan_code,bool p_press,bool p_repeat,int p_modifier_mask);
	
friend class Frame;	
	void remove_frame( Frame * p_frame );
	
public:
	
	

	Signal< Method1<Size> > top_size_adjust_signal;
	
	virtual void check_minimum_size(); 

	
	Point get_child_pos( Frame * p_child );
	Size get_child_size( Frame * p_child );
	
	Frame *next_in_focus( Frame *p_from=0 );
	Frame *prev_in_focus( Frame *p_from=0 );
	
	virtual void set_minimum_size(const Size & p_size );
	virtual Size get_minimum_size();
		
	ChildIterator first_child();
	ChildIterator next_child(const ChildIterator& p_child);

	bool is_child( Frame *p_frame);
	
	void set_stylebox_override(const StyleBox& p_sb);
	
	Container();
	~Container();
};

}

#endif
