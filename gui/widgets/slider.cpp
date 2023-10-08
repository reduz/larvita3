
//
// C++ Implementation: slider
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "slider.h"
#include "base/painter.h"

#include "base/skin.h"
namespace GUI {


int Slider::get_grabber_size() {
	
	int grabber_size=0;
	
	BitmapID grabber_bmp = (orientation==VERTICAL)?bitmap( BITMAP_SLIDER_GRABBER_V ) : bitmap( BITMAP_SLIDER_GRABBER_H );
	if (constant( C_SLIDER_GRABBER_SIZE )>0) { //if using stylebox..
		
		grabber_size=constant( C_SLIDER_GRABBER_SIZE );
	} else { //if using bitmap
		
		grabber_size=(orientation==VERTICAL)?
				get_painter()->get_bitmap_size( grabber_bmp ).height:
				get_painter()->get_bitmap_size( grabber_bmp ).width;
	}
	
		
	
	return grabber_size;
}

int Slider::get_margins_size() {
	
	const StyleBox &sb_slider_normal=(orientation==VERTICAL)?stylebox(SB_SLIDER_NORMAL_V):stylebox(SB_SLIDER_NORMAL_H);
	return (orientation==VERTICAL)?
			( get_painter()->get_stylebox_margin( sb_slider_normal , MARGIN_TOP ) +
			get_painter()->get_stylebox_margin( sb_slider_normal , MARGIN_BOTTOM ) ):
			( get_painter()->get_stylebox_margin( sb_slider_normal , MARGIN_LEFT ) +
			get_painter()->get_stylebox_margin( sb_slider_normal , MARGIN_RIGHT ) );
	
}

double Slider::get_click_pos(const Point& p_pos) {
	
	
	int pos=(orientation==VERTICAL)?(size.height-p_pos.y):p_pos.x;
	int range=(orientation==VERTICAL)?size.height:size.width;
//	int grabber=get_grabber_size();
	
	range-=(get_margins_size()+get_grabber_size());
	
	return (double)pos/(double)range; //may look strange, but it's very useful	
}

int Slider::get_grabber_offset() {
	
	
	int area=(orientation==VERTICAL)?size.height:size.width;
	
	
	area-=get_grabber_size();
			
	
	area-=get_margins_size();

	int grabber_ofs=(int)(get_range()->get_unit_value()*(double)area);
	
	if (orientation==VERTICAL)
		grabber_ofs=area-grabber_ofs;
	
	return grabber_ofs;
}

	

Size Slider::get_minimum_size_internal() {
	
	const StyleBox &sb_slider_normal=(orientation==VERTICAL)?stylebox(SB_SLIDER_NORMAL_V):stylebox(SB_SLIDER_NORMAL_H);
	BitmapID grabber_bmp = (orientation==VERTICAL)?bitmap( BITMAP_SLIDER_GRABBER_V ) : bitmap( BITMAP_SLIDER_GRABBER_H );
	
	
	Size min;
	
	min.width=get_painter()->get_stylebox_margin( sb_slider_normal , MARGIN_LEFT )+get_painter()->get_stylebox_margin( sb_slider_normal , MARGIN_RIGHT );
	
	min.height=get_painter()->get_stylebox_margin( sb_slider_normal , MARGIN_TOP )+get_painter()->get_stylebox_margin( sb_slider_normal , MARGIN_BOTTOM );
	

	if (constant( C_SLIDER_GRABBER_SIZE )>0) {
		
		min.height+=constant( C_SLIDER_GRABBER_SIZE );
		min.width+=constant( C_SLIDER_GRABBER_SIZE );
		
	} else {
		
		min+=get_painter()->get_bitmap_size( grabber_bmp );
	}
	
	return min;
	
			
}

void Slider::draw(const Point& p_global,const Size& p_size,const Rect& p_exposed) {

	const StyleBox &sb_slider_normal=(orientation==VERTICAL)?stylebox(SB_SLIDER_NORMAL_V):stylebox(SB_SLIDER_NORMAL_H);
	BitmapID grabber_bmp = (orientation==VERTICAL)?bitmap( BITMAP_SLIDER_GRABBER_V ) : bitmap( BITMAP_SLIDER_GRABBER_H );
	const StyleBox &sb_slider_grabber=(orientation==VERTICAL)?stylebox(SB_SLIDER_GRABBER_V):stylebox(SB_SLIDER_GRABBER_H);
	
	
	get_painter()->draw_stylebox( sb_slider_normal, Point() , p_size );
	
	if (has_focus())
		get_painter()->draw_stylebox( stylebox( SB_SLIDER_FOCUS ), Point() , p_size );
	
//	int grabber_size=get_grabber_size();
	int grabber_ofs=get_grabber_offset()+ ((orientation==VERTICAL)?( get_painter()->get_stylebox_margin( sb_slider_normal , MARGIN_TOP )):( get_painter()->get_stylebox_margin( sb_slider_normal , MARGIN_LEFT )));


	Point pos;

	if (orientation==VERTICAL) {
		
		pos=Point( get_painter()->get_stylebox_margin( sb_slider_normal , MARGIN_LEFT ), grabber_ofs );
		
	} else {
		pos=Point( grabber_ofs, get_painter()->get_stylebox_margin( sb_slider_normal , MARGIN_TOP ) );
	}
	
	if (constant( C_SLIDER_GRABBER_SIZE )>0) { //paint using stylebox
			
		get_painter()->draw_stylebox( sb_slider_grabber , pos , Size( constant( C_SLIDER_GRABBER_SIZE ), constant( C_SLIDER_GRABBER_SIZE ) ) );
			
	} else {
			
		get_painter()->draw_bitmap( grabber_bmp , pos );
			
	}

}
void Slider::mouse_button(const Point& p_pos, int p_button,bool p_press,int p_modifier_mask) {

	
	if (p_button==BUTTON_WHEEL_UP && p_press) {
		
		get_range()->step_increment();

	}
	if (p_button==BUTTON_WHEEL_DOWN && p_press) {
		
		get_range()->step_decrement();

	}
	
	if (p_button!=BUTTON_LEFT)
		return;
	
	if (p_press) {
	
		drag.active=true;
		drag.pos_at_click=get_click_pos(p_pos);
		drag.value_at_click=get_range()->get_unit_value();
		

	} else {
		
		drag.active=false;
	}
	
}
void Slider::mouse_motion(const Point& p_pos, const Point& p_rel, int p_button_mask) {

	if (!drag.active)
		return;
			
	double value_ofs=drag.value_at_click+(get_click_pos(p_pos)-drag.pos_at_click);
	

	if (value_ofs<0)
		value_ofs=0;
	if (value_ofs>1)
		value_ofs=1;
	if (get_range()->get_unit_value()==value_ofs)
		return; //dont bother if the value is the same

	get_range()->set_unit_value( value_ofs );
	


}
bool Slider::key(unsigned long p_unicode, unsigned long p_scan_code,bool p_press,bool p_repeat,int p_modifier_mask) {

	if (!p_press)
		return false;
	
	switch (p_scan_code) {

		case KEY_LEFT: {
			
			if (orientation!=HORIZONTAL)
				return false;
			get_range()->set( get_range()->get() - get_range()->get_step() );
				
		} break;
		case KEY_RIGHT: {
	
			if (orientation!=HORIZONTAL)
				return false;
			get_range()->set( get_range()->get() + get_range()->get_step() );
			
		} break;
		case KEY_UP: {
			
			if (orientation!=VERTICAL)
				return false;
			
			get_range()->set( get_range()->get() - get_range()->get_step() );
			
				
		} break;
		case KEY_DOWN: {
	
			if (orientation!=VERTICAL)
				return false;
			get_range()->set( get_range()->get() + get_range()->get_step() );
			
		} break;
		case KEY_HOME: {
				
			get_range()->set( get_range()->get_min() );
			
		} break;
		case KEY_END: {
	
			get_range()->set( get_range()->get_max() );
			
		} break;

		default:
			return false;
			
	}
	
	return true;
}

void Slider::resize(const Size& p_new_size) {

	size=p_new_size;

}

Slider::Slider(Orientation p_orientation)
{
	
	orientation=p_orientation;
	
	if (p_orientation==HORIZONTAL)
		set_fill_vertical(false);
	else
		set_fill_horizontal(false);

	drag.active=false;	
	
	set_focus_mode( FOCUS_ALL );

}



Slider::~Slider()
{
}


}
