//
// C++ Interface: audio_server_sdl
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef AUDIO_SERVER_SDL_H
#define AUDIO_SERVER_SDL_H

#ifdef SDL_ENABLED

#include "audio/audio_server_software.h"
#include <SDL/SDL.h>
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class AudioServer_SDL: public AudioServerSoftware {
	
	enum {
		
		MIXDOWN_MAX_SIZE=1024	
	};
	
	Sint32 mixdown[MIXDOWN_MAX_SIZE*2];
	SDL_AudioSpec internal_params;
	
	void process (Uint8* p_stream, int p_bytes);
	static void process_cbk(void* arg, Uint8* p_stream, int p_bytes);
		
	bool active;
	
	virtual void lock(); ///< Lock the audio thread
	virtual void unlock(); ///< Unlock the audio thread
	
public:
    
	AudioServer_SDL(int p_sample_ram_size);
	~AudioServer_SDL();

};

#endif
#endif
