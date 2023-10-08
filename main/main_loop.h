//
// C++ Interface: main_loop
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MAIN_LOOP_H
#define MAIN_LOOP_H

#include "os/input_event.h"

/**
	@author ,,, <red@lunatea>
*/
class MainLoop {

public:	
	virtual void event(const InputEvent& p_event)=0; ///< Ask to handle an event (called before iteration() )
	virtual void request_quit()=0; ///< The host OS/Device may request to quit the app, if so this will be called, remember to return true on iteration() to actually exit
	virtual void init()=0; ///< Will be called before iterations happen, at the begining (after Main::run() is called )
	virtual bool iteration(float p_time)=0; ///< Iteration, return true to exit event loop and stop iterating, false otherwise. Iteration interval time happens in SECONDS.
	virtual void draw()=0; ///< draw request, called from the draw thread
	virtual void finish()=0; ///< Will be called after iterations happen (after an iteration returns true), at the end (when Main::run() ends )	
	
	virtual void set_main_loop(MainLoop *p_loop){} ///< this can be used to bootsrap another main loop
	MainLoop();
	virtual ~MainLoop();


};
#endif
