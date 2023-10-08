//
// C++ Interface: scene_main_loop
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SCENE_MAIN_LOOP_H
#define SCENE_MAIN_LOOP_H

#include "main/main_loop.h"
#include "physics/physics.h"
#include "scene/nodes/camera_node.h"
#include "scene/nodes/scene_node.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/



class SceneMainLoop : public MainLoop {
	
	enum {
		
		INPUT_EVENT_BUFFER_MAX=512
	};
	
	
	InputEvent input_event_buffer[INPUT_EVENT_BUFFER_MAX];
	int input_event_count;
	
	
	WeakRef<CameraNode> current_camera;
	IRef<SceneNode> scene_tree;
	
	static SceneMainLoop * singleton;
	Uint32 process_flags;
	Physics *physics;
	float iteration_hz;
	float interval_remainder;
	float interval_time;
	
	
	/* Main Loop - these functions must be called back when overriden (as in, SceneMainLoop::iteration() */
	
	virtual void event(const InputEvent& p_event); 
	virtual void request_quit(); 
	virtual void init();
	virtual bool iteration(float p_time);
	virtual void draw(); 
	virtual void finish(); 
	
protected:	
	
	/* User should override these */
	
	virtual void event_user(const InputEvent& p_event) {};
	virtual void init_user() {};
	virtual void request_quit_user() { };
	virtual bool iteration_user(float p_time) { return false; };
	virtual void draw_user() {};
	virtual void finish_user() {};
	
public:
	
	void set_scene_tree(IRef<SceneNode> p_scene);
	IRef<SceneNode> get_scene_tree();
	
	Signal<Method1<float> > frame_process_signal;
	Signal<Method1<float> > physics_process_signal;
	Signal<Method1<float> > post_frame_process_signal;

	Signal< Method1< IRef<Node> > > node_enter_scene_signal;
	Signal< Method1< IRef<Node> > > node_exit_scene_signal;
	
	/* SceneMainLoop Methods */
	
	static SceneMainLoop *get_singleton();
	
	void set_process_flags(Uint32 p_flags);
	Uint32 get_process_flags() const;
		
	Physics *get_physics();
	
	void set_current_camera(IRef<CameraNode> p_current_camera); ///< set a default camera for default rendering. If NULL, no default rendering happens and user must do it by hand.
	IRef<CameraNode> get_current_camera() const;
		
	SceneMainLoop();
	~SceneMainLoop();

};

#endif
