//
// C++ Implementation: progress_bar
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "progress_bar.h"
#include "base/painter.h"
#include "base/skin.h"

namespace GUI {


Size ProgressBar::get_minimum_size_internal() {

	Painter *p=get_painter();
	if (!p)
		return Size(0,0);

	Size minsize = p->get_stylebox_min_size( stylebox( SB_PROGRESSBAR_FRAME ) );
	minsize.height+=p->get_font_height( font( FONT_PROGRESSBAR ) );
	minsize.height+=constant( C_PROGRESSBAR_MARGIN ) *2;

	return minsize;
}

void ProgressBar::set_suffix(String p_suffix) {

	suffix = p_suffix;
};


void ProgressBar::draw(const Point& p_global,const Size& p_size,const Rect& p_exposed) {

	Painter *p=get_painter();
	if (!p)
		return;

	get_painter()->draw_stylebox( stylebox( SB_PROGRESSBAR_FRAME ), Point(), p_size );
	Size inside_area=p_size-p->get_stylebox_min_size( stylebox( SB_PROGRESSBAR_FRAME ) );
	Point ofs(
			p->get_stylebox_margin( stylebox( SB_PROGRESSBAR_FRAME ), MARGIN_LEFT ),
			p->get_stylebox_margin( stylebox( SB_PROGRESSBAR_FRAME ), MARGIN_TOP )
	 );

	double unit_size=get_range()->get_unit_value();
	if (unit_size>0) {
		Size progress_area=inside_area;
		progress_area.width = (int)((double)progress_area.width*unit_size);
		get_painter()->draw_stylebox( stylebox( SB_PROGRESSBAR_PROGRESS ), ofs, progress_area );
	}

	String string=get_range()->get_as_text()+suffix;
	Size text_size( p->get_font_string_width( font( FONT_PROGRESSBAR ), string ), p->get_font_height( font( FONT_PROGRESSBAR ) ) );

	p->draw_text(font( FONT_PROGRESSBAR), ofs+Point( (inside_area.width-text_size.width)/2,(inside_area.height-text_size.height)/2+p->get_font_ascent( font(FONT_PROGRESSBAR) ) ) , string, color(COLOR_PROGRESSBAR_FONT) );
}
ProgressBar::ProgressBar() {

	suffix="%";
}


ProgressBar::~ProgressBar()
{
}


}
