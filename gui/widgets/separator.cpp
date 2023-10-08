//
// C++ Implementation: separator
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "separator.h"
#include "base/painter.h"
#include "base/skin.h"
namespace GUI {


void Separator::draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed) {


	Size size=p_size;
	Point ofs=Point();
	Size minsize=get_painter()->get_stylebox_min_size( stylebox( SB_SEPARATOR ) );

	if (dir==VERTICAL) {
		size.height-=constant( C_SEPARATOR_EXTRA_MARGIN )*2;
		ofs.y+=constant( C_SEPARATOR_EXTRA_MARGIN );
		
	} else {
		size.width-=constant( C_SEPARATOR_EXTRA_MARGIN )*2;
		ofs.x+=constant( C_SEPARATOR_EXTRA_MARGIN );
	}

	if (dir==VERTICAL)
		get_painter()->draw_stylebox( stylebox( SB_SEPARATOR ), ofs+Point( (size.width-minsize.width)/2, 0), Size( minsize.width, size.height ) );
	else
		get_painter()->draw_stylebox( stylebox( SB_SEPARATOR ), ofs+Point( 0, (size.height-minsize.height)/2), Size( size.width, minsize.height ) );


}
Size Separator::get_minimum_size_internal() {


	Size minsize=get_painter()->get_stylebox_min_size( stylebox( SB_SEPARATOR ) );

	if (dir==VERTICAL)
		minsize.width+=constant( C_SEPARATOR_EXTRA_MARGIN )*2;
	else
		minsize.height+=constant( C_SEPARATOR_EXTRA_MARGIN )*2;
	
	return minsize;
}


Separator::Separator(Orientation p_dir) {

	dir=p_dir;
	
}


Separator::~Separator()
{
}


}
