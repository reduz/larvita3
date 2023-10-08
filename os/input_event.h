/**
 * @file    event.h
 * @brief   Definition of class GUI::Event
 * @date    Sat Jun 16 2001
 * @version $Id: event.h,v 1.8 2003/02/05 02:00:23 punto Exp $
 *
 * Suggestions:
 * @todo
 * 	Joystick events!
 *
 */

#ifndef INPUT_EVENT_H
#define INPUT_EVENT_H

#include "typedefs.h"

/**
 * Input Event classes. These are used in the main loop.
 * The events are pretty obvious.
 */

enum {
	JOY_AXIS_MAX=1024
};

/**
 * Input Modifier Status
 * for keyboard/mouse events.
 */
struct InputModifierStatus {

	bool shift;
	bool alt;
	bool control;
	bool meta; //< windows/mac key

	bool operator==(const InputModifierStatus& rvalue) {

		return ( (shift==rvalue.shift) && (alt==rvalue.alt) && (control==rvalue.control) );
	}

	InputModifierStatus() { shift=alt=control=meta=false; }
};






struct InputEventKey  {

	InputModifierStatus mod;
		
	bool pressed; /// otherwise release

	Uint16 scancode; ///< check keyboard.h , KeyCode enum, without modifier masks
	Uint16 unicode; ///unicode

	bool echo; /// true if this is an echo key

	InputEventKey() { pressed=echo=false; scancode=unicode=0; }
};


struct InputEventMouse  {

	InputModifierStatus mod;
	int pointer_index;
	Sint16 x,y;
	Sint16 global_x,global_y;
	
	InputEventMouse() { x=y=global_x=global_y=0; }

};

struct InputEventMouseButton : public InputEventMouse {

	
	Uint8 button_index;
	bool pressed; //otherwise released
	bool doubleclick; //last even less than doubleclick time
	
	
	InputEventMouseButton() { button_index=0; pressed=false; doubleclick=false; }
};

struct InputEventMouseMotion : public InputEventMouse {

	Uint8 button_mask; //each bit is a button
	Sint16 relative_x,relative_y;
	
	InputEventMouseMotion() { button_mask=0; }
};

struct InputEventJoystickMotion  {

	Uint8 axis; ///< Joystick axis
	int axis_value; ///< value from -JOY_AXIS_MAX to JOY_AXIS_MAX

	InputEventJoystickMotion() { axis=0; axis_value=0; }
};

struct InputEventJoystickButton  {

	Uint8 button_index;
	bool pressed;

	InputEventJoystickButton() { button_index=0;}
};


struct InputEvent {

	enum Type {
		NONE,
		KEY,
		MOUSE_MOTION,
		MOUSE_BUTTON,
		JOYSTICK_MOTION,
		JOYSTICK_BUTTON
	};

	Type type;
	int device_index;

	InputEventMouseMotion mouse_motion;
	InputEventMouseButton mouse_button;
	InputEventJoystickMotion joy_motion;
	InputEventJoystickButton joy_button;
	InputEventKey key;

	InputEvent() { type=NONE; }
};

#endif

