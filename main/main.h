//
// C++ Interface: main
//
// Description:
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MAIN_H
#define MAIN_H

#include "main_loop.h"
#include "rstring.h"
#include "list.h"
/**
	@author Juan Linietsky <reduzio@gmail.com>
 * plz describe typical usage
*/

namespace GUI {

	class Window;
};


struct VideoMode {

	int width,height;
	bool fullscreen;
	bool resizable;
	float get_aspect() const { return (float)width/(float)height; }
	VideoMode(int p_width=640,int p_height=480,bool p_fullscreen=false, bool p_resizable = true) { width=p_width; height=p_height; fullscreen=p_fullscreen; resizable = p_resizable; }
};

class Main {

	static Main* singleton;
protected:

	static Main*(*create_func)(const VideoMode&);

public:


	static Main* get_singleton() { return singleton; }; /** Main is a singleton */

	virtual void set_mouse_show(bool p_show)=0;
	virtual void set_mouse_grab(bool p_grab)=0;
	virtual bool is_mouse_grab() const = 0;
	virtual void get_mouse_pos(int &x, int &y) const=0;
	virtual void set_window_title(String p_title)=0;
	virtual void set_video_mode(const VideoMode& p_video_mode)=0;
	virtual VideoMode get_video_mode() const=0;
	virtual void get_fullscreen_mode_list(List<VideoMode> *p_list) const=0;
	virtual void correct_analog(int& p_x, int& p_y, int p_epsilon=-1)=0;

	virtual String get_platform_name()=0; /** Name of the platform */
	virtual String get_OS_name()=0;
	virtual void set_main_loop(MainLoop *p_main_loop)=0; /** Set the main loop */
	virtual MainLoop *get_main_loop()=0;
	virtual void run()=0; /** run the main loop */

	virtual Uint32 get_running_time()=0; // running time in msec

	virtual void delay(Uint32 p_msec)=0;

	static Main* create(const VideoMode& p_preferred=VideoMode());

	Main();
	virtual ~Main();

};

#endif
