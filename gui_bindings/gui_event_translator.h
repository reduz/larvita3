//
// C++ Interface: gui_event_translator
//
// Description: 
//
//
// Author: Juan Linietsky <red@lunatea>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef GUI_EVENT_TRANSLATOR_H
#define GUI_EVENT_TRANSLATOR_H

#ifndef GUI_DISABLED

#include "gui/base/window.h"
#include "os/input_event.h"
/**
	@author Juan Linietsky <red@lunatea>
*/

/**
 * This translates events from the Event system (InputEvent)
 * to the GUI
 */


class GUI_EventTranslator {


	GUI::Window *main_window;
	int get_mod(const InputModifierStatus& p_status);
public:
	
	
	void push_event(const InputEvent& p_event); ///< push event to the gui 
	
	GUI_EventTranslator(GUI::Window *p_main_window); 
	~GUI_EventTranslator();

};

#endif //gui disabled

#endif
