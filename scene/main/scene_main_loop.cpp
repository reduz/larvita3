//
// C++ Implementation: scene_main_loop
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "scene_main_loop.h"

#include "render/renderer.h"
#include "performance_metrics.h"
#include "print_string.h"


SceneMainLoop* SceneMainLoop::singleton=NULL;

SceneMainLoop *SceneMainLoop::get_singleton() {
	
	return singleton;	
}
	

void SceneMainLoop::event(const InputEvent& p_event) {
	
	if (input_event_count>=INPUT_EVENT_BUFFER_MAX)
		return;
	
	input_event_buffer[input_event_count++]=p_event;
	
}
void SceneMainLoop::request_quit() {
	
	request_quit_user();
}
void SceneMainLoop::init() {
	
	init_user();
}


bool SceneMainLoop::iteration(float p_time) {

	
	if (p_time>0.5) // avoid problems when loading, debugging, etc
		p_time=0.5;	
	
	float frame_time=1.0/iteration_hz;
	interval_remainder+=p_time;	
	
	if (interval_remainder<frame_time)
		return false;

	// input events
	
	for (int i=0;i<input_event_count;i++) {
		
		event_user(input_event_buffer[i]);
	}
	input_event_count=0;
	
	PerformanceMetrics::begin_section_frame( PerformanceMetrics::SECTION_PROCESS );
	
	int slice_count=(int)(interval_remainder/frame_time);
	float slice_time=slice_count*frame_time;
	

	
	//printf("time: %g, frame_time %g\n",p_time,frame_time);
	//printf("interval remainder %g, slice time %g\n",interval_remainder,slice_time);
	
	PerformanceMetrics::begin_area( PerformanceMetrics::SECTION_PROCESS, "frame_process" );
	
	frame_process_signal.call(slice_time);
	
	PerformanceMetrics::end_area( PerformanceMetrics::SECTION_PROCESS, "frame_process" );
	
	//printf("slice count %i, slice time %g\n",slice_count,slice_time);
	PerformanceMetrics::begin_section_frame( PerformanceMetrics::SECTION_PHYSICS );
	for (int i=0;i<slice_count;i++) {

		if ((process_flags & Node::MODE_FLAG_GAME) && !(process_flags & Node::MODE_FLAG_PAUSE)) {

			PerformanceMetrics::begin_area( PerformanceMetrics::SECTION_PHYSICS, "physics_process" );
			physics_process_signal.call(frame_time);
			PerformanceMetrics::end_area( PerformanceMetrics::SECTION_PHYSICS, "physics_process" );
			
			PerformanceMetrics::begin_area( PerformanceMetrics::SECTION_PHYSICS, "physics" );
			get_physics()->process(frame_time);
			PerformanceMetrics::end_area( PerformanceMetrics::SECTION_PHYSICS, "physics" );
		};
				
		
	}
	PerformanceMetrics::end_section_frame( PerformanceMetrics::SECTION_PHYSICS );
	
	PerformanceMetrics::begin_area( PerformanceMetrics::SECTION_PROCESS, "post_process" );

	post_frame_process_signal.call(slice_time);
	
	PerformanceMetrics::end_area( PerformanceMetrics::SECTION_PROCESS, "post_process" );
	
	interval_remainder-=slice_time;
	
	bool res_user = iteration_user(slice_time);
	
	PerformanceMetrics::end_section_frame( PerformanceMetrics::SECTION_PROCESS );
	
	return  res_user;
}

void SceneMainLoop::set_current_camera(IRef<CameraNode> p_current_camera) {
	
	current_camera=p_current_camera;
}
IRef<CameraNode> SceneMainLoop::get_current_camera() const {
	
	return current_camera;
}

void SceneMainLoop::set_process_flags(Uint32 p_flags) {
	
	process_flags=p_flags;
}

Uint32 SceneMainLoop::get_process_flags() const {
	
	return process_flags;
}

void SceneMainLoop::draw() {

	PerformanceMetrics::begin_area( PerformanceMetrics::SECTION_PROCESS, "render" );
	
	Renderer::get_singleton()->render_clear( Renderer::CLEAR_COLOR | Renderer::CLEAR_DEPTH, Color(80,80,100) );
	if (!current_camera.is_null())
		current_camera->render();
	draw_user();

	PerformanceMetrics::end_area( PerformanceMetrics::SECTION_PROCESS, "render" );
}
void SceneMainLoop::finish() {
	
	finish_user();
}

Physics *SceneMainLoop::get_physics() {

	return physics;
}
 
void SceneMainLoop::set_scene_tree(IRef<SceneNode> p_scene) {
	
	if (p_scene==scene_tree)
		return;
	if (!scene_tree.is_null())
		scene_tree->_set_inside_scene(false);
	scene_tree=p_scene;	
	if (scene_tree)
		scene_tree->_set_inside_scene(true);
	
}
IRef<SceneNode> SceneMainLoop::get_scene_tree() {
	
	return scene_tree;
}


SceneMainLoop::SceneMainLoop() {
	
	input_event_count=0;
	process_flags=0;
	ERR_FAIL_COND( singleton && singleton!=this );
	singleton=this;
	physics = memnew(Physics);
	iteration_hz=30;
	interval_remainder=0;
	
}
     
SceneMainLoop::~SceneMainLoop() {
	
	memdelete(physics);
	scene_tree.clear();
}


