//
// C++ Implementation: gui_event_translator
//
// Description: 
//
//
// Author: Juan Linietsky <red@lunatea>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "gui_event_translator.h"
#include "os/keyboard.h"

#ifndef GUI_DISABLED

int GUI_EventTranslator::get_mod(const InputModifierStatus& p_status) {

	unsigned int modmask=0;
	if (p_status.alt)
		modmask|=KEY_MASK_ALT;
	if (p_status.shift)
		modmask|=KEY_MASK_SHIFT;
	if (p_status.control)
		modmask|=KEY_MASK_CTRL;
	if (p_status.meta)
		modmask|=KEY_MASK_META;
	
	return modmask;

}

void GUI_EventTranslator::push_event(const InputEvent& p_event) {

	switch (p_event.type) {
		
		
		case InputEvent::KEY: {
			
			unsigned int modmask=get_mod( p_event.key.mod ) ;
			
			main_window->key( p_event.key.unicode, p_event.key.scancode, p_event.key.pressed, p_event.key.echo, modmask );
			
		} break;
		case InputEvent::MOUSE_BUTTON: {
			
			unsigned int modmask=get_mod( p_event.mouse_button.mod );
			
			main_window->mouse_button( GUI::Point( p_event.mouse_button.x, p_event.mouse_button.y ), p_event.mouse_button.button_index, p_event.mouse_button.pressed, modmask );
			if (p_event.mouse_button.doubleclick)
				main_window->mouse_doubleclick( GUI::Point( p_event.mouse_button.x, p_event.mouse_button.y ), modmask );
			
		} break;
		case InputEvent::MOUSE_MOTION: {
			
			//unsigned int modmask=get_mod( p_event.mouse_motion.mod );
						
			main_window->mouse_motion( GUI::Point( p_event.mouse_motion.x, p_event.mouse_motion.y ), GUI::Point( p_event.mouse_motion.relative_x, p_event.mouse_motion.relative_y) , p_event.mouse_motion.button_mask);
			
			
		} break;
		default: {};
	}

}

GUI_EventTranslator::GUI_EventTranslator(GUI::Window *p_main_window) {
	
	main_window=p_main_window;
}


GUI_EventTranslator::~GUI_EventTranslator() {
	
}


#endif // gui disabled
