//
// C++ Implementation: window_button
//
// Description:
//
//
// Author: Juan Linietsky <reduz@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "window_button.h"
#include "base/painter.h"
#include "base/skin.h"
#include "base/window.h"

namespace GUI {


void WindowButton::pressed() {

	switch(mode) {

		case MODE_CLOSE: {

			if (get_window())
				get_window()->hide();
		} break;
		case MODE_MIN: {


		} break;
		case MODE_FULL: {


		} break;
	}

}


Size WindowButton::get_minimum_size_internal() {

	Size min=get_painter()->get_stylebox_min_size( stylebox( SB_WINDOWBUTTON_NORMAL ) );

	BitmapID bmp=-1;

	switch(mode) {

		case MODE_CLOSE: {

			bmp=bitmap(BITMAP_WINDOWBUTTON_CLOSE);
		} break;
		case MODE_MIN: {

			bmp=bitmap(BITMAP_WINDOWBUTTON_MINIMIZE);
		} break;
		case MODE_FULL: {

			bmp=bitmap(BITMAP_WINDOWBUTTON_MAXIMIZE);
		} break;
	}



	if (bmp<=0) {

		min.height+=get_painter()->get_font_height(font(FONT_WINDOWBUTTON));
		min.width+=get_painter()->get_font_char_width(font(FONT_WINDOWBUTTON),'X');
	} else {

		min+=get_painter()->get_bitmap_size( bmp );
	}


	min.height+=constant( C_WINDOWBUTTON_DISPLACEMENT );
	min.width+=constant( C_WINDOWBUTTON_DISPLACEMENT );

	return min;

}
void WindowButton::draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed) {

	Painter *p=get_painter();

	/* Draw Outline */

	//if disabled...
	bool draw_displaced=false;
	switch( get_draw_mode() ) {


		case DRAW_HOVER: { //optative, otherwise draw normal

			if (stylebox( SB_WINDOWBUTTON_HOVER ).mode!=StyleBox::MODE_NONE) {

				p->draw_stylebox( stylebox( SB_WINDOWBUTTON_HOVER ) , Point() , p_size, p_exposed );
				break;
			}
		}

		case DRAW_NORMAL: {

			p->draw_stylebox( stylebox( SB_WINDOWBUTTON_NORMAL ) , Point() , p_size, p_exposed );

		} break;

		case DRAW_PRESSED: {

			p->draw_stylebox( stylebox( SB_WINDOWBUTTON_PRESSED ) , Point() , p_size, p_exposed );
			draw_displaced=true;
		} break;

	}


	Rect area_rect=Rect( p_pos, p_size );



	area_rect.pos.x=p->get_stylebox_margin( stylebox( SB_WINDOWBUTTON_NORMAL ), MARGIN_LEFT );
	area_rect.pos.y=p->get_stylebox_margin( stylebox( SB_WINDOWBUTTON_NORMAL ), MARGIN_TOP );
	area_rect.size-=area_rect.pos;

	area_rect.size.x-=p->get_stylebox_margin( stylebox( SB_WINDOWBUTTON_NORMAL ), MARGIN_RIGHT );
	area_rect.size.y-=p->get_stylebox_margin( stylebox( SB_WINDOWBUTTON_NORMAL ), MARGIN_BOTTOM );

	area_rect.size.x-=constant( C_WINDOWBUTTON_DISPLACEMENT );
	area_rect.size.y-=constant( C_WINDOWBUTTON_DISPLACEMENT );
	if (draw_displaced)
		area_rect.pos+=Point( constant( C_WINDOWBUTTON_DISPLACEMENT ), constant( C_WINDOWBUTTON_DISPLACEMENT ) );


	BitmapID bmp;

	switch(mode) {

		case MODE_CLOSE: {

			bmp=bitmap(BITMAP_WINDOWBUTTON_CLOSE);
		} break;
		case MODE_MIN: {

			bmp=bitmap(BITMAP_WINDOWBUTTON_MINIMIZE);
		} break;
		case MODE_FULL: {

			bmp=bitmap(BITMAP_WINDOWBUTTON_MAXIMIZE);
		} break;
	}

	if (bmp>0) {
		Size bmsize=get_painter()->get_bitmap_size( bmp );
		get_painter()->draw_bitmap( bmp, area_rect.pos+(area_rect.size-bmsize)/2 );
	} else {
		Point textpos=area_rect.pos;

		textpos.y+=get_painter()->get_font_ascent( font(FONT_WINDOWBUTTON) );

		switch(mode) {

			case MODE_CLOSE: {

				get_painter()->draw_text( font(FONT_WINDOWBUTTON), textpos,"X",color( COLOR_WINDOWBUTTON_FONT) );
			} break;
			case MODE_MIN: {
				get_painter()->draw_text( font(FONT_WINDOWBUTTON), textpos,"_",color( COLOR_WINDOWBUTTON_FONT) );
			} break;
			case MODE_FULL: {
				get_painter()->draw_text( font(FONT_WINDOWBUTTON), textpos,"^", color( COLOR_WINDOWBUTTON_FONT) );
			} break;
		}
	}

	if (has_focus())
		p->draw_stylebox( stylebox( SB_WINDOWBUTTON_FOCUS ) , Point() , p_size, p_exposed);


}


WindowButton::WindowButton(Mode p_mode) {

	mode=p_mode;
	set_focus_mode( FOCUS_NONE );
}


WindowButton::~WindowButton()
{
}


}
