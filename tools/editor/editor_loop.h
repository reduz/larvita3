//
// C++ Interface: editor_loop
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EDITOR_LOOP_H
#define EDITOR_LOOP_H

#include "main/main_loop.h"
#include "base/window.h"
#include "gui_bindings/painter_renderer.h"
#include "gui_bindings/gui_timer.h"
#include "gui_bindings/gui_event_translator.h"
#include "editor/editor_main.h"
#include "editor/editor_skin.h"
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class EditorLoop : public MainLoop {
	
	bool editing;
	
	MainLoop *game_loop;
	bool quit_request;
	
	PainterRenderer *gui_painter;
	GUI_EventTranslator * gui_event_translator;
	GUI_Timer *gui_timer;
	EditorSkin *editor_skin;
	EditorMain * editor_main;
	
	IRef<CameraNode> editor_camera;
	IRef<Light> editor_light;
	
public:
	
	virtual void event(const InputEvent& p_event);
	virtual void request_quit(); 
	virtual void init(); 
	virtual bool iteration(float p_time); 
	virtual void draw();
	virtual void finish();		
	virtual void set_main_loop(MainLoop *p_loop);
			
	EditorLoop();	
	~EditorLoop();

};

#endif
