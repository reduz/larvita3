//
// C++ Implementation: knob
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "knob.h"
#include "base/skin.h"
#include "base/painter.h"
#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace GUI {


void Knob::mouse_button(const  Point& p_pos, int p_button,bool p_press,int p_modifier_mask) {

	if (p_button==BUTTON_LEFT) {
	
		if (p_press) {
		
			old_pos=p_pos;
			grab_val=get_range()->get_unit_value();
                	grabbing=true;
		} else {
		
                	grabbing=false;		
		}
	}
}

void Knob::mouse_motion(const  Point& p_pos, const  Point& p_rel, int p_button_mask)  {

      if (!grabbing) {
                old_pos=p_pos;
                return; //not dragging, pointless
        }
        
        Point moved=p_pos-old_pos;
        
//        float turnamount=-moved.y; // y turn
        float turnamount=moved.x; // x turn
        
        turnamount/=100; //?
        if (p_button_mask&KEY_MASK_SHIFT)
                turnamount/=8.0;

	grab_val+=turnamount;
	get_range()->set_unit_value( grab_val );
	        
        old_pos=p_pos;
        
        update();

}


Size Knob::get_minimum_size_internal() {

	return Size( constant( C_KNOB_SIZE) ,  constant( C_KNOB_SIZE) );
}

void Knob::draw(const  Point& p_global,const Size& p_size,const Rect& p_exposed) {

	get_painter()->draw_circle(  Point(), p_size, color (COLOR_KNOB_BG ) );
	
	
	float ang_begin_rad=(constant(C_KNOB_ANGLE_BEGIN)*(M_PI*2.0)/360.0);
        float angle_range=M_PI*2.0-ang_begin_rad*2.0;
        float angle=ang_begin_rad+angle_range*get_range()->get_unit_value();
        
        int handle_y= get_size_cache().height/2-(int)(-cos(angle)*(float)constant(C_KNOB_HANDLE_AT_RADIUS))-constant( C_KNOB_HANDLE_SIZE)/2;
        int handle_x= get_size_cache().width/2+(int)(-sin(angle)*(float)constant(C_KNOB_HANDLE_AT_RADIUS))-constant( C_KNOB_HANDLE_SIZE)/2;
        
        Point from( handle_x, handle_y );
        Point to=from+Point( constant( C_KNOB_HANDLE_SIZE), constant( C_KNOB_HANDLE_SIZE) );
        get_painter()->draw_circle( from, to, color( COLOR_KNOB_HANDLE ) );
	
}

void Knob::range_changed() {

	update();
}

void Knob::value_changed(double p_new_val) {

	update();
}



Knob::Knob() {

	grabbing=false;
}


Knob::~Knob()
{
}


}
