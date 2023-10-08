//
// C++ Interface: audio_mixer_software
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef AUDIO_MIXER_SOFTWARE_H
#define AUDIO_MIXER_SOFTWARE_H

#include "audio/audio_mixer.h"
#include "audio/audio_sample_manager_software.h"
/**
	@author ,,, <red@lunatea>
*/

enum InterpolationType {

	INTERPOLATION_RAW,
	INTERPOLATION_LINEAR
};

class AudioMixerSoftware : public AudioMixer {
public:
	enum {
		
		MAX_VOICES=64,
		// fixed point defs
		MIX_CHUNK_BITS=8,
		MIX_CHUNK_SIZE=(1<<MIX_CHUNK_BITS),
		MIX_CHUNK_MASK=MIX_CHUNK_BITS-1,
		MIX_FRAC_BITS=13,
		MIX_FRAC_LEN=(1<<MIX_FRAC_BITS),
		MIX_FRAC_MASK=MIX_FRAC_LEN-1,
		MIX_VOLRAMP_FRAC_BITS=8,
		MIX_VOLRAMP_FRAC_LEN=(1<<MIX_VOLRAMP_FRAC_BITS),
		MIX_VOLRAMP_FRAC_MASK=MIX_VOLRAMP_FRAC_LEN-1,
		MIX_VOL_DAMP_BITS=0,
		MIX_FILTER_FRAC_BITS=7,
		MIX_FILTER_FRAC_LEN=(1<<MIX_FILTER_FRAC_BITS)		
	};
	
	struct Voice {
		
		AudioSampleID sample;
		struct Mix {
			Sint64 offset;
			Sint32 increment;
			Sint32 l_vol,r_vol;
			Sint32 old_l_vol,old_r_vol;
						
			struct Filter { //history (stereo)
				Sint32 h1[2],h2[2];
				Sint32 old_c1,old_c2,old_c3; //old fixed point filter stuff
				
			} filter;
		
		} mix;
		
		Uint8 vol;
		Uint8 pan;
		Uint8 depth;
		Uint8 chorus_send;
		Uint8 reverb_send;
		Uint32 speed;
		int check;
		struct Filter {
		
			Uint32 cutoff;
			Uint8 resonance;
			
			Sint32 c1,c2,c3; //fixed point filter stuff
			bool enabled;
															
		} filter;
		bool first_mix;
		bool active;
		Voice() { active=false; check=-1; first_mix=false; }
	};
	
	Voice voices[MAX_VOICES];
	
	Uint32 mix_rate;
	bool fx_enabled;
	InterpolationType interpolation_type;
	
	Sint32 mix_buffer[MIX_CHUNK_SIZE*2];
	Sint32 reverb_buffer[MIX_CHUNK_SIZE*2];
	Sint32 chorus_buffer[MIX_CHUNK_SIZE*4];

	template<class Depth,bool is_stereo,bool use_filter,bool use_vramp,bool use_fx,InterpolationType type>
	void do_resample(
		const Depth* p_src,
		Sint32 *p_dst,int p_amount,
		const Sint32 &p_increment,
		Sint32 &p_pos,
		Sint32 *p_lvol,
		Sint32 *p_rvol,
		Sint32 p_lvol_inc,
		Sint32 p_rvol_inc,
		//filter
		Sint32 &p_lp_c1,
		Sint32 &p_lp_c2,
		Sint32 &p_lp_c3,
		Sint32 p_lp_c1_inc,
		Sint32 p_lp_c2_inc,
		Sint32 p_lp_c3_inc,
		
		Sint32 *p_lp_h1,
		Sint32 *p_lp_h2,
		Sint32 *p_chorus_buff,
		Sint32 p_chorus_level,
		Sint32 *p_reverb_buff,
		Sint32 p_reverb_level
		);

	void mix_voice(Voice& p_voice);
	int mix_chunk_left;
	void mix_chunk();
	int get_voice_idx(int p_voice) const;

	int voice_id_count;
	bool inside_mix;
public:

	/* VOICE API */
			
	virtual AudioVoiceID voice_alloc(AudioSampleID p_sample,int p_offset=0); ///< Start a voice, allocating one, VOICE_INVALID_ID if not found
	virtual void voice_stop(AudioVoiceID p_voice);
	virtual void voice_set_volume(AudioVoiceID p_voice,Uint8 p_volume); ///< Set volume, 0 .. 255
	virtual void voice_set_pan(AudioVoiceID p_voice,Uint8 p_pan,Uint8 p_depth=0x7F); ///< Set pan and alternatively depth, 0..1 
	virtual void voice_set_speed(AudioVoiceID p_voice,Uint32 p_speed); ///< Change the pitch of a voice, in HZ	
	virtual void voice_set_pitch(AudioVoiceID p_voice,Uint32 p_hz); ///< Change the pitch of a voice, in HZ
	virtual void voice_set_filter(AudioVoiceID p_voice,AudioFilterType p_type=AUDIO_FILTER_NONE,Uint32 p_cutoff_hz=200,Uint8 p_resonance=0); ///< Setup filter
	virtual void voice_set_chorus(AudioVoiceID p_voice,Uint8 p_send); ///< Chorus send 0 .. 1
	virtual void voice_set_reverb(AudioVoiceID p_voice,Uint8 p_send); ///< Reverb send 0 .. 1
	
	
	virtual Uint8 voice_get_volume(AudioVoiceID p_voice) const;; ///< Get volume, 0 .. 255
	virtual Uint8 voice_get_pan(AudioVoiceID p_voice) const;
	virtual Uint8 voice_get_depth(AudioVoiceID p_voice) const;
	
	virtual Uint32 voice_get_speed(AudioVoiceID p_voice) const;
	
	virtual Uint8 voice_get_chorus(AudioVoiceID p_voice) const;
	virtual Uint8 voice_get_reverb(AudioVoiceID p_voice) const;		
		
	virtual bool is_voice_active(AudioVoiceID p_voice) const;
	
	// specific to AudioMixerSoftware
	void set_mix_rate(Uint32 p_mix_rate);
	Uint32 mix(Sint32 *p_buffer,int p_frames);
	
	AudioMixerSoftware();
	~AudioMixerSoftware();

};

#endif
