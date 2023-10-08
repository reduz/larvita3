//
// C++ Implementation: window_top
//
// Description:
//
//
// Author: Juan Linietsky <reduz@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "window_top.h"

#include "base/painter.h"
#include "base/skin.h"
#include "base/window.h"

namespace GUI {


void WindowTop::set_text(String p_text) {

	text=p_text;
	update();
}

Size WindowTop::get_minimum_size_internal() {

	if (!get_painter()) {
		return Size();
	}

	Size min;

	min.height=get_painter()->get_font_height( font(FONT_WINDOWTOP) ) + get_painter()->get_stylebox_min_size( stylebox( SB_WINDOWTOP ) ).height;

	return min;

}


void WindowTop::draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed) {


	get_painter()->draw_stylebox( stylebox( SB_WINDOWTOP ), Point(), p_size );

	Point ofs=Point (get_painter()->get_stylebox_margin( stylebox( SB_WINDOWTOP ) , MARGIN_LEFT ) ,
			 get_painter()->get_stylebox_margin( stylebox( SB_WINDOWTOP ) , MARGIN_TOP ) );
	ofs.y+=get_painter()->get_font_ascent(font(FONT_WINDOWTOP));
	int theight=p_size.height-get_painter()->get_stylebox_min_size( stylebox( SB_WINDOWTOP ) ).height;
	ofs.y+=(theight-get_painter()->get_font_height( font(FONT_WINDOWTOP) ) ) /2;

	get_painter()->draw_text( font(FONT_WINDOWTOP), ofs, text, color(COLOR_WINDOWTOP_FONT) );

}

void WindowTop::mouse_motion(const Point& p_pos, const Point& p_rel, int p_button_mask) {

    if ( p_button_mask & BUTTON_MASK_LEFT ){

        get_window()->set_pos( get_window()->get_pos() + p_rel );
    }
}


WindowTop::WindowTop(String p_text)
{
	text=p_text;
}


WindowTop::~WindowTop()
{
}


}
