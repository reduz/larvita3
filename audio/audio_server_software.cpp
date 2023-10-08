//
// C++ Implementation: audio_server_software
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "audio_server_software.h"
#include "global_vars.h"

AudioMixer *AudioServerSoftware::get_mixer() {

	return audio_mixer;
}


void AudioServerSoftware::mix(Sint32 *p_to_buffer,int p_len,int p_mix_rate) {

	mix_rate=p_mix_rate;
	audio_mixer->set_mix_rate(p_mix_rate);
	
	/* process ringbuffer */
	
	while ( cmd_rb.read_pos != cmd_rb.write_pos ) {
		
		FxCommand &fxc=cmd_rb.data[cmd_rb.read_pos];
		int idx=fxc.id%FX_MAX;
		int check=fxc.id/FX_MAX;
		
		if (idx<0 || idx >=FX_MAX || check!=fx[idx].check ) {
			
			
			cmd_rb.read_pos=(cmd_rb.read_pos+1)&FX_CMD_RINGBUFF_MASK;
			ERR_CONTINUE(idx<0);
			ERR_CONTINUE(idx>=FX_MAX);
			ERR_CONTINUE(check!=fx[idx].check);
		}
		switch (fxc.type) {
			
			case FxCommand::CMD_START: {
				
				if (audio_mixer->is_voice_active(fx[idx].voice))
					audio_mixer->voice_stop(fx[idx].voice);
				fx[idx].voice=audio_mixer->voice_alloc( fxc.sample_id, 0 );
				if (fx[idx].voice==AUDIO_VOICE_INVALID_ID) {
					// couldn't allocate voice
					ERR_PRINT("Likely out of voices\n");
					fx[idx].stage=STAGE_OFF;
					break;
				}
				fx[idx].stage=STAGE_PLAY;
				
			}; // let it fall to CMD_CHANGE
			case FxCommand::CMD_CHANGE: {
				
				if (fx[idx].stage!=STAGE_PLAY) {
					ERR_PRINT("CMD_STAGE wrongly issued\n");
					break;
				}
				
				audio_mixer->voice_set_volume(fx[idx].voice, (Uint8)(fxc.params.volume*get_sfx_volume()*255.0));
				audio_mixer->voice_set_pan(fx[idx].voice, (Uint8)(fxc.params.pan*255.0),(Uint8)(fxc.params.depth*255.0));
				float sample_hz = sample_manager->get_c5_freq(fxc.sample_id);
				sample_hz*=fxc.params.rate_shift;
				audio_mixer->voice_set_speed(fx[idx].voice,(int)sample_hz);
				if (fxc.params.cutoff>=0.0) {
					
					audio_mixer->voice_set_filter(fx[idx].voice,AUDIO_FILTER_LOWPASS,(int)fxc.params.cutoff ,(Uint8)(fxc.params.resonance*255.0));
				}
				audio_mixer->voice_set_chorus(fx[idx].voice,(Uint8)(fxc.params.chorus*255.0));
				audio_mixer->voice_set_reverb(fx[idx].voice,(Uint8)(fxc.params.reverb*255.0));
				
				
			} break;
			case FxCommand::CMD_STOP: {
				
				if (audio_mixer->is_voice_active(fx[idx].voice))
					audio_mixer->voice_stop(fx[idx].voice);
				fx[idx].stage=STAGE_DONE;
				
			} break;			
		}
		
		cmd_rb.read_pos=(cmd_rb.read_pos+1)&FX_CMD_RINGBUFF_MASK;
		
	}
	
	
	audio_mixer->mix(p_to_buffer,p_len);
	audio_streams.call( p_to_buffer, p_len );
	
	/* apply gain & clip buffer */

	for (int i=0;i<p_len*2;i++) {
		
		// apply gain
		p_to_buffer[i]>>=8;
		p_to_buffer[i]*=gain;
		// clip
		if (p_to_buffer[i]>0x7FFFFF)
			p_to_buffer[i]=0x7FFFFF;
		else if (p_to_buffer[i]<-0x800000)
			p_to_buffer[i]=-0x800000;		
	}
}


void AudioServerSoftware::request_output_stream( Method2<Sint32*,int> p_method ) {

	audio_streams.connect( p_method );
}
void AudioServerSoftware::cancel_output_stream(Method2<Sint32*,int> p_method) {

	audio_streams.disconnect( p_method );

}
int AudioServerSoftware::get_stream_mix_rate() const {

	return mix_rate;
}

AudioSFXID AudioServerSoftware::play_fx(AudioSampleID p_sample,const FxParams* p_fx_params) {

	ERR_FAIL_COND_V( !sample_manager->check(p_sample),AUDIO_FX_INVALID_ID );

	if ( ((cmd_rb.write_pos+1)&FX_CMD_RINGBUFF_MASK)==cmd_rb.read_pos) {
		WARN_PRINT("AudioServerSoftware::play_fx: ringbuffer full.");
		
		return AUDIO_FX_INVALID_ID;
	}
	
	int index=-1;
	for (int i=0;i<FX_MAX;i++) {
		
		if (fx[i].stage==STAGE_OFF || fx[i].stage==STAGE_DONE || (fx[i].stage==STAGE_PLAY && !audio_mixer->is_voice_active(fx[i].voice))) {
			
			index=i;
			fx[i].stage=STAGE_INIT;
			fx[i].check=fx_id_count++;
			if (fx_id_count==0x7FFFFFFF)
				fx_id_count=1;
			break;	
		}
	}
	
	if (index==-1) {
		
		WARN_PRINT("AudioServerSoftware::play_fx: out of FX slots.");
		return AUDIO_FX_INVALID_ID;
	}
	
	FxCommand cmd;
	
	cmd.id=index+FX_MAX*fx[index].check;
	cmd.type=FxCommand::CMD_START;
	cmd.params=p_fx_params?*p_fx_params:FxParams();
	cmd.sample_id=p_sample;
	
	cmd_rb.data[cmd_rb.write_pos]=cmd;
	cmd_rb.write_pos=(cmd_rb.write_pos+1)&FX_CMD_RINGBUFF_MASK;
	
	return cmd.id;
}
void AudioServerSoftware::change_fx(AudioSFXID p_sample,const FxParams *p_fx_params) {

	ERR_FAIL_COND(p_sample<0);
	ERR_FAIL_COND(p_fx_params==NULL);
	
	if ( ((cmd_rb.write_pos+1)&FX_CMD_RINGBUFF_MASK)==cmd_rb.read_pos) {
		WARN_PRINT("AudioServerSoftware::play_fx: ringbuffer full.");
		
		return;
	}
	
	FxCommand cmd;
	
	cmd.id=p_sample;
	cmd.type=FxCommand::CMD_CHANGE;
	cmd.params=*p_fx_params;
	
	cmd_rb.data[cmd_rb.write_pos]=cmd;
	cmd_rb.write_pos=(cmd_rb.write_pos+1)&FX_CMD_RINGBUFF_MASK;
	
}
Error AudioServerSoftware::get_fx_params(AudioSFXID p_sample,FxParams *p_fx_params) {

	int idx=p_sample%FX_MAX;
	int check=p_sample/FX_MAX;
	
	ERR_FAIL_INDEX_V( idx, FX_MAX, ERR_INVALID_PARAMETER);
	
	ERR_FAIL_COND_V( fx[idx].check!=check, ERR_INVALID_PARAMETER );
	ERR_FAIL_COND_V( fx[idx].stage==STAGE_OFF, ERR_INVALID_PARAMETER );
	
	ERR_FAIL_COND_V(fx[idx].voice==AUDIO_VOICE_INVALID_ID, ERR_INVALID_PARAMETER );
	
	*p_fx_params=fx[idx].params;
	
	return OK;
}

bool AudioServerSoftware::is_fx_active(AudioSFXID p_sample) const {
	
	int idx=p_sample%FX_MAX;
	int check=p_sample/FX_MAX;
	ERR_FAIL_INDEX_V( idx, FX_MAX, false);
	if (fx[idx].check!=check) {
		printf("id %i(#%i): check mismatch %i/%i\n",p_sample,idx,fx[idx].check,check);
		return false;
	}
	if (fx[idx].stage==STAGE_OFF || fx[idx].stage==STAGE_DONE || (fx[idx].stage==STAGE_PLAY && !audio_mixer->is_voice_active(fx[idx].voice)))
		return false;
	
	return true;
}

void AudioServerSoftware::stop_fx(AudioSFXID p_sample) {

	if (!is_fx_active(p_sample))
		return;
		
	
	if ( ((cmd_rb.write_pos+1)&FX_CMD_RINGBUFF_MASK)==cmd_rb.read_pos) {
		WARN_PRINT("AudioServerSoftware::stop_fx: ringbuffer full.");
		
		return;
	}
	
	FxCommand cmd;
	
	cmd.id=p_sample;
	cmd.type=FxCommand::CMD_STOP;
	
	cmd_rb.data[cmd_rb.write_pos]=cmd;
	cmd_rb.write_pos=(cmd_rb.write_pos+1)&FX_CMD_RINGBUFF_MASK;
	
}


AudioServerSoftware::AudioServerSoftware(int p_sample_pool_size) {

	sample_manager = memnew( AudioSampleManagerSoftware(p_sample_pool_size) );
	audio_mixer = memnew( AudioMixerSoftware );
	mix_rate=44100;
	cmd_rb.read_pos=0;
	cmd_rb.write_pos=0;
	fx_id_count=1;
	gain=(Uint8)(GlobalVars::get_singleton()->get("audio/sw_gain").get_real()*255.0);

}


AudioServerSoftware::~AudioServerSoftware() {
	
	
	memdelete( audio_mixer );
	memdelete( sample_manager );
}


