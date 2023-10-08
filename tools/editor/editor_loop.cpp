//
// C++ Implementation: editor_loop
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "editor_loop.h"
#include "main/main.h"
#include "scene/main/scene_main_loop.h"
#include "render/spatial_indexer.h"

void EditorLoop::event(const InputEvent& p_event) {
		
	gui_event_translator->push_event( p_event );
	if (game_loop)
		game_loop->event(p_event);
}

void EditorLoop::request_quit() {
	
	quit_request=true;
}
void EditorLoop::init() {
	
	if (game_loop)
		game_loop->init();
	
	if (SceneMainLoop::get_singleton()) {
		editor_camera.create();
		editor_main->get_viewport()->setup(editor_camera);
		editor_light=Renderer::get_singleton()->create_light();
		editor_light->set_mode( Light::MODE_DIRECTIONAL );
		editor_light->set_ambient( Color(80,80,80) );
		editor_light->set_diffuse( Color(150,150,150) );
		editor_light->set_specular( Color(200,200,200) );
		Renderer::get_singleton()->get_spatial_indexer()->add_global_light( editor_light );
	}
}
bool EditorLoop::iteration(float p_time) {
	
	if (game_loop) {
		if (game_loop->iteration(p_time))
			quit_request=true;
	}
	
		
	return quit_request;
}

void EditorLoop::draw() {
		
	if (editing && game_loop && SceneMainLoop::get_singleton()) {
		
		Matrix4 proj;
		proj.set_projection(90,Main::get_singleton()->get_video_mode().get_aspect(),0.1,100);
		editor_camera->set_projection_matrix( proj );
		
		editor_camera->set_as_current();
	}

	if (editing) {
		
		Renderer::get_singleton()->render_clear( Renderer::CLEAR_COLOR|Renderer::CLEAR_DEPTH, Color(80,80,100) );
		
	}
	
	if (game_loop)
		game_loop->draw();
			
	
		
	/* Draw GUI */
			
	/* set best material for drawing UIs */
	//render->primitive_set_material( render->material_get_ui() );

	/* don't need projection matrix for this */
	
	Renderer *r=Renderer::get_singleton();
	r->render_set_matrix( Renderer::MATRIX_PROJECTION, Matrix4() );
	r->render_set_matrix( Renderer::MATRIX_CAMERA, Matrix4() );

	r->render_primitive_set_material( r->get_gui_material() );

	GUI::Size screen_size( 
		Main::get_singleton()->get_video_mode().width, 
		Main::get_singleton()->get_video_mode().height );
				
	if (screen_size!=editor_main->get_size()) {
		editor_main->set_size( screen_size );
		gui_painter->set_screen_size( screen_size );
	}
		
	Matrix4 gui_mtx; /* start from identity */
	gui_mtx.scale( 2.0f / screen_size.width, -2.0f / screen_size.height, 1.0f );
	gui_mtx.translate( -(screen_size.width / 2.0f), -(screen_size.height / 2.0f), 0.0f );

	r->render_set_matrix( Renderer::MATRIX_WORLD, gui_mtx );

	editor_main->check_for_updates();
	editor_main->redraw_all(GUI::Rect( GUI::Point(), screen_size ));

}
void EditorLoop::finish() {
	
	if (game_loop)
		game_loop->finish();
	
}

void EditorLoop::set_main_loop(MainLoop *p_loop) {
	
	game_loop=p_loop;
	if (SceneMainLoop::get_singleton())
		SceneMainLoop::get_singleton()->set_process_flags( Node::MODE_FLAG_EDITOR );
		
}


EditorLoop::EditorLoop() {
	
	game_loop=NULL;
	quit_request=false;
	editing=true;
	
	gui_painter = memnew( PainterRenderer );
	gui_timer = memnew( GUI_Timer );
	editor_skin = memnew( EditorSkin(gui_painter) );
	editor_main = memnew( EditorMain(gui_painter,gui_timer,editor_skin) );
	gui_event_translator = memnew( GUI_EventTranslator(editor_main) );
	
	
}


EditorLoop::~EditorLoop() {   

	memdelete( editor_main );
	memdelete( gui_event_translator );
	memdelete( gui_painter );
	memdelete( gui_timer );
	memdelete( editor_skin );

}


