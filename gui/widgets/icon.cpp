//
// C++ Implementation: icon
//
// Description: 
//
//
// Author: Juan Linietsky <reduz@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "icon.h"
#include "base/painter.h"
#include "base/skin.h"
namespace GUI {


Size Icon::get_minimum_size_internal() {
	
	GUI::BitmapID icon_bitmap = (bitmapid<0) ? bitmap( BITMAP_ICON_DEFAULT ) : bitmapid;
	
	if (icon_bitmap<0)
		return Size();
	
	return get_painter()->get_bitmap_size( icon_bitmap );
}

void Icon::draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed) {
	
	GUI::BitmapID icon_bitmap = (bitmapid<0) ? bitmap( BITMAP_ICON_DEFAULT ) : bitmapid;
	
	if (icon_bitmap<0)
		return;
	
	Point ofs=(p_size-get_painter()->get_bitmap_size( icon_bitmap ))/2;
	
	get_painter()->draw_bitmap( icon_bitmap, ofs );
	
}

String Icon::get_type() {
	
	return "Icon";
}

GUI::BitmapID Icon::get_bitmap() {
	
	return bitmapid;
};

void Icon::set_bitmap(BitmapID p_bitmap) {
	
	bitmapid=p_bitmap;
	check_minimum_size();
	update();
}


Icon::Icon(BitmapID p_bitmap) {
	
	bitmapid=p_bitmap;
}


Icon::~Icon()
{
}


}
