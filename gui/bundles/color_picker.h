//
// C++ Interface: color_picker
//
// Description: 
//
//
// Author: Juan Linietsky <red@lunatea>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef GUICOLOR_PICKER_H
#define GUICOLOR_PICKER_H

#include "widgets/slider.h"
#include "containers/box_container.h"


namespace GUI {

/**
	@author Juan Linietsky <red@lunatea>
*/
class ColorPicker : public HBoxContainer {
	
	
	class ColorShow : public Widget {
			
		Color color;
	public:		
		void set_color(Color p_color) { color=p_color; update(); }
		void draw(const Point& p_global,const Size& p_size,const Rect& p_exposed);
	};
	
	HSlider *r_slider;
	HSlider *g_slider;
	HSlider *b_slider;
	HSlider *a_slider;
	ColorShow *show_color;
	Color color;
	unsigned char alpha;
	bool use_alpha;
	
	void color_changed_callback(double);
	bool updating;
	
public:
	
	Signal< Method > color_changed_signal;
	
	
	void set_color(const Color &p_color, int p_alpha=-1);
	Color get_color() const;
	unsigned char get_alpha() const;
	
	ColorPicker(bool p_with_alpha=false);

    ~ColorPicker();

};

}

#endif
