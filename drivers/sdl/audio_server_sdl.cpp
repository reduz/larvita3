//
// C++ Implementation: audio_server_sdl
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "audio_server_sdl.h"
#ifdef SDL_ENABLED

#include "global_vars.h"

void AudioServer_SDL::process(Uint8* p_stream, int p_bytes) {
	
	int todo=p_bytes;
	todo/=internal_params.channels; //discount stereo

	if (internal_params.format==AUDIO_S8) {

		Sint8*data=(Sint8*)p_stream;
		while (todo) {
	
			int to_mix=MIN(MIXDOWN_MAX_SIZE,todo);
					
			mix(mixdown,to_mix,internal_params.freq);
			const Sint32 *mixdown_ptr=mixdown;
			
			if (internal_params.channels==2) {
			
				for (int i=0;i<to_mix;i++) {
					// 24->8 bits
					*data++ = (Sint8)(*(mixdown_ptr++)>>16);
					*data++ = (Sint8)(*(mixdown_ptr++)>>16);
				}
			} else if (internal_params.channels==1) {
				
				for (int i=0;i<to_mix;i++) {
		
					for (int i=0;i<to_mix;i++) {
					// 24->8 bits
						*data++ = (Sint8)(*(mixdown_ptr++)>>17)+(Sint8)(*(mixdown_ptr++)>>17);
					}
				}				
			}
	
			todo-=to_mix;
		}
		
	} else { // 16
		
		todo/=2;		
		
		Sint16*data=(Sint16*)p_stream;
		while (todo) {
	
			int to_mix=MIN(MIXDOWN_MAX_SIZE,todo);
					
			mix(mixdown,to_mix,internal_params.freq);
			const Sint32 *mixdown_ptr=mixdown;
			
			if (internal_params.channels==2) {
			
				for (int i=0;i<to_mix;i++) {
					// 24->8 bits
					*data++ = (Sint16)(*(mixdown_ptr++)>>8);
					*data++ = (Sint16)(*(mixdown_ptr++)>>8);
				}
			} else if (internal_params.channels==1) {
				
				for (int i=0;i<to_mix;i++) {
		
					for (int i=0;i<to_mix;i++) {
					// 24->8 bits
						*data++ = (Sint16)(*(mixdown_ptr++)>>7)+(Sint16)(*(mixdown_ptr++)>>7);
					}
				}				
			}
	
			todo-=to_mix;
		}
		
	}
}

void AudioServer_SDL::process_cbk(void* arg, Uint8* p_stream, int p_bytes) {
	
	AudioServer_SDL *_self = (AudioServer_SDL*)arg;
	
	_self->process(p_stream,p_bytes);
}


void AudioServer_SDL::lock() {
	
	SDL_LockAudio();
}
void AudioServer_SDL::unlock() {
	
	SDL_UnlockAudio();
	
}


AudioServer_SDL::AudioServer_SDL(int p_sample_ram_size) : AudioServerSoftware(p_sample_ram_size) {
	
	int err = SDL_InitSubSystem(SDL_INIT_AUDIO);
	if (err!=0) {
	
		ERR_PRINT("SDL Error initializing audio");
	}
	
	SDL_AudioSpec *desired;

	/* Allocate a desired SDL_AudioSpec */
	desired = new SDL_AudioSpec;

	/* Allocate space for the internal_params SDL_AudioSpec */

	desired->freq=GlobalVars::get_singleton()->get("audio/sw_mixing_rate");
	desired->format=(GlobalVars::get_singleton()->get("audio/sw_bits").get_int()==8)?AUDIO_S8:AUDIO_S16SYS;
	desired->channels=GlobalVars::get_singleton()->get("audio/sw_stereo").get_bool()?2:1;
	desired->samples=GlobalVars::get_singleton()->get("audio/sw_buffer_size");
	desired->callback=process_cbk;
	desired->userdata=this;

	/* Open the audio device */
	if ( SDL_OpenAudio(desired, &internal_params) < 0 ) {
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		active=false;
		delete desired;
		return;
	}
	delete desired;

	printf("Audio Open SUCCESS:\n\tMix Rate: %i\n\tBit Depth: %s\n\tStereo: %s\n\tBuffer Size: %i\n",internal_params.freq,(internal_params.format==AUDIO_S8?"8":"16"),(internal_params.channels==2?"yes":"no"),internal_params.samples);
	active=true;
	SDL_PauseAudio(0);		
}


AudioServer_SDL::~AudioServer_SDL()
{
	
	if (active)
		SDL_CloseAudio();
}


#endif
