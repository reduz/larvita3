//
// C++ Implementation: scroll_bar
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "scroll_bar.h"
#include "base/painter.h"
#include "base/skin.h"


namespace GUI {

bool ScrollBar::focus_by_default=true;

String ScrollBar::get_type() {

	return "ScrollBar";
}


void ScrollBar::set_can_focus_by_default(bool p_can_focus) {

	focus_by_default=p_can_focus;
}

int ScrollBar::get_grabber_size() {

	int total_space=(orientation==VERTICAL)?(size.height-get_margins_size()):(size.width-get_margins_size());

	int grabber_size=0;


	const StyleBox &sb_scrollbar_normal=(orientation==VERTICAL)?stylebox(SB_SCROLLBAR_NORMAL_V):stylebox(SB_SCROLLBAR_NORMAL_H);
	const StyleBox &sb_scrollbar_grabber=(orientation==VERTICAL)?stylebox(SB_SCROLLBAR_GRABBER_V):stylebox(SB_SCROLLBAR_GRABBER_H);
	double gs_d = get_range()->get_max()-get_range()->get_min();

	if (!gs_d)
		return 1;

	gs_d = get_range()->get_page()/gs_d;


	grabber_size=(int)( (double)total_space *gs_d );

	if (grabber_size > total_space )
		grabber_size=total_space;

	if (constant( C_SCROLLBAR_GRABBER_SIZE )>0) {
		if (grabber_size<constant( C_SCROLLBAR_GRABBER_SIZE )) {
			grabber_size=constant( C_SCROLLBAR_GRABBER_SIZE );
		}
	} else {

		Size mins=get_painter()->get_stylebox_min_size( sb_scrollbar_grabber);

		int min=(orientation==VERTICAL)?mins.height:mins.width;

		if (grabber_size<min)
			grabber_size=min;
	}

	return grabber_size;
}

int ScrollBar::get_margins_size() {

	const StyleBox &sb_scrollbar_normal=(orientation==VERTICAL)?stylebox(SB_SCROLLBAR_NORMAL_V):stylebox(SB_SCROLLBAR_NORMAL_H);

	return (orientation==VERTICAL)?
			( get_painter()->get_stylebox_margin( sb_scrollbar_normal , MARGIN_TOP ) +
			get_painter()->get_stylebox_margin( sb_scrollbar_normal , MARGIN_BOTTOM ) ):
			( get_painter()->get_stylebox_margin( sb_scrollbar_normal , MARGIN_LEFT ) +
			get_painter()->get_stylebox_margin( sb_scrollbar_normal , MARGIN_RIGHT ) );

}

double ScrollBar::get_click_pos(const Point& p_pos) {


	int pos=(orientation==VERTICAL)?p_pos.y:p_pos.x;
	int range=(orientation==VERTICAL)?size.height:size.width;

	range-=get_margins_size();
	if (range==0)
		return 0;
	double click_pos=(double)pos/(double)range;

	/*
	double total=get_range()->get_max()-get_range()->get_min();
	click_pos*=total;
	click_pos+=get_range()->get_min();
	*/
	return click_pos;
}

int ScrollBar::get_grabber_offset() {


	int area=(orientation==VERTICAL)?size.height:size.width;


	area-=get_margins_size();

	int grabber_ofs=(int)(get_range()->get_unit_value()*(double)area);

	return grabber_ofs;
}



Size ScrollBar::get_minimum_size_internal() {

	const StyleBox &sb_scrollbar_normal=(orientation==VERTICAL)?stylebox(SB_SCROLLBAR_NORMAL_V):stylebox(SB_SCROLLBAR_NORMAL_H);

	Size min;
	const StyleBox &sb_scrollbar_grabber=(orientation==VERTICAL)?stylebox(SB_SCROLLBAR_GRABBER_V):stylebox(SB_SCROLLBAR_GRABBER_H);

	min.width=get_painter()->get_stylebox_margin( sb_scrollbar_normal , MARGIN_LEFT )+get_painter()->get_stylebox_margin( sb_scrollbar_normal , MARGIN_RIGHT );

	min.height=get_painter()->get_stylebox_margin( sb_scrollbar_normal , MARGIN_TOP )+get_painter()->get_stylebox_margin( sb_scrollbar_normal , MARGIN_BOTTOM );


	if (constant( C_SCROLLBAR_GRABBER_SIZE )>0) {

		if (orientation==VERTICAL) {

			min.width+=constant( C_SCROLLBAR_GRABBER_SIZE )+get_painter()->get_stylebox_min_size( sb_scrollbar_normal ).width;
		} else {

			min.height+=constant( C_SCROLLBAR_GRABBER_SIZE )+get_painter()->get_stylebox_min_size( sb_scrollbar_normal ).height;

		}

	} else {

		if (orientation==VERTICAL) {

			min.width+=get_painter()->get_stylebox_min_size( sb_scrollbar_grabber, true ).width;
		} else {

			min.height+=get_painter()->get_stylebox_min_size( sb_scrollbar_grabber, true ).height;

		}
	}

	return min;


}

void ScrollBar::draw(const Point& p_global,const Size& p_size,const Rect& p_exposed) {

	const StyleBox &sb_scrollbar_normal=(orientation==VERTICAL)?stylebox(SB_SCROLLBAR_NORMAL_V):stylebox(SB_SCROLLBAR_NORMAL_H);

	const StyleBox &sb_scrollbar_grabber=(orientation==VERTICAL)?stylebox(SB_SCROLLBAR_GRABBER_V):stylebox(SB_SCROLLBAR_GRABBER_H);

	get_painter()->draw_stylebox( sb_scrollbar_normal, Point() , p_size );

	if (has_focus())
		get_painter()->draw_stylebox( stylebox( SB_SCROLLBAR_FOCUS ), Point() , p_size );

	int grabber_size=get_grabber_size();
	int grabber_ofs=get_grabber_offset()+ ((orientation==VERTICAL)?( get_painter()->get_stylebox_margin( sb_scrollbar_normal , MARGIN_TOP )):( get_painter()->get_stylebox_margin( sb_scrollbar_normal , MARGIN_LEFT )));



	Point pos;

	if (orientation==VERTICAL) {

		pos=Point( get_painter()->get_stylebox_margin( sb_scrollbar_normal , MARGIN_LEFT ), grabber_ofs );

	} else {
		pos=Point( grabber_ofs, get_painter()->get_stylebox_margin( sb_scrollbar_normal , MARGIN_TOP ) );
	}


	int minperp=(orientation==VERTICAL)?get_painter()->get_stylebox_min_size( sb_scrollbar_normal ).width : get_painter()->get_stylebox_min_size( sb_scrollbar_normal ).height;

	if (orientation==VERTICAL) {
	
		Size gbsize( size.width-minperp, get_grabber_size()+1 );
		get_painter()->draw_stylebox( sb_scrollbar_grabber, pos , gbsize );
		
		if (bitmap( BITMAP_SCROLLBAR_GRIP_V)!=INVALID_BITMAP_ID ) {
		
			Point grip_pos=pos + (gbsize-get_painter()->get_bitmap_size( bitmap(BITMAP_SCROLLBAR_GRIP_V) ))/2;

			get_painter()->draw_bitmap( bitmap(BITMAP_SCROLLBAR_GRIP_V), grip_pos);
		}
	} else {
	
		Size gbsize( get_grabber_size()+1 , size.height-minperp );
		get_painter()->draw_stylebox( sb_scrollbar_grabber, pos , gbsize );
		
		if (bitmap( BITMAP_SCROLLBAR_GRIP_H)!=INVALID_BITMAP_ID ) {
		
			get_painter()->draw_bitmap( bitmap(BITMAP_SCROLLBAR_GRIP_H), pos + (gbsize-get_painter()->get_bitmap_size( bitmap(BITMAP_SCROLLBAR_GRIP_H) ))/2);
		}
		
	}


}
void ScrollBar::mouse_button(const Point& p_pos, int p_button,bool p_press,int p_modifier_mask) {

	// wheel!


	if (p_button==BUTTON_WHEEL_UP && p_press) {

		if (orientation==VERTICAL)
			get_range()->set( get_range()->get() - get_range()->get_page() / 4.0 );
		else
			get_range()->set( get_range()->get() + get_range()->get_page() / 4.0 );

	}
	if (p_button==BUTTON_WHEEL_DOWN && p_press) {

		if (orientation==HORIZONTAL)
			get_range()->set( get_range()->get() - get_range()->get_page() / 4.0 );
		else
			get_range()->set( get_range()->get() + get_range()->get_page() / 4.0  );
	}

	if (p_button!=BUTTON_LEFT)
		return;

	if (p_press) {

		int ofs = orientation==VERTICAL ? p_pos.y : p_pos.x ;
		int grabber_ofs = get_grabber_offset();
		int grabber_size = get_grabber_size();
		
		if ( ofs < grabber_ofs ) {
			
			get_range()->set( get_range()->get() - get_range()->get_page() );
			
		} else if (ofs > grabber_ofs + grabber_size ) {
			
			get_range()->set( get_range()->get() + get_range()->get_page() );
			
		} else {
		
			
			drag.active=true;
			drag.pos_at_click=get_click_pos(p_pos);
			drag.value_at_click=get_range()->get_unit_value();
		}


	} else {

		drag.active=false;
	}

}
void ScrollBar::mouse_motion(const Point& p_pos, const Point& p_rel, int p_button_mask) {

	if (!drag.active)
		return;

	double value_ofs=drag.value_at_click+(get_click_pos(p_pos)-drag.pos_at_click);


	value_ofs=value_ofs*( get_range()->get_max() - get_range()->get_min() );
	if (value_ofs<get_range()->get_min())
		value_ofs=get_range()->get_min();
	if (value_ofs>(get_range()->get_max()-get_range()->get_page()))
		value_ofs=get_range()->get_max()-get_range()->get_page();
	if (get_range()->get()==value_ofs)
		return; //dont bother if the value is the same

	get_range()->set( value_ofs );

}

bool ScrollBar::key(unsigned long p_unicode, unsigned long p_scan_code,bool p_press,bool p_repeat,int p_modifier_mask) {

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

void ScrollBar::resize(const Size& p_new_size) {

	size=p_new_size;

}




ScrollBar::ScrollBar(Orientation p_orientation)
{

	orientation=p_orientation;

	if (p_orientation==HORIZONTAL)
		set_fill_vertical(false);
	else
		set_fill_horizontal(false);

	drag.active=false;

	if (focus_by_default)
		set_focus_mode( FOCUS_ALL );

}



ScrollBar::~ScrollBar()
{
}


}
