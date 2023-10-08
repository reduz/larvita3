//
// C++ Implementation: base_button
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "base_button.h"



namespace GUI {

void BaseButton::set_disabled(bool p_disabled) {
	
	status.disabled = p_disabled;
};

bool BaseButton::is_disabled() const {

	return status.disabled;
};

void BaseButton::set_pressed(bool p_pressed) {
	
	if (!toggle_mode)
		return;
	status.pressed=p_pressed;
	update();
}

bool BaseButton::is_pressed() {
	
	return status.pressed;
}


BaseButton::DrawMode BaseButton::get_draw_mode() {
	
	if (status.disabled) {
		return DRAW_DISABLED;
	};
	
	if (status.press_attempt==false && status.hovering && !status.pressed) {
		
		
		return DRAW_HOVER;
	} else {
		/* determine if pressed or not */
				
		bool pressing;
		if (status.press_attempt) {
			
			pressing=status.pressing_inside;
			if (status.pressed)
				pressing=!pressing;
		} else {
			
			pressing=status.pressed;
		}
		
		if (pressing) 
			return DRAW_PRESSED;
		else			
			return DRAW_NORMAL;
	}	
	
	return DRAW_NORMAL;
}

void BaseButton::set_toggle_mode(bool p_on) {
	
	toggle_mode=p_on;
}

void BaseButton::pressed() {
	

	
}

void BaseButton::toggled(bool p_on) {
	
	
	
}

void BaseButton::mouse_enter() {
	
	if (status.press_attempt)
		status.pressing_inside=true;
	status.hovering=true;
	update();
	hover_signal.call(true);
	

	
}
void BaseButton::mouse_leave() {
	
	if (status.press_attempt)
		status.pressing_inside=false;
	
	status.hovering=false;
	update();
	hover_signal.call(false);
	
	
}
void BaseButton::mouse_button(const Point& p_pos, int p_button,bool p_press,int p_modifier_mask){
	
	if ( status.disabled || p_button!=BUTTON_LEFT )
		return;
	
	if (p_press) {
		
		status.press_attempt=true;
		status.pressing_inside=true;
		
	} else {
		
		
		if (status.press_attempt &&status.pressing_inside) {

			status.press_attempt=false;
		
			if (!toggle_mode) { //mouse press attempt
			
				pressed();
				
				pressed_signal.call();
				
				
			} else {
			
				status.pressed=!status.pressed;
				
				pressed();
				pressed_signal.call();
				
				toggled(status.pressed);
				toggled_signal.call(status.pressed);
				
			
			}

		}
	
		status.press_attempt=false;
		
	}
	
	update();			
}	


void BaseButton::focus_leave() {

	status.press_attempt=false;
}

bool BaseButton::key(unsigned long p_unicode, unsigned long p_scan_code,bool p_press,bool p_repeat,int p_modifier_mask) {

	if (status.disabled)
		return false;
	
	if (p_scan_code==KEY_SPACE) {


		if (p_press)
			status.press_attempt=true;
		
		else if (status.press_attempt) {

			status.press_attempt=false;
		
			if (!toggle_mode) { //mouse press attempt
			
				pressed();
				pressed_signal.call();
				
			} else {
			
				status.pressed=!status.pressed;
				
				pressed();
				pressed_signal.call();
				
				toggled(status.pressed);
				toggled_signal.call(status.pressed);
			
			}
		}

		
		
		update();

		return true;
	}

	return false;
}

String BaseButton::get_type() {
	
	return "Base Button";

}

BaseButton::BaseButton() {
	
	toggle_mode=false;
	status.pressed=false;
	status.press_attempt=false;
	status.hovering=false;
	status.pressing_inside=false;
	status.disabled = false;
	set_focus_mode( FOCUS_ALL );
}

BaseButton::~BaseButton()
{
}


}
