//
// C++ Interface: main_sdl
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MAIN_SDL_H
#define MAIN_SDL_H

#ifdef SDL_ENABLED

#include "SDL/SDL.h"
#include "drivers/sdl/event_translator_sdl.h"
#include "main/main.h"
#include "render/renderer.h"
/**
	@author ,,, <red@lunatea>
*/
class Main_SDL : public Main {

	enum {
		MAX_JOYSTICKS = 8,
	};

	static Main *create_func_SDL(const VideoMode& p_preferred);
	VideoMode current_video_mode;
	MainLoop *main_loop;
	EventTranslator_SDL sdl_event_translator;
	
	SDL_Surface *screen;
	SDL_Joystick *joysticks[MAX_JOYSTICKS];
	Renderer *renderer;
	void check_events();
	
	float fps;
	float fps_time;
	int fps_count;
	
public:


	virtual void set_mouse_show(bool p_show);
	virtual void set_mouse_grab(bool p_grab);
	virtual bool is_mouse_grab() const;
	virtual void get_mouse_pos(int &x, int &y) const;	
	virtual void set_window_title(String p_title);
	virtual void set_video_mode(const VideoMode& p_video_mode);
	virtual VideoMode get_video_mode() const;
	virtual void get_fullscreen_mode_list(List<VideoMode> *p_list) const;
	
	virtual String get_platform_name();
	virtual String get_OS_name();
	
	virtual void set_main_loop(MainLoop *p_main_loop);
	virtual MainLoop *get_main_loop();
	virtual void run();

	virtual Uint32 get_running_time(); // running time in msec
	virtual void delay(Uint32 p_msec);	

	void correct_analog(int& p_x, int& p_y, int p_epsilon=-1);
	
	static void set_as_default();
	
	Main_SDL(const VideoMode& p_preferred);
	~Main_SDL();

};

#endif
#endif
