//
// C++ Implementation: box_container
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "box_container.h"

namespace GUI {

Container::Element *BoxContainer::create_new_element() {

	return GUI_NEW( BoxElement );
}

void BoxContainer::set_separation( int p_separation ) {

	separation=p_separation;
	//check_minimum_size();
}
Size BoxContainer::get_minimum_size_internal() {

	/* Calculate MINIMUM SIZE */

	Element * e= get_element_list();

	Size minimum;
	int sep=(separation<0)?constant(C_BOX_CONTAINER_DEFAULT_SEPARATION):separation;

	bool first=true;
	while (e) {

		if (!e->frame->is_visible()) {

			e=e->next;
			continue;
		}

		Size size=e->frame->get_minimum_size();

		if (vertical) { /* VERTICAL */

			if ( size.width > minimum.width ) {
				minimum.width=size.width;
			}

			minimum.height+=size.height+(first?0:sep);

		} else { /* HORIZONTAL */

			if ( size.height > minimum.height ) {
				minimum.height=size.height;
			}

			minimum.width+=size.width+(first?0:sep);

		}

		e=e->next;
		first=false;
	}

	return minimum;

}

void BoxContainer::add_frame(Frame *p_frame,int p_stretch) {

	BoxElement * e=(BoxElement *)add_frame_internal( p_frame );
	if (!e)
		return;
	e->stretch=p_stretch;

	check_minimum_size();
}


void BoxContainer::resize_internal(const Size& p_new_size) {

	/** First pass, determine minimum size AND amount of stretchable elements */


	Size new_size=p_new_size;

	Element *me=get_element_list();
	int sep=(separation<0)?constant(C_BOX_CONTAINER_DEFAULT_SEPARATION):separation;

	bool first=true;
	while (me) {

		if (!me->frame->is_visible()) {

			me=me->next;
			continue;
		}

		if (first) {

			me=me->next;
			first=false;
			continue;

		}

		if (vertical)
			new_size.height-=sep;
		else
			new_size.width-=sep;

		me=me->next;
	}

	BoxElement * e= (BoxElement*)get_element_list(); //this cast is for speed

	if (!e)
		return; //no elements? don't bother!

	BoxElement * first_stretchable=e;

	Size min_size;

	int stretch_coeffs_total=0;
	int stretchable_space=0;
	while (e) {

		if (!e->frame->is_visible()) {

			e=(BoxElement*)e->next;
			continue;
		}

		Size size=e->frame->get_minimum_size();

		if (vertical) { /* VERTICAL */

			if ( size.width > min_size.width ) {
				min_size.width=size.width;
			}

			min_size.height+=size.height;

			e->stretch_cache.min_size=size.height;
			e->stretch_cache.perp_min_size=size.width;

		} else { /* HORIZONTAL */

			if ( size.height > min_size.height ) {
				min_size.height=size.height;
			}

			min_size.width+=size.width;
			e->stretch_cache.min_size=size.width;
			e->stretch_cache.perp_min_size=size.height;

		}

		if (e->stretch>0) {

			stretch_coeffs_total+=e->stretch;
			e->stretch_cache.will_stretch=true;
			stretchable_space+=e->stretch_cache.min_size;

		} else {

			e->stretch_cache.will_stretch=false;

		}

		e=(BoxElement*)e->next; //speed cast
	}

	stretchable_space+=vertical? //determine available space for stretching widgets
			( new_size.height - min_size.height ): // vertical layout
			( new_size.width - min_size.width ); // horizontal layout


	if (stretchable_space < 0) {
		#ifndef WINDOWS_ENABLED
			#warning this is probably a bug, it IS happening
		#endif
		//PRINT_ERROR("Negative stretchable space?");
		return;
	}

	/** Second, pass sucessively to discard elements that can't be stretched, this will run while stretchable
		elements exist */


	while(stretch_coeffs_total>0) { // first of all, dont even be here if no stretchable objects exist

		e= (BoxElement*)get_element_list(); //this cast is for speed


		bool refit_successful=true; //assume refit-test will go well

		while (e) {

			if (!e->frame->is_visible()) {

				e=(BoxElement*)e->next;
				continue;
			}

			if (e->stretch_cache.will_stretch) { //wants to stretch
				//let's see if it can really stretch

				int final_pixel_size=stretchable_space * e->stretch / stretch_coeffs_total;
				if (final_pixel_size<e->stretch_cache.min_size) {
					//if available stretching area is too small for widget,
					//then remove it from stretching area
					e->stretch_cache.will_stretch=false;
					stretch_coeffs_total-=e->stretch;
					refit_successful=false;
					stretchable_space-=e->stretch_cache.min_size;

					break;
				}
			}

			e=(BoxElement*)e->next; //speed cast
		}

		if (refit_successful) //uf refit went well, break
			break;

	}


	/** Final pass, draw and stretch elements **/



	e= (BoxElement*)get_element_list(); //this cast is for speed

	if (stretchable_space < 0) {
		PRINT_ERROR("Negative stretchable space");
		return;
	}

	int offset=vertical?p_new_size.height:p_new_size.width; //this will decrement until reaching zero

	if (stretch_coeffs_total==0) { //nothing to stretch
		offset-=stretchable_space; //add room at the right
	}

	int total_size_perp=vertical?p_new_size.width:p_new_size.height;


	first=true;

	while (e) {

		if (!e->frame->is_visible()) {

			e=(BoxElement*)e->next;
			continue;
		}

		int final_size=0;

		if (e->stretch_cache.will_stretch) {

			final_size=stretchable_space * e->stretch / stretch_coeffs_total;



		} else {

			final_size=e->stretch_cache.min_size;
		}

		offset-=final_size;

		if (first)

			first=false;
		else
			offset-=sep;

		if (!e->next && offset>0) {
			//adjust so the last one always fits perfect
			//compensating for numerical imprecision

			final_size+=offset;
			offset=0;
		}

		int final_pos=offset;

		bool can_fill=vertical?e->frame->can_fill_vertical():e->frame->can_fill_horizontal();

		if (!can_fill) {

			final_pos+=(final_size-e->stretch_cache.min_size)/2; //center it
			final_size=e->stretch_cache.min_size; //and change size

		}

		//bool can_fill_perp=vertical?e->frame->can_fill_horizontal():e->frame->can_fill_vertical();
		int final_pos_perp=0;
		int final_size_perp=total_size_perp;

		/*
		if (!can_fill_perp) {

			final_pos_perp+=(final_size_perp-e->stretch_cache.perp_min_size)/2; //center it
			final_size_perp=e->stretch_cache.perp_min_size; //and change size

		}*/

		Rect final_rect;

		if (vertical) { /* VERTICAL */


			final_rect.pos.x=final_pos_perp;
			final_rect.size.width=final_size_perp;

			final_rect.pos.y=final_pos;
			final_rect.size.height=final_size;

		} else { /* HORIZONTAL */

			final_rect.pos.y=final_pos_perp;
			final_rect.size.height=final_size_perp;

			final_rect.pos.x=final_pos;
			final_rect.size.width=final_size;

		}

		e->set_rect( final_rect );


		e->frame->resize_tree( e->rect.size );

		e=(BoxElement*)e->next; //speed cast
	}


}


BoxContainer::BoxContainer(bool p_vertical)
{
	vertical=p_vertical;
	separation=-1;
}


BoxContainer::~BoxContainer()
{
}


}
