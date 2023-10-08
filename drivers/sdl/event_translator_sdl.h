//
// C++ Interface: event_translator_sdl
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EVENT_TRANSLATOR_SDL_H
#define EVENT_TRANSLATOR_SDL_H

#ifdef SDL_ENABLED
#include "os/input_event.h"
#include <SDL/SDL_events.h>
#include "os/keyboard.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class EventTranslator_SDL {
	
	static const unsigned int sdl_key_table[KEY_MAX];	
	int last_key_pressed;
public:
	bool translate_event(const SDL_Event& p_sdl_event, InputEvent& p_ievent);
	
	EventTranslator_SDL();
	~EventTranslator_SDL();

};

#endif
#endif

