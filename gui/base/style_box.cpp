//
// C++ Implementation: stylebox
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "style_box.h"

namespace GUI {

StyleBox::StyleBox()
{
	
	for (int i=0;i<4;i++) {
		margins[i]=-1; //dont use forced margin
		bitmap_margins[i]=0;
	}
	
	mode=MODE_NONE;
	flat.margin=1;
	flat.blend=false;
	draw_center=true;
	bitmap=-1;
}

StyleBox::StyleBox(int p_margin,Color p_upleft,Color p_downright) {
	
	mode=MODE_FLAT;
	flat.blend=false;
	flat.margin=p_margin;
	flat.border_upleft=p_upleft;
	flat.border_downright=p_downright;
	draw_center=false;
	bitmap=-1;
	
	for (int i=0;i<4;i++) {
		margins[i]=p_margin;
		bitmap_margins[i]=0;
	}
	
}

StyleBox::StyleBox(int p_margin,Color p_center,Color p_upleft,Color p_downright,bool p_blend) {
	
	mode=MODE_FLAT;
	flat.blend=p_blend;
	flat.margin=p_margin;
	flat.center=p_center;
	flat.border_upleft=p_upleft;
	flat.border_downright=p_downright;
	draw_center=true;
	bitmap=-1;
	
	for (int i=0;i<4;i++) {
		margins[i]=p_margin;
		bitmap_margins[i]=0;
	}
	
}


}
