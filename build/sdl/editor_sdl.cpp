//
// C++ Implementation: editor_sdl
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "build/sdl/setup_sdl.h"
#include "scene/main/scene_main_loop.h"
#include "tools/editor/editor_loop.h"
#include "main/main.h"

int main(int argc,char* argv[]) {
	
	init_sdl( argc,argv );
		
	Main *main = Main::create();
	
	EditorLoop *editor_loop = memnew( EditorLoop );
	
	main->set_main_loop( editor_loop );
	
	if (1) { //user will not provide loop, just use default
		SceneMainLoop *scene_loop = memnew( SceneMainLoop ) ;
		main->set_main_loop( scene_loop );
		main->run();
		memdelete( scene_loop );
	} else {
		
		//Error err = ScriptServer::load_code( p_script );
		
	}

	memdelete( editor_loop );
	
	memdelete( main );
	
	finish_sdl();

	return 0;
}

