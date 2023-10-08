//
// C++ Interface: audio_mixer
//
// Description:
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef AUDIO_MIXER_H
#define AUDIO_MIXER_H

#include "typedefs.h"
#include "signals/signals.h"
#include "audio/audio_sample_manager.h"

typedef int AudioVoiceID;


enum {

	AUDIO_VOICE_INVALID_ID=-1, ///< Generic value for invalid voices. anything < 0 works.
};

/**
	@author ,,, <red@lunatea>
*/

enum AudioFilterType {

	AUDIO_FILTER_NONE,
	AUDIO_FILTER_LOWPASS,
	AUDIO_FILTER_BANDPASS,
	AUDIO_FILTER_HIPASS,
	AUDIO_FILTER_NOTCH,
};


class AudioMixer {
public:


	/* VOICE API */

	virtual AudioVoiceID voice_alloc(AudioSampleID p_sample,int p_offset=0)=0; ///< Start a voice, allocating one, VOICE_INVALID_ID if not found
	virtual void voice_stop(AudioVoiceID p_voice)=0;
	virtual void voice_set_volume(AudioVoiceID p_voice,Uint8 p_volume)=0; ///< Set volume, 0 .. 255
	virtual void voice_set_pan(AudioVoiceID p_voice,Uint8 p_pan,Uint8 p_depth=0x7F)=0; ///< Set pan and alternatively depth, 0..1
	virtual void voice_set_speed(AudioVoiceID p_voice,Uint32 p_speed)=0; ///< Change the pitch of a voice, in HZ
	virtual void voice_set_pitch(AudioVoiceID p_voice,Uint32 p_hz)=0; ///< Change the pitch of a voice, in HZ, but takes in consideration the c5 freq of the sample
	virtual void voice_set_filter(AudioVoiceID p_voice,AudioFilterType p_type=AUDIO_FILTER_NONE,Uint32 p_cutoff_hz=200,Uint8 p_resonance=0)=0; ///< Setup filter
	virtual void voice_set_chorus(AudioVoiceID p_voice,Uint8 p_send)=0; ///< Chorus send 0 .. 1
	virtual void voice_set_reverb(AudioVoiceID p_voice,Uint8 p_send)=0; ///< Reverb send 0 .. 1


	virtual Uint8 voice_get_volume(AudioVoiceID p_voice) const=0; ///< Get volume, 0 .. 255
	virtual Uint8 voice_get_pan(AudioVoiceID p_voice) const=0;
	virtual Uint8 voice_get_depth(AudioVoiceID p_voice) const=0;

	virtual Uint32 voice_get_speed(AudioVoiceID p_voice) const=0;

	virtual Uint8 voice_get_chorus(AudioVoiceID p_voice) const=0;
	virtual Uint8 voice_get_reverb(AudioVoiceID p_voice) const=0;

	virtual bool is_voice_active(AudioVoiceID p_voice) const=0;

	Signal< Method1<Uint32> > mixer_tick_signal; //  (mixer tick, in usecs

	AudioMixer();
	virtual ~AudioMixer();

};

#endif
