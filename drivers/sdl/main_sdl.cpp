//
// C++ Implementation: main_sdl
//
// Description:
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "main_sdl.h"

#ifdef SDL_ENABLED

#include "drivers/opengl/gl_extensions.h"
#include "os/memory_pool_static.h"
#include "os/time.h"
#define SDL_JOY_MAX 32768

#define SDL_ANALOG_EPSILON 128

#define MAX_SKIP_ITERATIONS 4

Main *Main_SDL::create_func_SDL(const VideoMode& p_preferred) {

	return memnew( Main_SDL(p_preferred) );
}

void Main_SDL::set_mouse_show(bool p_show) {

	SDL_ShowCursor(p_show);
}

void Main_SDL::get_mouse_pos(int &x, int &y) const {

	SDL_GetMouseState(&x,&y);
}

void Main_SDL::set_mouse_grab(bool p_grab) {

	SDL_WM_GrabInput(p_grab?SDL_GRAB_ON:SDL_GRAB_OFF);
}

bool Main_SDL::is_mouse_grab() const {

	return SDL_WM_GrabInput(SDL_GRAB_QUERY);
};

void Main_SDL::correct_analog(int& p_x, int& p_y, int p_epsilon) {
	
	if (p_epsilon == -1)
		p_epsilon = SDL_ANALOG_EPSILON;
	
	int dist = Math::sqrt(p_x * p_x + p_y * p_y);

	if (dist < p_epsilon) {
		p_x = 0;
		p_y = 0;
		return;
	};
	float r = (float)(dist - p_epsilon) / (float)(JOY_AXIS_MAX - p_epsilon);
	r = (r * JOY_AXIS_MAX) / dist;

	p_x = (int)((float)p_x * r);
	p_y = (int)((float)p_y * r);
};


void Main_SDL::set_window_title(String p_title) {

	SDL_WM_SetCaption(p_title.utf8().get_data(), p_title.utf8().get_data());
}
void Main_SDL::set_video_mode(const VideoMode& p_video_mode) {

	const SDL_VideoInfo *vinfo=SDL_GetVideoInfo();

	int width = p_video_mode.width;
	int height = p_video_mode.height;

	Uint32 flags=0;
	if (p_video_mode.fullscreen) {

		flags = SDL_FULLSCREEN | SDL_OPENGL;
		width = vinfo->current_w;
		height = vinfo->current_h;
	} else {
		if (p_video_mode.resizable) {
			flags = SDL_RESIZABLE;
		};
		flags |= SDL_OPENGL;
	};

	screen = SDL_SetVideoMode(width, height, vinfo->vfmt->BitsPerPixel, flags);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	current_video_mode=p_video_mode;
	current_video_mode.width = width;
	current_video_mode.height = height;
}
VideoMode Main_SDL::get_video_mode() const {

	return current_video_mode;
}
void Main_SDL::get_fullscreen_mode_list(List<VideoMode> *p_list) const {

	SDL_Rect **modes;
	int i;

	/* Get available fullscreen/hardware modes */
	modes=SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_OPENGL);


	/* Check if there are any modes available */
	if(modes == (SDL_Rect **)0){
  		return;
	}

	/* Check if our resolution is restricted */
	if(modes == (SDL_Rect **)-1) {
  		return;

	} else{
  		/* Print valid modes */

  		VideoMode vm;
  		vm.width=modes[i]->w;
  		vm.height=modes[i]->h;
  		vm.fullscreen=true;
  		p_list->push_back(vm);
	}

}

String Main_SDL::get_platform_name() {

	return "SDL+"+Renderer::get_singleton()->get_name();
}

String Main_SDL::get_OS_name() {

	#ifdef WINDOWS_ENABLED
		return "WINDOWS";
	#else

		#ifdef POSIX_ENABLED
			#ifdef OSX_ENABLED
				return "OSX";
			#else
				return "LINUX";
			#endif
		#else
			#error No OS?
		#endif
	#endif
};


void Main_SDL::set_main_loop(MainLoop *p_main_loop) {

	if (main_loop)
		main_loop->set_main_loop(p_main_loop);
	else
		main_loop=p_main_loop;
}
MainLoop *Main_SDL::get_main_loop() {

	return main_loop;
}

void Main_SDL::check_events() {

	SDL_Event event;


	while (SDL_PollEvent(&event)!=0) {

		switch(event.type) {
			/* SYSTEM EVENTS */
			case SDL_VIDEORESIZE: {

				/*
				int attempts=50; // wait five seconds to get an idle render thread
				do {
					SDL_Delay(100);

				} while (!renderer->is_idle() && attempts--);
				ERR_FAIL_COND(attempts<=0); // couldnt get the render thread idle

				*/

				const SDL_VideoInfo *vinfo=SDL_GetVideoInfo();

				screen = SDL_SetVideoMode  (event.resize.w,event.resize.h,vinfo->vfmt->BitsPerPixel , screen->flags);

				glViewport(0,0,screen->w,screen->h); // should I really do this here? maybe not..
				current_video_mode.width=screen->w;
				current_video_mode.height=screen->h;

			} break;
			case SDL_QUIT: {

				if (main_loop)
					main_loop->request_quit();
			} break;

			default: {

				/*
				if (event.type==SDL_KEYDOWN && event.key.keysym.mod&KMOD_LSHIFT && event.key.keysym.mod&KMOD_LCTRL && event.key.keysym.sym==SDLK_m) {

					printf("printing all mem..\n");
					MemoryPoolStatic::get_singleton()->debug_print_all_memory();
				}
				*/

				InputEvent ievent;
				if (sdl_event_translator.translate_event(event,ievent)) {
					
					// ctrl + shift + m
					if (ievent.type == InputEvent::KEY && ievent.key.pressed && (!ievent.key.echo) &&
						ievent.key.scancode == KEY_m && ievent.key.mod.control && ievent.key.mod.shift) {
						
						printf("printing all mem..\n");
						MemoryPoolStatic::get_singleton()->debug_print_all_memory();
					};
					
					if (main_loop)
						main_loop->event(ievent);
				}

			}

		}   // End switch

	}
}

void Main_SDL::run() {


	ERR_FAIL_COND(!main_loop);
	bool quit_request=false;

	main_loop->init();

	Uint64 last_frame_tick = OSTime::get_singleton()->get_ticks_usec();

	while((!quit_request)) {

		check_events(); // check events

		/* calculate frame time */
		Uint64 tick = OSTime::get_singleton()->get_ticks_usec();
		float interval=(float)(tick-last_frame_tick)/1000000.0; // usec->sec
		last_frame_tick=tick;

		fps_time+=interval;
		if (fps_time>1.0) {

			fps=fps_count;
			fps_time=0;
			fps_count=0;
			//printf("fps %f\n",fps);
		}


		quit_request=main_loop->iteration(interval);

		if (renderer->can_render()) { // if rendering is possible

			renderer->frame_begin();

			main_loop->draw();
			SDL_GL_SwapBuffers();
			fps_count++;
			//draw_gui();

			renderer->frame_end();
		}

		//SDL_Delay(1); //for testing for now
	};

	main_loop->finish();

}

Uint32 Main_SDL::get_running_time() {

	return SDL_GetTicks();
}

void Main_SDL::delay(Uint32 p_msec) {

	SDL_Delay(p_msec);
}

void Main_SDL::set_as_default() {

	create_func=create_func_SDL;
}

Main_SDL::Main_SDL(const VideoMode& p_preferred) {

	main_loop=NULL;
	screen=NULL;

	ERR_FAIL_COND( Renderer::get_singleton() ); // renderer must be instanceable

	int err = SDL_InitSubSystem(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_JOYSTICK);

	ERR_FAIL_COND( err != 0);

	SDL_EnableUNICODE(1);
	SDL_EnableKeyRepeat(332, 50);
	SDL_JoystickEventState(SDL_ENABLE);
	for (int i=0; i<SDL_NumJoysticks() && i<MAX_JOYSTICKS; i++) {

//		printf("Enabling joystick %i: %s\n", i, SDL_JoystickName(i));
		joysticks[i] = SDL_JoystickOpen(i);
	};

	fps=0;
	fps_time=0;
	fps_count=0;

	set_video_mode(p_preferred);

	renderer = Renderer::create();

}


Main_SDL::~Main_SDL() {

	memdelete( renderer );
	//SDL_Quit();
}


#endif
