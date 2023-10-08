//
// C++ Interface: stylebox
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUISTYLE_BOX_H
#define PIGUISTYLE_BOX_H

#include "base/defs.h"
#include "base/geometry.h"
#include "signals/signals.h"

namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

class Rect;
class Painter;

struct StyleBox {
	
	
	enum Mode {
		MODE_NONE, //dont draw anything
		MODE_FLAT, //draw flat, superfast, one color only
		MODE_FLAT_BITMAP, // draw bitmap border, but color for center
		MODE_BITMAP, //draw with bitmaps, this looks great, but it's sightly slower
  		MODE_CUSTOM_METHOD // custom_draw_method is called to draw
	};
	
	Mode mode;
	
	struct Flat { //Used if flat mode is selected, this is very fast, but looks simple
		
		Color border_upleft;
		Color border_downright;
		int margin; //if margin is zero, then no border is drawn
		Color center;
		bool blend;
	} flat;
	
	int margins[4];
	BitmapID bitmap;
	int bitmap_margins[4];
	
	bool draw_center;

	Method2<const Rect&,Painter*> custom_draw_method;
	
	StyleBox();
	StyleBox(int p_margin,Color p_center,Color p_upleft,Color p_downright,bool p_blend=true);	
	StyleBox(int p_margin,Color p_upleft,Color p_downright);

	
};

}

#endif
