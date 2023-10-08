//
// C++ Implementation: audio_mixer_software
//
// Description:
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "audio_mixer_software.h"


inline int AudioMixerSoftware::get_voice_idx(int p_voice) const {

	if (p_voice<0) {
		return -1;
	}

	int idx=p_voice%MAX_VOICES;
	int check=p_voice/MAX_VOICES;
	ERR_FAIL_INDEX_V(idx,MAX_VOICES,-1);
	if (voices[idx].check!=check) {
	//rintf("check mismatch %i!=%i\n",voices[idx].check,check);
		return -1;
	}
	if (!voices[idx].active) {
	//rintf("voice was not active\n");
		return -1;
	}
	return idx;
}

AudioVoiceID AudioMixerSoftware::voice_alloc(AudioSampleID p_sample,int p_offset) {

	AudioSampleManagerSoftware *asms = AudioSampleManagerSoftware::get_singleton();
	ERR_FAIL_COND_V(!asms,AUDIO_VOICE_INVALID_ID);


	ERR_FAIL_COND_V( !asms->check( p_sample ), AUDIO_VOICE_INVALID_ID );

	int index=-1;
	for (int i=0;i<MAX_VOICES;i++) {

		if (!voices[i].active) {
			index=i;
			break;
		}
	}

	if (index==-1)
		return AUDIO_VOICE_INVALID_ID;

	Voice &v=voices[index];

	// init variables
	v.sample=p_sample;
	v.vol=255;
	v.pan=128;
	v.depth=128;
	v.chorus_send=0;
	v.reverb_send=0;
	v.speed=1;
	v.active=true;
	v.check=voice_id_count++;
	v.first_mix=true;

	// init mix variables

	v.mix.increment=1;
	v.mix.r_vol=0;
	v.mix.l_vol=0;
	v.mix.old_r_vol=0;
	v.mix.old_l_vol=0;
	v.mix.filter.h1[0]=0;
	v.mix.filter.h1[1]=0;
	v.mix.filter.h2[0]=0;
	v.mix.filter.h2[1]=0;
	v.mix.offset=0;
	v.filter.enabled=false;

	AudioVoiceID ret_id = index+v.check*MAX_VOICES;
	return ret_id;
}
void AudioMixerSoftware::voice_stop(AudioVoiceID p_voice) {

	int idx=get_voice_idx(p_voice);
	if (idx<0 || idx >=MAX_VOICES)
		return;

	Voice &v=voices[idx];
	if (v.active && (v.mix.l_vol || v.mix.r_vol) && inside_mix) {
		// drive voice to zero, and run a chunk, the VRAMP will fade it good
		v.vol=0;
		mix_voice(v);
	}
	/* @TODO RAMP DOWN ON STOP */
	v.active=false;

}
void AudioMixerSoftware::voice_set_volume(AudioVoiceID p_voice,Uint8 p_volume) {

	int idx=get_voice_idx(p_voice);
	ERR_FAIL_INDEX(idx,MAX_VOICES);
	Voice &v=voices[idx];
	v.vol=p_volume;

}
void AudioMixerSoftware::voice_set_pan(AudioVoiceID p_voice,Uint8 p_pan,Uint8 p_depth) {


	int idx=get_voice_idx(p_voice);
	ERR_FAIL_INDEX(idx,MAX_VOICES);
	Voice &v=voices[idx];
	v.pan=p_pan;
	v.depth=p_depth;

}

void AudioMixerSoftware::voice_set_speed(AudioVoiceID p_voice,Uint32 p_speed) {

	int idx=get_voice_idx(p_voice);
	ERR_FAIL_INDEX(idx,MAX_VOICES);
	Voice &v=voices[idx];
	v.speed=p_speed;

}

void AudioMixerSoftware::voice_set_pitch(AudioVoiceID p_voice,Uint32 p_hz) {

	int idx=get_voice_idx(p_voice);
	ERR_FAIL_INDEX(idx,MAX_VOICES);
	Voice &v=voices[idx];
	//v.speed=p_hz;

}



void AudioMixerSoftware::voice_set_filter(AudioVoiceID p_voice,AudioFilterType p_type,Uint32 p_cutoff_hz,Uint8 p_resonance) {

	int idx=get_voice_idx(p_voice);
	ERR_FAIL_INDEX(idx,MAX_VOICES);
	Voice &v=voices[idx];

	bool do_enable=p_type!=AUDIO_FILTER_NONE;
	if (!do_enable) {

		v.filter.enabled=false; // turn off, goodbye
		return;
	}

	if (v.filter.enabled && p_cutoff_hz==v.filter.cutoff && p_resonance==v.filter.resonance)
		return; // noc hanges, goodbye



	static const float losstable[128]={
		1.0,0.97864,0.95774,0.93729,0.91727,0.89768,0.87851,0.85975,
		0.84139,0.82342,0.80584,0.78863,0.77179,0.75530,0.73917,0.72339,
		0.70794,0.69282,0.67803,0.66355,0.64938,0.63551,0.62194,0.60866,
		0.59566,0.58294,0.57049,0.55830,0.54638,0.53471,0.52329,0.51212,
		0.50118,0.49048,0.48000,0.46975,0.45972,0.44990,0.44030,0.43089,
		0.42169,0.41269,0.40387,0.39525,0.38681,0.37855,0.37046,0.36255,
		0.35481,0.34723,0.33982,0.33256,0.32546,0.31851,0.31170,0.30505,
		0.29853,0.29216,0.28592,0.27981,0.27384,0.26799,0.26227,0.25666,
		0.25118,0.24582,0.24057,0.23543,0.23040,0.22548,0.22067,0.21596,
		0.21134,0.20683,0.20241,0.19809,0.19386,0.18972,0.18567,0.18170,
		0.17782,0.17403,0.17031,0.16667,0.16311,0.15963,0.15622,0.15288,
		0.14962,0.14642,0.14330,0.14024,0.13724,0.13431,0.13144,0.12863,
		0.12589,0.12320,0.12057,0.11799,0.11547,0.11301,0.11059,0.10823,
		0.10592,0.10366,0.10144,0.09928,0.09716,0.09508,0.09305,0.09106,
		0.08912,0.08722,0.08535,0.08353,0.08175,0.08000,0.07829,0.07662,
		0.07498,0.07338,0.07182,0.07028,0.06878,0.06731,0.06587,0.06447
	};

	v.filter.cutoff=p_cutoff_hz;
	v.filter.resonance=p_resonance;

	float inv_angle = (float)((float)v.filter.cutoff * ((2*3.14159265358979323846)))/mix_rate;
	inv_angle=1.0/inv_angle;

	float loss = losstable[v.filter.resonance>>1];
	float d, e;

	d = (1.0f - loss) / inv_angle;
	if (d > 2.0f) d = 2.0f;
	d = (loss - d) * inv_angle;
	e = inv_angle * inv_angle;
	float a = 1.0f / (1.0f + d + e);
	float c = -e * a;
	float b = 1.0f - a - c;

	v.filter.c1=(Sint32)(a*MIX_FILTER_FRAC_LEN);
	v.filter.c2=(Sint32)(b*MIX_FILTER_FRAC_LEN);
	v.filter.c3=(Sint32)(c*MIX_FILTER_FRAC_LEN);

	if (!v.filter.enabled) {
		// if just being enabled, clear history
		v.mix.filter.h1[0]=0;
		v.mix.filter.h1[1]=0;
		v.mix.filter.h2[0]=0;
		v.mix.filter.h2[1]=0;
		v.mix.filter.old_c1=v.filter.c1;
		v.mix.filter.old_c2=v.filter.c2;
		v.mix.filter.old_c3=v.filter.c3;
		v.filter.enabled;
	}


}
void AudioMixerSoftware::voice_set_chorus(AudioVoiceID p_voice,Uint8 p_send) {


	int idx=get_voice_idx(p_voice);
	ERR_FAIL_INDEX(idx,MAX_VOICES);
	Voice &v=voices[idx];
	v.chorus_send=p_send;

}
void AudioMixerSoftware::voice_set_reverb(AudioVoiceID p_voice,Uint8 p_send) {

	int idx=get_voice_idx(p_voice);
	ERR_FAIL_INDEX(idx,MAX_VOICES);
	Voice &v=voices[idx];
	v.reverb_send=p_send;
}


Uint8 AudioMixerSoftware::voice_get_volume(AudioVoiceID p_voice) const {

	int idx=get_voice_idx(p_voice);
	ERR_FAIL_INDEX_V(idx,MAX_VOICES,0);
	const Voice &v=voices[idx];
	return v.vol;

}
Uint8 AudioMixerSoftware::voice_get_pan(AudioVoiceID p_voice) const {

	int idx=get_voice_idx(p_voice);
	ERR_FAIL_INDEX_V(idx,MAX_VOICES,0);
	const Voice &v=voices[idx];
	return v.pan;

}
Uint8 AudioMixerSoftware::voice_get_depth(AudioVoiceID p_voice) const {

	int idx=get_voice_idx(p_voice);
	ERR_FAIL_INDEX_V(idx,MAX_VOICES,0);
	const Voice &v=voices[idx];
	return v.depth;

}

Uint32 AudioMixerSoftware::voice_get_speed(AudioVoiceID p_voice) const {

	int idx=get_voice_idx(p_voice);
	ERR_FAIL_INDEX_V(idx,MAX_VOICES,0);
	const Voice &v=voices[idx];
	return v.speed;

}

Uint8 AudioMixerSoftware::voice_get_chorus(AudioVoiceID p_voice) const {

	int idx=get_voice_idx(p_voice);
	ERR_FAIL_INDEX_V(idx,MAX_VOICES,0);
	const Voice &v=voices[idx];
	return v.chorus_send;
}
Uint8 AudioMixerSoftware::voice_get_reverb(AudioVoiceID p_voice) const {

	int idx=get_voice_idx(p_voice);
	ERR_FAIL_INDEX_V(idx,MAX_VOICES,0);
	const Voice &v=voices[idx];
	return v.reverb_send;

}

bool AudioMixerSoftware::is_voice_active(AudioVoiceID p_voice) const {

	int idx=get_voice_idx(p_voice);
	if (idx<0 || idx>=MAX_VOICES)
		return false;
	const Voice &v=voices[idx];
	return v.active;

}

/** RESAMPLER ROUTINE **/

// templates are used so unnecesary code is removed at compilation time. All conditionals dissapear by then. //

template<class Depth,bool is_stereo,bool use_filter,bool use_vramp,bool use_fx,InterpolationType type>
inline void AudioMixerSoftware::do_resample(


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

		       ) {
	Sint32 final,final_r,next,next_r;
	while (p_amount--) {

		Sint32 pos=p_pos >> MIX_FRAC_BITS;
		if (is_stereo)
			pos<<=1;

		final=p_src[pos];
		if (is_stereo)
			final_r=p_src[pos+1];

		if (sizeof(Depth)==1) { /* conditions will not exist anymore when compiled! */
			final<<=8;
			if (is_stereo)
				final_r<<=8;
		}

		if (type==INTERPOLATION_LINEAR) {

			if (is_stereo) {

				next=p_src[pos+2];
				next_r=p_src[pos+3];
			} else {
				next=p_src[pos+1];
			}
			if (sizeof(Depth)==1) {
				next<<=8;
				if (is_stereo)
					next_r<<=8;
			}

			Sint32 frac=Sint32(p_pos&MIX_FRAC_MASK);

			final=final+((next-final)*frac >> MIX_FRAC_BITS);
			if (is_stereo)
				final_r=final_r+((next_r-final_r)*frac >> MIX_FRAC_BITS);
		}

		if (use_filter) {

			// well, convert to less resolution for filters.. can't do much, sorry!
			//final>>=4;
			final = (final * (p_lp_c1>>10) + *p_lp_h1 * (p_lp_c2>>10) + *p_lp_h2 * (p_lp_c3>>10)) >> MIX_FILTER_FRAC_BITS;

			*p_lp_h2=*p_lp_h1;
			*p_lp_h1=final;  	//final<<=4;


			if (is_stereo) {

				final_r = (final_r * p_lp_c1 + p_lp_h1[1] * p_lp_c2 + p_lp_h2[1] * p_lp_c3) >> MIX_FILTER_FRAC_BITS;
				p_lp_h2[1]=p_lp_h1[1];                                  p_lp_h1[1]=final_r;

			}


			p_lp_c1+=p_lp_c1_inc;
			p_lp_c2+=p_lp_c2_inc;
			p_lp_c3+=p_lp_c3_inc;
		}

		if (use_vramp) {


			if (is_stereo) {

				final=final*(*p_lvol>>MIX_VOLRAMP_FRAC_BITS);
				final_r=final_r*(*p_rvol>>MIX_VOLRAMP_FRAC_BITS);

			} else {
				final_r=final*(*p_rvol>>MIX_VOLRAMP_FRAC_BITS);
				final=final*(*p_lvol>>MIX_VOLRAMP_FRAC_BITS);
			}
		} else {

			if (is_stereo) {

				final=final*(*p_lvol);
				final_r=final_r*(*p_rvol);

			} else {
				final_r=final*(*p_rvol);
				final=final*(*p_lvol);
			}

		}

		// This is a bit difficult to explain, the idea is that:
		//	1) 0dB is at (1<<24)
		//	2) have room for fp multiply send buffers

		final>>=8+MIX_VOL_DAMP_BITS;
		final_r>>=8+MIX_VOL_DAMP_BITS;

		if (use_fx) {

			*(p_chorus_buff++)+=(final*p_chorus_level)>>8;
			*(p_chorus_buff++)+=(final_r*p_chorus_level)>>8;
			*(p_reverb_buff++)+=(final*p_reverb_level)>>8;
			*(p_reverb_buff++)+=(final_r*p_reverb_level)>>8;
		}


		*p_dst++ +=final;
		*p_dst++ +=final_r;

		p_pos+=p_increment;


		if (use_vramp) {
			(*p_lvol)+=p_lvol_inc;
			(*p_rvol)+=p_rvol_inc;
		}
	}
}

void AudioMixerSoftware::mix_voice(Voice& v) {

	AudioSampleManagerSoftware *asms = AudioSampleManagerSoftware::get_singleton();
	ERR_FAIL_COND(!asms);

	/* some pointers.. */

	if (!asms->check(v.sample)) {
		// sample is gone!
		v.active=false;
		return;
	}


	/* some 64-bit fixed point precaches */

	Sint64 loop_begin_fp=((Sint64)asms->get_loop_begin(v.sample) << MIX_FRAC_BITS);
	Sint64 loop_end_fp=((Sint64)asms->get_loop_end(v.sample) << MIX_FRAC_BITS);
	Sint64 length_fp=((Sint64)asms->get_size(v.sample) << MIX_FRAC_BITS);
	Sint64 begin_limit=(asms->get_loop_type(v.sample)!=AUDIO_SAMPLE_LOOP_NONE)?loop_begin_fp:0;
	Sint64 end_limit=(asms->get_loop_type(v.sample)!=AUDIO_SAMPLE_LOOP_NONE)?loop_end_fp:length_fp;
	bool is_stereo=asms->is_stereo(v.sample);

	Sint32 todo=MIX_CHUNK_SIZE;
	int mixed=0;

	/* compute voume ramps, increment, etc */


	v.mix.old_l_vol=v.mix.l_vol;
	v.mix.old_r_vol=v.mix.r_vol;
	v.mix.l_vol=((Sint32)v.vol*(255-v.pan));
	v.mix.r_vol=((Sint32)v.vol*v.pan);
	if (v.first_mix) { // avoid ramp up

		v.mix.old_l_vol=v.mix.l_vol;
		v.mix.old_r_vol=v.mix.r_vol;
		v.first_mix=false;
	}

	if (v.mix.increment>0)
		v.mix.increment=((Sint64)v.speed<<MIX_FRAC_BITS)/mix_rate;
	else
		v.mix.increment=-((Sint64)v.speed<<MIX_FRAC_BITS)/mix_rate;

	Sint32 vol_l=v.mix.old_l_vol;
	Sint32 vol_r=v.mix.old_r_vol;
	Sint32 vol_l_inc=((v.mix.l_vol-v.mix.old_l_vol)<<MIX_VOLRAMP_FRAC_BITS)>>MIX_CHUNK_BITS;
	Sint32 vol_r_inc=((v.mix.r_vol-v.mix.old_r_vol)<<MIX_VOLRAMP_FRAC_BITS)>>MIX_CHUNK_BITS;

	bool use_vramp=(vol_l_inc || vol_r_inc);
	if (use_vramp) {

		vol_l<<=MIX_VOLRAMP_FRAC_BITS;
		vol_r<<=MIX_VOLRAMP_FRAC_BITS;
	}

	AudioSampleLoopType loop_type=asms->get_loop_type(v.sample);
	AudioSampleFormat format=asms->get_format(v.sample);

	bool use_filter=false;
	bool use_fx=(fx_enabled && v.chorus_send && v.reverb_send);

	/* audio data */

	const void *data=asms->get_data_ptr(v.sample);
	Sint32 *dst_buff=mix_buffer;

	/* @TODO validar loops al registrar? */

	while (todo>0) {

		Sint64 limit=0;
		Sint32 target=0,aux=0;

		/** LOOP CHECKING **/

		if ( v.mix.increment < 0 ) {
			/* going backwards */

			if(  loop_type!=AUDIO_SAMPLE_LOOP_NONE && v.mix.offset < loop_begin_fp ) {
				/* loopstart reached */

				if ( loop_type==AUDIO_SAMPLE_LOOP_PING_PONG ) {
					/* bounce ping pong */
					v.mix.offset= loop_begin_fp + ( loop_begin_fp-v.mix.offset );
					v.mix.increment=-v.mix.increment;
				} else {
					/* go to loop-end */
					v.mix.offset=loop_end_fp-(loop_begin_fp-v.mix.offset);
				}
			} else {
				/* check for sample not reaching begining */
				if(v.mix.offset < 0) {

					v.active=false;
					break;
				}
			}
		} else {
			/* going forward */
			if(  loop_type!=AUDIO_SAMPLE_LOOP_NONE && v.mix.offset >= loop_end_fp ) {
				/* loopend reached */

				if ( loop_type==AUDIO_SAMPLE_LOOP_PING_PONG ) {
					/* bounce ping pong */
					v.mix.offset=loop_end_fp-(v.mix.offset-loop_end_fp);
					v.mix.increment=-v.mix.increment;
				} else {
					/* go to loop-begin */

					v.mix.offset=loop_begin_fp+(v.mix.offset-loop_end_fp);

				}
			} else {
				/* no loop, check for end of sample */
				if(v.mix.offset >= length_fp) {

					v.active=false;

					break;
				}
			}
		}

		/** MIXCOUNT COMPUTING **/

		/* next possible limit (looppoints or sample begin/end */
		limit=(v.mix.increment < 0) ?begin_limit:end_limit;

		/* compute what is shorter, the todo or the limit? */
		aux=(limit-v.mix.offset)/v.mix.increment+1;
		target=(aux<todo)?aux:todo; /* mix target is the shorter buffer */

		/* check just in case */
		if ( target<=0 ) {

			v.active=false;
			break;
		}

		todo-=target;

		Sint32 offset=v.mix.offset&MIX_CHUNK_MASK; /* strip integer */

/* Macros to call the resample function for all possibilities */

#define CALL_RESAMPLE_FUNC( m_depth, m_stereo, m_use_filter, m_use_vramp, m_use_fx, m_interp)\
	do_resample<m_depth,m_stereo,m_use_filter,m_use_vramp,m_use_fx,m_interp>(\
		src_ptr,\
		dst_buff,target,\
		v.mix.increment,\
		offset,\
		&vol_l,\
		&vol_r,\
		vol_l_inc,\
		vol_r_inc,\
		v.mix.filter.old_c1,\
		v.mix.filter.old_c2,\
		v.mix.filter.old_c3,\
		0,\
		0,\
		0,\
		v.mix.filter.h1,\
		v.mix.filter.h2,\
		&chorus_buffer[(MIX_CHUNK_SIZE+mixed)*2],\
		v.chorus_send,\
		&reverb_buffer[mixed*2],\
		v.reverb_send);


#define CALL_RESAMPLE_INTERP( m_depth, m_stereo, m_use_filter, m_use_vramp, m_use_fx, m_interp)\
	if(m_interp==INTERPOLATION_RAW) {\
		CALL_RESAMPLE_FUNC(m_depth,m_stereo,m_use_filter,m_use_vramp,m_use_fx,INTERPOLATION_RAW);\
	} else if(m_interp==INTERPOLATION_LINEAR) {\
		CALL_RESAMPLE_FUNC(m_depth,m_stereo,m_use_filter,m_use_vramp,m_use_fx,INTERPOLATION_LINEAR);\
	}\

#define CALL_RESAMPLE_FX( m_depth, m_stereo, m_use_filter, m_use_vramp, m_use_fx, m_interp)\
	if(m_use_fx) {\
		CALL_RESAMPLE_INTERP(m_depth,m_stereo,m_use_filter,m_use_vramp,true,m_interp);\
	} else {\
		CALL_RESAMPLE_INTERP(m_depth,m_stereo,m_use_filter,m_use_vramp,false,m_interp);\
	}\

#define CALL_RESAMPLE_VRAMP( m_depth, m_stereo, m_use_filter, m_use_vramp, m_use_fx, m_interp)\
	if(m_use_vramp) {\
		CALL_RESAMPLE_FX(m_depth,m_stereo,m_use_filter,true,m_use_fx,m_interp);\
	} else {\
		CALL_RESAMPLE_FX(m_depth,m_stereo,m_use_filter,false,m_use_fx,m_interp);\
	}\

#define CALL_RESAMPLE_FILTER( m_depth, m_stereo, m_use_filter, m_use_vramp, m_use_fx, m_interp)\
	if(m_use_filter) {\
		CALL_RESAMPLE_VRAMP(m_depth,m_stereo,true,m_use_vramp,m_use_fx,m_interp);\
	} else {\
		CALL_RESAMPLE_VRAMP(m_depth,m_stereo,false,m_use_vramp,m_use_fx,m_interp);\
	}\

#define CALL_RESAMPLE( m_depth, m_stereo, m_use_filter, m_use_vramp, m_use_fx, m_interp)\
	if(m_stereo) {\
		CALL_RESAMPLE_FILTER(m_depth,true,m_use_filter,m_use_vramp,m_use_fx,m_interp);\
	} else {\
		CALL_RESAMPLE_FILTER(m_depth,false,m_use_filter,m_use_vramp,m_use_fx,m_interp);\
	}\



		if (format==AUDIO_SAMPLE_FORMAT_PCM8) {

			Sint8 *src_ptr =  &((Sint8*)data)[(v.mix.offset >> MIX_FRAC_BITS)<<(is_stereo?1:0) ];
			CALL_RESAMPLE(Sint8,is_stereo,use_filter,use_vramp,use_fx,interpolation_type);

		} else if (format==AUDIO_SAMPLE_FORMAT_PCM16) {
			Sint16 *src_ptr =  &((Sint16*)data)[(v.mix.offset >> MIX_FRAC_BITS)<<(is_stereo?1:0) ];
			CALL_RESAMPLE(Sint16,is_stereo,use_filter,use_vramp,use_fx,interpolation_type);

		}


		v.mix.offset+=offset;
		dst_buff+=target*2;

	}
}


void AudioMixerSoftware::mix_chunk() {

	ERR_FAIL_COND(mix_chunk_left);

	AudioSampleManagerSoftware *asms = AudioSampleManagerSoftware::get_singleton();
	ERR_FAIL_COND(!asms);

	inside_mix=true;

	// emit tick in usecs
	for (int i=0;i<MIX_CHUNK_SIZE*2;i++) {

		mix_buffer[i]=0;
		if (fx_enabled) {
			reverb_buffer[i]=0;
			chorus_buffer[i]=chorus_buffer[MIX_CHUNK_SIZE*2+i];
			chorus_buffer[MIX_CHUNK_SIZE*2+i]=0;
		}
	}

	mixer_tick_signal.call( (Uint32)((Uint64)MIX_CHUNK_SIZE*1000000/(Uint64)mix_rate) );



	for (int i=0;i<MAX_VOICES;i++) {

		if (!voices[i].active)
			continue;

		/* process volume */
		Voice&v=voices[i];
		mix_voice(v);

	}

	mix_chunk_left=MIX_CHUNK_SIZE;
	inside_mix=false;

}

void AudioMixerSoftware::set_mix_rate(Uint32 p_mix_rate) {

	mix_rate=p_mix_rate;
}

Uint32 AudioMixerSoftware::mix(Sint32 *p_buffer,int p_frames) {

	int todo=p_frames;
	while(todo) {


		if (!mix_chunk_left) {

			mix_chunk();
		}

		int to_mix=MIN(mix_chunk_left,todo);
		int from=MIX_CHUNK_SIZE-mix_chunk_left;

		for (int i=0;i<to_mix*2;i++) {

			(*p_buffer++)=mix_buffer[from*2+i];
		}

		mix_chunk_left-=to_mix;
		todo-=to_mix;
	}

	return p_frames; //mixed all, always, no exception!
}


AudioMixerSoftware::AudioMixerSoftware() {

	mix_rate=44100;
	mix_chunk_left=0;
	fx_enabled=false;
	interpolation_type=INTERPOLATION_LINEAR;
	voice_id_count=1;
	inside_mix=false;

}


AudioMixerSoftware::~AudioMixerSoftware()
{
}


