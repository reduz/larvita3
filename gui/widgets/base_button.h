//
// C++ Interface: base_button
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIBASE_BUTTON_H
#define PIGUIBASE_BUTTON_H


#include "base/widget.h"

namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class BaseButton : public Widget {

public:

	enum DrawMode {
		DRAW_NORMAL,
		DRAW_PRESSED,
		DRAW_HOVER,
		DRAW_DISABLED,
	};

private:
	
	
	bool toggle_mode;
	
	struct ButtonStatus {
		
		bool pressed;
		bool hovering;
		bool press_attempt;
		bool pressing_inside;

		bool disabled;

	} status;
	
protected:	
	
	DrawMode get_draw_mode();

	
	/* Protected constructor, since this class cant be used directly */
	BaseButton();
	
	/* Overridable when reimplementing */
	
	virtual void pressed();
	virtual void toggled(bool p_on);
	
	/* Overrided methods */
	
	void mouse_button(const Point& p_pos, int p_button,bool p_press,int p_modifier_mask);
	void mouse_enter();
	void mouse_leave();

	void focus_leave();
	
	bool key(unsigned long p_unicode, unsigned long p_scan_code,bool p_press,bool p_repeat,int p_modifier_mask);

	virtual String get_type();
	
	bool is_toggle_mode() { return toggle_mode; }

public:

	void set_disabled(bool p_disabled);
	bool is_disabled() const;
	
	/* Signals */
	
	Signal<> pressed_signal;
	Signal< Method1<bool> > toggled_signal;
	Signal< Method1<bool> > hover_signal;

	bool is_pressed(); ///< return wether button is pressed (toggled in)
	void set_pressed(bool p_pressed); ///only works in toggle mode
	void set_toggle_mode(bool p_on);
	
	~BaseButton();

};

}

#endif
