//
// C++ Interface: desktop_sdl
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIDESKTOP_SDL_H
#define PIGUIDESKTOP_SDL_H

namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class DesktopSDL {

	struct WindowList {
		
		PainterSDL painter;
		Window *window;		
		
		WindowList *next;
	};

	WindowList *window_list;
	WindowList *find_window(Window *p_window);
public:
	
	void update_window(Window * p_window);
	void update_window(Window * p_window, const Rect &p_area);
	
	DesktopSDL();
	~DesktopSDL();

};

}

#endif
