//
// C++ Implementation: bitmap_button
//
// Description:
//
//
// Author: Juan,,, <red@hororo>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bitmap_button.h"
#include "base/painter.h"
#include "base/skin.h"

namespace GUI {


Size BitmapButton::get_minimum_size_internal() {

	if (normal>=0)
		return get_painter()->get_bitmap_size( normal );
	else if (bitmap( BITMAP_BUTTON_NORMAL)>=0)
		return get_painter()->get_bitmap_size( bitmap( BITMAP_BUTTON_NORMAL) );

	return Size();

}

void BitmapButton::draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed) {

	switch( get_draw_mode() ) {

		case DRAW_NORMAL: {

			if (normal>=0)
				get_painter()->draw_bitmap( normal, Point() );
			else if (bitmap( BITMAP_BUTTON_NORMAL)>=0)
				get_painter()->draw_bitmap( bitmap( BITMAP_BUTTON_NORMAL), Point() );

		} break;
		case DRAW_HOVER: {


			BitmapID bm = hover>=0 ? hover : normal;
			if (bm>=0)
				get_painter()->draw_bitmap( bm, Point() );
			else if (bitmap( BITMAP_BUTTON_HOVER )>=0)
				get_painter()->draw_bitmap( bitmap( BITMAP_BUTTON_HOVER ), Point() );
			else if (bitmap( BITMAP_BUTTON_NORMAL)>=0)
				get_painter()->draw_bitmap( bitmap( BITMAP_BUTTON_NORMAL), Point() );
		} break;
		case DRAW_PRESSED: {

			BitmapID bm = pressed>=0 ? pressed : normal;
			if (bm>=0)
				get_painter()->draw_bitmap( bm, Point() );
			else if (bitmap( BITMAP_BUTTON_PRESSED )>=0)
				get_painter()->draw_bitmap( bitmap( BITMAP_BUTTON_PRESSED ), Point() );
			else if (bitmap( BITMAP_BUTTON_NORMAL)>=0)
				get_painter()->draw_bitmap( bitmap( BITMAP_BUTTON_NORMAL), Point() );

		} break;
        case DRAW_DISABLED: {
            BitmapID bm = disabled>=0 ? disabled : normal;
			if (bm>=0)
				get_painter()->draw_bitmap( bm, Point() );
			else if (bitmap( BITMAP_BUTTON_DISABLED )>=0)
				get_painter()->draw_bitmap( bitmap( BITMAP_BUTTON_DISABLED ), Point() );
			else if (bitmap( BITMAP_BUTTON_NORMAL)>=0)
				get_painter()->draw_bitmap( bitmap( BITMAP_BUTTON_NORMAL), Point() );
        } break;
	}

	if (icon>=0)
		get_painter()->draw_bitmap( icon, (p_size-get_painter()->get_bitmap_size(icon))/2 );

}

void BitmapButton::set_normal_bitmap(BitmapID p_bitmap) {

	normal=p_bitmap;
}
void BitmapButton::set_pressed_bitmap(BitmapID p_bitmap) {

	pressed=p_bitmap;

}
void BitmapButton::set_hover_bitmap(BitmapID p_bitmap) {

	hover=p_bitmap;

}

void BitmapButton::set_disabled_bitmap(BitmapID p_bitmap) {

    disabled=p_bitmap;
}

void BitmapButton::set_icon_bitmap(BitmapID p_bitmap) {

	icon=p_bitmap;

}

BitmapButton::BitmapButton(BitmapID p_normal, BitmapID p_pressed, BitmapID p_hover,BitmapID p_icon,BitmapID p_disabled) {

	normal=p_normal;
	pressed=p_pressed;
	hover=p_hover;
	disabled=p_disabled;
	icon=p_icon;
	set_fill_vertical( false );
}


BitmapButton::~BitmapButton()
{
}


}
