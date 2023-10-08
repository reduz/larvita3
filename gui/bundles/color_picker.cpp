//
// C++ Implementation: color_picker
//
// Description: 
//
//
// Author: Juan Linietsky <red@lunatea>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "color_picker.h"

#include "base/painter.h"
#include "containers/grid_container.h"
#include "widgets/label.h"

namespace GUI {

void ColorPicker::ColorShow::draw(const Point& p_global,const Size& p_size,const Rect& p_exposed) {
	
	get_painter()->draw_fill_rect(Point(),p_size,color);
}
	
void ColorPicker::set_color(const Color &p_color, int p_alpha) {
	
	color=p_color;
	if (p_alpha>=0 || p_alpha<256) {
		
		alpha=p_alpha;
	}
	show_color->set_color(color);
	
	updating=true;
	
	r_slider->get_range()->set( p_color.r );
	g_slider->get_range()->set( p_color.g );
	b_slider->get_range()->set( p_color.b );
	if (a_slider)
		a_slider->get_range()->set( alpha );
			
	updating=false;
	
}
Color ColorPicker::get_color() const {
	
	return color;	
}
unsigned char ColorPicker::get_alpha() const {
	
	return alpha;
}
	
void ColorPicker::color_changed_callback(double) {
		
	if (updating)
		return;
	
	
	color.r=(unsigned char)r_slider->get_range()->get();
	color.g=(unsigned char)g_slider->get_range()->get();
	color.b=(unsigned char)b_slider->get_range()->get();
	if (!use_alpha)
		set_color(color);
	else {
		alpha=(unsigned char)a_slider->get_range()->get();
		set_color(color,alpha);
	}

	color_changed_signal.call();
	
}
	
ColorPicker::ColorPicker(bool p_with_alpha) {
	
	use_alpha=p_with_alpha;
	
	show_color = add( GUI_NEW( ColorShow ), 1 );
	show_color->set_minimum_size(GUI::Size(50, 0));
	
	GridContainer *g = add( GUI_NEW( GridContainer ), 2 );
	
	g->add( GUI_NEW(Label("R")), false, false );
	r_slider = g->add( GUI_NEW( HSlider ), true, false );
	r_slider->get_range()->config( 0, 255, 0, 1 );
	r_slider->get_range()->value_changed_signal.connect( this, &ColorPicker::color_changed_callback );
	r_slider->set_minimum_size(GUI::Size(100, 0));
	
	g->add( GUI_NEW( Label("G")), false, false );
	g_slider = g->add( GUI_NEW( HSlider), true, false );
	g_slider->get_range()->config( 0, 255, 0, 1 );
	g_slider->get_range()->value_changed_signal.connect( this, &ColorPicker::color_changed_callback );
	
	g->add( GUI_NEW( Label("B")) , false, false );
	b_slider = g->add( GUI_NEW( HSlider ), true, false );
	b_slider->get_range()->config( 0, 255, 0, 1 );
	b_slider->get_range()->value_changed_signal.connect( this, &ColorPicker::color_changed_callback );
	
	if (use_alpha) {
		g->add( GUI_NEW( Label("A")), false, false );
		a_slider = g->add( GUI_NEW( HSlider ), true, false );
		a_slider->get_range()->config( 0, 255, 0, 1 );
		a_slider->get_range()->value_changed_signal.connect( this, &ColorPicker::color_changed_callback );
	} else {
		
		a_slider=0;
	}
	
	
	set_color(Color(0),255);
	updating=false;
	
}


ColorPicker::~ColorPicker()
{
}


}
