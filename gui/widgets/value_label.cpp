//
// C++ Implementation: value_label
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "value_label.h"
#include "base/painter.h"
#include "base/skin.h"

namespace GUI {

Size ValueLabel::get_minimum_size_internal() {
	
	if (!get_painter() || !range) {
		return Size();
	}

	/* Compute how many decimals, integers and sign will be needed (max displayable characters) */

	double r=range->get_step();
	int r_int=(int)range->get_step();
	// never use more than 4 decimals, otherwise doubles become unprecise
	int max=4;
	int decs=0;
	while ( (r-(double)r_int)!=0 && max) {
		
		r-=(double)r_int;
		r*=10.0;
		max--;
		decs++;
		r_int=(int)r;
	}


	int maxint=ABS((int)range->get_max());
	int minint=ABS((int)range->get_min());
	int integers=0;
	bool sign=false;


	if (maxint>minint) {
		r_int=maxint;
		if (range->get_max()<0)
			sign=true;
	} else {
		r_int=minint;
		if (range->get_min()<0)
			sign=true;
		sign=true;
	}

	
	while (r_int) {

		r_int/=10;
		integers++;
	}
	
	if (integers==0)
		integers=1;
	int total_size=integers;
	total_size+=decs?(decs+1):0;
	if (sign)
		total_size++; //add sign too!

	/* Since we know how many chars max this will display, also find the biggest character size */
	int char_minwidth=0;

	for (int i=0;i<=9;i++) {

		int charw=get_painter()->get_font_char_width( font( FONT_LABEL ), '0'+i );
		if (charw>char_minwidth)
			char_minwidth=charw;

	}

	/* Finally we know the maximum size this range can take horizontally! */
	Size min;
	
	min.width=char_minwidth*total_size + constant( C_LABEL_MARGIN )*2;
	min.height=get_painter()->get_font_height( font(FONT_LABEL) ) + constant( C_LABEL_MARGIN )*2;
	
	if (suffix.length())
		min.width+=get_painter()->get_font_string_width( font(FONT_LABEL), suffix );
	
	return min;
	
}


void ValueLabel::draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed) {

	if (!range)
		return;
	
	Size string_size;

	String text=range->get_as_text();
			
	if (suffix.length())
		text+=suffix;
	
	string_size.width=get_painter()->get_font_string_width( font(FONT_LABEL), text );
	string_size.height=get_painter()->get_font_height( font(FONT_LABEL) );
	
	int y_ofs=constant( C_LABEL_MARGIN )+((p_size.height-constant( C_LABEL_MARGIN )*2)-string_size.height)/2+get_painter()->get_font_ascent( font(FONT_LABEL) );
	int x_ofs=0;
	
	
	switch (align) {
		
		case ALIGN_LEFT: {
			
			x_ofs=constant( C_LABEL_MARGIN );
		} break;
		case ALIGN_CENTER: {
			
			x_ofs=constant( C_LABEL_MARGIN )+((p_size.width-constant( C_LABEL_MARGIN )*2)-string_size.width)/2;
			
		} break;
		case ALIGN_RIGHT: {
			
			x_ofs=p_size.width-constant( C_LABEL_MARGIN )-string_size.width;
		} break;
		
	}
	
	get_painter()->draw_text( font(FONT_LABEL), Point( x_ofs, y_ofs ), text, color(COLOR_LABEL_FONT) );
	
	
}

void ValueLabel::range_changed() {

	check_minimum_size();
}
void ValueLabel::range_value_changed(double p_to_what) {


	update();
}
void ValueLabel::set_align(Align p_align) {
	
	align=p_align;
	update();
}
ValueLabel::Align ValueLabel::get_align(){
	
	return align;
}

void ValueLabel::set_range(RangeBase *p_range) {

	if (p_range) {
		p_range->range_changed_signal.connect(this,&ValueLabel::range_changed );
		p_range->value_changed_signal.connect(this,&ValueLabel::range_value_changed );
	}
	range=p_range;
	check_minimum_size();

}
void ValueLabel::set_suffix(String p_suffix) {
	
	suffix=p_suffix;
	update();
	check_minimum_size();	
}

ValueLabel::ValueLabel() {

	range=0;
	align=ALIGN_CENTER;
}


ValueLabel::~ValueLabel()
{
}


}
