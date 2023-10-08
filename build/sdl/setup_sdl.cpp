//
// C++ Implementation: setup_sdl
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "setup_sdl.h"
#include "bin/tests/test_main.h"

#include "drivers/libc/file_access_posix.h"
#include "drivers/windows/file_access_windows.h"

#include "drivers/libc/memory_pool_dynamic_malloc.h"
#include "drivers/libc/memory_pool_static_malloc.h"
#include "drivers/libc/print_error_libc.h"
#include "drivers/libc/dir_access_dirent.h"
#include "drivers/libc/time_posix.h"

#include "os/resolver.h"

#include "drivers/windows/dir_access_windows.h"
#include "drivers/windows/time_windows.h"

#include "fileio/sql_client_sqlite.h"
#include "fileio/sql_client_postgres.h"
#include "fileio/iapi_db_file.h"

#include "fileio/iapi_db_sql.h"
#include "fileio/image_loader_png.h"

#include "fileio/audio_loader_wav.h"

#include "drivers/sdl/mutex_sdl.h"
#include "drivers/sdl/semaphore_sdl.h"
#include "drivers/sdl/thread_sdl.h"
#include "drivers/sdl/main_sdl.h"
#include "drivers/sdl/audio_server_sdl.h"

#include "drivers/libc/tcp_client_posix.h"
#include "drivers/windows/tcp_client_win32.h"
#include "drivers/libc/udp_connection_posix.h"
#include "drivers/windows/udp_connection_windows.h"


#include "drivers/opengl/renderer_gl14.h"
#include "drivers/opengl/renderer_gl2.h"
#include "physics/broad_phase_basic.h"
#include "tools/editor/editor_loop.h"
#include "drivers/windows/shell_windows.h"
#include "drivers/libc/shell_posix.h"

#include "scene/main/scene_main_loop.h"

#include "gui_bindings/file_system_dir_access.h"
#include "global_vars.h"

#include "iapi_persist.h"

#ifdef COLLADA_ENABLED

#include "tools/FCollada/collada.h"

#endif

static MemoryPoolStaticMalloc * pool_static=NULL;
static MemoryPoolDynamicMalloc* pool_dynamic=NULL;
static IAPI_Persist * iapi_persist=NULL;
static SQL_Client *sql_client=NULL;
static IAPI_DB *iapi_db=NULL;
static ImageLoader_PNG *image_loader_png=NULL;
static IRef<GlobalVars> global_variables;
static AudioServer_SDL *audio_server=NULL;
static AudioFormatLoaderWAV *audio_format_loader_wav=NULL;
static OSTime* _time=NULL;
static Resolver* _resolver = NULL;
static Shell *_shell=NULL;
#ifdef COLLADA_ENABLED

	AnimationFormatLoaderCollada *animation_format_loader_collada=NULL;

#endif

void init_sdl(int argc, char *argv[]) {

	SDL_Init(0);

	pool_static = new MemoryPoolStaticMalloc;
	pool_dynamic = new MemoryPoolDynamicMalloc;
	Memory::add_dynamic_pool(pool_dynamic);
	set_print_error_libc();

	image_loader_png = memnew( ImageLoader_PNG );
	ImageLoader::add_image_format_loader(image_loader_png);

#ifdef COLLADA_ENABLED

	animation_format_loader_collada=memnew(AnimationFormatLoaderCollada);
	AnimationLoader::add_animation_format_loader(animation_format_loader_collada);
#endif

	{ // driver backend configuration

		BroadPhaseBasic::set_as_default();
		Thread_SDL::set_as_default();
		Mutex_SDL::set_as_default();
		Semaphore_SDL::set_as_default();
		Renderer_GL14::set_as_default();
		Main_SDL::set_as_default();
#ifdef POSIX_ENABLED
		FileAccessPosix::set_as_default();
		DirAccessDirent::set_as_default();
		_time = memnew( TimePosix );
		TcpClientPosix::set_as_default();
		_shell = memnew( ShellPosix );
		UDPConnectionPosix::set_as_default();
#elif WINDOWS_ENABLED
		FileAccessWindows::set_as_default();
		DirAccessWindows::set_as_default();
		TcpClientWin32::set_as_default();
		UDPConnectionWindows::set_as_default();
		_time = memnew( TimeWindows );
		_shell = memnew( ShellWindows );
#endif

		_resolver = memnew( Resolver );

#ifndef GUI_DISABLED

		FileSystem_DirAccess::set_as_default();
#endif
	}

	iapi_persist = memnew( IAPI_Persist( iapi_db ) );

	GlobalVars* gv = memnew(GlobalVars);
	global_variables = gv;

	/* load config */
	String config_path=".";

	if (argc>1) {

		config_path=argv[1];
	}

	global_variables->load_from_file(config_path+"/game.cfg");
	global_variables->load_from_file(config_path+"/game_override.cfg");


	{
		/* read game.conf successfully */
		// adjust paths
		DirAccess *da = DirAccess::create();
		String current = da->get_current_dir();
		da->change_dir(config_path+"/"+global_variables->get("path/resources").get_string());
		printf("current dir is %s\n",da->get_current_dir().ascii().get_data());

		global_variables->set("path/resources", da->get_current_dir());
		//da->change_dir( current );

		memdelete( da );
	};


	iapi_persist->load_db();

	audio_server = memnew( AudioServer_SDL( global_variables->get("audio/sample_ram").get_int() ) );
	audio_format_loader_wav=memnew( AudioFormatLoaderWAV );
	AudioLoader::add_audio_format_loader( audio_format_loader_wav );

}


void finish_sdl() {


	global_variables.clear();
	memdelete( iapi_persist );
	if (sql_client)
		memdelete( sql_client );
	if (iapi_db)
		memdelete( iapi_db );
	memdelete(audio_format_loader_wav);
	memdelete(audio_server);
	memdelete(_shell);
#ifdef COLLADA_ENABLED

	memdelete(animation_format_loader_collada);
#endif

	memdelete( image_loader_png );
	memdelete( _resolver );
	memdelete( _time );
	delete pool_dynamic;
	delete pool_static;


}

