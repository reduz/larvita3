//
// C++ Implementation: updown
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "updown.h"
#include "base/skin.h"
#include "base/painter.h"

namespace GUI {

void UpDown::resize(const Size& p_new_size) {
	
	size=p_new_size;
}

Size UpDown::get_minimum_size_internal() {
	
	Size minsize;
	
	if (constant( C_UPDOWN_ARROW_SIZE )>0 ) {
		minsize.width=constant( C_UPDOWN_ARROW_SIZE );
		minsize.height=constant( C_UPDOWN_ARROW_SIZE )*2;
	} else {
		
		minsize=get_painter()->get_bitmap_size( bitmap( BITMAP_UPDOWN ) );
	}
	
	minsize.width+=constant( C_UPDOWN_MARGIN )*2;
	minsize.height+=constant( C_UPDOWN_MARGIN )*2;
	return minsize;
}
String UpDown::get_type() {
	
	return "UpDown";	
}
void UpDown::draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed) {
	
	Point finalpos( 	constant( C_UPDOWN_MARGIN ), constant( C_UPDOWN_MARGIN ) );

	Size finalsize=size;
	finalsize.width-=constant( C_UPDOWN_MARGIN )*2;
	finalsize.height-=constant( C_UPDOWN_MARGIN )*2;
	
	if (constant( C_UPDOWN_ARROW_SIZE )>0 ) {
	
		get_painter()->draw_arrow( finalpos , Size( finalsize.width, finalsize.height/2) , UP, color( COLOR_UPDOWN ) );
		get_painter()->draw_arrow( finalpos+Point(0,finalsize.height/2) , Size( finalsize.width, finalsize.height/2) , DOWN, color( COLOR_UPDOWN ) );
	} else {
		
		Size bmpsize=get_painter()->get_bitmap_size( bitmap( BITMAP_UPDOWN ) );
		get_painter()->draw_bitmap( bitmap( BITMAP_UPDOWN ), finalpos+Point( (finalsize.width-bmpsize.width)/2, (finalsize.height-bmpsize.height)/2 ) );
		
	}
		
}
void UpDown::mouse_button(const Point& p_pos, int p_button,bool p_press,int p_modifier_mask) {
	
	if (!p_press)
		return;
	
	bool up=p_pos.y < (size.height/2);
	
	switch (p_button) {
		
		case BUTTON_LEFT: {
			
			if (up)
				get_range()->set( get_range()->get() + get_range()->get_step() );
			else
				get_range()->set( get_range()->get() - get_range()->get_step() );
		} break;
		case BUTTON_MIDDLE: {
			
			if (up)
				get_range()->set( get_range()->get() + get_range()->get_page() );
			else
				get_range()->set( get_range()->get() - get_range()->get_page() );
			
		} break;
		case BUTTON_RIGHT: {
			
			if (up)
				get_range()->set( get_range()->get_max() );
			else
				get_range()->set( get_range()->get_min() );
			
			
		} break;
		case BUTTON_WHEEL_UP: {
			
			get_range()->set( get_range()->get() + get_range()->get_step() );
			
		} break;
		case BUTTON_WHEEL_DOWN: {
			
			get_range()->set( get_range()->get() - get_range()->get_step() );
			
		} break;

	}
}

UpDown::UpDown()
{
}


UpDown::~UpDown()
{
}


}
