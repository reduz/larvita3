//
// C++ Interface: audio_server
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef AUDIO_SERVER_H
#define AUDIO_SERVER_H

#include "audio/audio_sample_manager.h"
#include "audio/audio_mixer.h"
#include "signals/signals.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

enum {
	
	AUDIO_FX_INVALID_ID=-1
};


typedef int AudioSFXID;

class AudioSequencer;

class AudioServer {

	float _music_volume;
	float _sfx_volume;
	static AudioServer *singleton;
public:

	/**
	 * -RETRIEVE MIXER-
	 * BIG WARNING: AudioMixer should ONLY be used within audio thread.
	 */
	virtual AudioMixer *get_mixer()=0; 
	
	static AudioServer *get_singleton();

	/* STREAM API */
	
	/**
	  * Request a stream. The method will be called from the audio thread.
	  * method format is: void ::stream_callback(Sint32 *p_frames,int p_frame_count);
	  * Mixing a stream should be done as 24 bits (NOT 32). Since 1<<24 is considered 0dB, anything above
	  * that value will be clipped. Audio frames must be ADDED, not ASSIGNED.
	  * The stream can only be cancelled by using the same method used to connect it, or destroying the
	  * object connected to.
	  */
	
	virtual void request_output_stream( Method2<Sint32*,int> p_method )=0;
	virtual void cancel_output_stream(Method2<Sint32*,int> p_method)=0;
	virtual int get_stream_mix_rate() const=0; // get mix rate at which streams will be clipped

	/* SIMPLE FX PLAYBACK API */
	
	struct FxParams {
	
		float volume; ///< 0 - 1
		float pan; ///< -1 - +1
		float depth; ///< -1 - +1
		float rate_shift; ///< >0 +
		float cutoff; ///< <0 disabled, 
		float resonance; ///< 0 - 1	
		float chorus;
		float reverb;	
		
		FxParams() { volume=1.0; pan=0.5; depth=0.5; rate_shift=1; cutoff=-1; resonance=0; chorus=0; reverb=0; }
	};
	
	virtual AudioSFXID play_fx(AudioSampleID p_sample,const FxParams* p_fx_params=NULL)=0;
	virtual void change_fx(AudioSFXID p_sample,const FxParams *p_fx_params)=0;
	virtual Error get_fx_params(AudioSFXID p_sample,FxParams *p_fx_params)=0;
	virtual bool is_fx_active(AudioSFXID p_sample) const=0;	
	virtual void stop_fx(AudioSFXID p_sample)=0;

		
	/* General Volume */
	
	void set_music_volume(float p_volume); ///< set music volume 0 - 1
	float get_music_volume() const; ///< set music volume 0 - 1
	void set_sfx_volume(float p_volume); ///< set sfx volume 0 - 1
	float get_sfx_volume() const; ///< set sfx volume 0 - 1
	
	/* LOCK API */

	virtual void lock()=0; ///< Lock the audio thread
	virtual void unlock()=0; ///< Unlock the audio thread

	AudioServer();
	virtual ~AudioServer();

};

#endif
