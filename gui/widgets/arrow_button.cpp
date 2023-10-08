
//
// C++ Implementation: arrow_button
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "arrow_button.h"
#include "base/painter.h"

#include "base/skin.h"

namespace GUI {


void ArrowButton::set_page_increment(bool p_increment) {

	page_increment=p_increment;
}

void ArrowButton::_check_auto_hide() {

	if (!range)
		return;

	if (auto_hide) {
		if ( ((range->get_max()-range->get_min())-range->get_page()) <=0 )
			hide();
		else {
			show();
		}
	}

}

void ArrowButton::_range_changed() {

	_check_auto_hide();
	update();

}

BitmapID ArrowButton::get_bitmap() {

	switch( dir ) {
	
		case UP: {
				
			return bitmap( BITMAP_ARROW_UP );
		} break;
		case DOWN: {
			return bitmap( BITMAP_ARROW_DOWN );
		
		} break;
		case LEFT: {
			return bitmap( BITMAP_ARROW_LEFT );
		
		} break;
		case RIGHT: {
			return bitmap( BITMAP_ARROW_RIGHT );
		
		} break;
	}
	
	return INVALID_BITMAP_ID;
}

void ArrowButton::press_slot(){

    switch( dir )
    {
        case GUI::UP:
        case GUI::LEFT:
        	
            range->set( range->get()-(page_increment?(range->get_page()/4.0):range->get_step()) );

        break;

        case GUI::DOWN:
        case GUI::RIGHT:
            range->set( range->get()+(page_increment?(range->get_page()/4.0):range->get_step()) );
        break;
    }
}

void ArrowButton::set_dir(Direction p_dir) {

	dir=p_dir;
	update();
}

Size ArrowButton::get_minimum_size_internal() {

	Size min=get_painter()->get_stylebox_min_size( stylebox( SB_ARROWBUTTON_NORMAL ) );


	if (get_bitmap()!=INVALID_BITMAP_ID) {

		min+=get_painter()->get_bitmap_size( get_bitmap() );
	} else if (constant( C_ARROWBUTTON_ARROW_SIZE )>0 && !no_minsize ) {

		min.width+=constant( C_ARROWBUTTON_ARROW_SIZE );
		min.height+=constant( C_ARROWBUTTON_ARROW_SIZE );

	}

	min.height+=constant( C_ARROWBUTTON_DISPLACEMENT );
	min.width+=constant( C_ARROWBUTTON_DISPLACEMENT );
	return min;


}
void ArrowButton::set_range( RangeBase *p_range ){

    range = p_range;
    if (range)
        range->range_changed_signal.connect( this, &ArrowButton::_range_changed );
    pressed_signal.connect( this, &ArrowButton::press_slot );
}
void ArrowButton::draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed) {

	Painter *p=get_painter();

	/* Draw Outline */

	//if disabled...
	bool draw_displaced=false;
	switch( get_draw_mode() ) {


		case DRAW_HOVER: { //optative, otherwise draw normal

			if (stylebox( SB_ARROWBUTTON_HOVER ).mode!=StyleBox::MODE_NONE) {

				p->draw_stylebox( stylebox( SB_ARROWBUTTON_HOVER ) , Point() , p_size, p_exposed );
				break;
			}
		}

		case DRAW_NORMAL: {

			p->draw_stylebox( stylebox( SB_ARROWBUTTON_NORMAL ) , Point() , p_size, p_exposed );

		} break;

		case DRAW_PRESSED: {

			p->draw_stylebox( stylebox( SB_ARROWBUTTON_PRESSED ) , Point() , p_size, p_exposed );
			draw_displaced=true;
		} break;

	}


	Rect area_rect=Rect( p_pos, p_size );



	area_rect.pos.x=p->get_stylebox_margin( stylebox( SB_ARROWBUTTON_NORMAL ), MARGIN_LEFT );
	area_rect.pos.y=p->get_stylebox_margin( stylebox( SB_ARROWBUTTON_NORMAL ), MARGIN_TOP );
	area_rect.size-=area_rect.pos;

	area_rect.size.x-=p->get_stylebox_margin( stylebox( SB_ARROWBUTTON_NORMAL ), MARGIN_RIGHT );
	area_rect.size.y-=p->get_stylebox_margin( stylebox( SB_ARROWBUTTON_NORMAL ), MARGIN_BOTTOM );

	area_rect.size.x-=constant( C_ARROWBUTTON_DISPLACEMENT );
	area_rect.size.y-=constant( C_ARROWBUTTON_DISPLACEMENT );
	if (draw_displaced)
		area_rect.pos+=Point( constant( C_ARROWBUTTON_DISPLACEMENT ), constant( C_ARROWBUTTON_DISPLACEMENT ) );


	if (get_bitmap()!=INVALID_BITMAP_ID) {


		Size bsize = get_painter()->get_bitmap_size( get_bitmap() );
			
		get_painter()->draw_bitmap( get_bitmap(), area_rect.pos + ( area_rect.size - bsize ) /2 );
		
	} else if (constant( C_ARROWBUTTON_ARROW_SIZE )>0) {

		get_painter()->draw_arrow( area_rect.pos, area_rect.size, dir, color( COLOR_ARROWBUTTON_ARROW_COLOR ) );
	}

	if (has_focus())
		p->draw_stylebox( stylebox( SB_ARROWBUTTON_FOCUS ) , Point() , p_size, p_exposed );


}

void ArrowButton::set_auto_hide( bool p_enabled ) {

    auto_hide=p_enabled;
}

Direction ArrowButton::get_dir () const {

	return dir;
};

String ArrowButton::get_type() {


	return "ArrowButton";
}


ArrowButton::ArrowButton(Direction p_dir,bool p_no_minsize)
{
	range=NULL;
	no_minsize=p_no_minsize;
	dir=p_dir;
	auto_hide=false;
	page_increment=false;
}


ArrowButton::~ArrowButton() {

	
}


}
