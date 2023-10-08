//
// C++ Interface: audio_server_software
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef AUDIO_SERVER_SOFTWARE_H
#define AUDIO_SERVER_SOFTWARE_H

#include "audio/audio_server.h"
#include "audio/audio_mixer_software.h"

/**7
	@author ,,, <red@lunatea>
*/
class AudioServerSoftware : public AudioServer {

	AudioMixerSoftware *audio_mixer;
	AudioSampleManagerSoftware *sample_manager;
	Signal< Method2<Sint32*,int> > audio_streams;
	
	int mix_rate;
	
	
	enum {
	
		FX_CMD_RINGBUFF_BITS=6,
  		FX_CMD_RINGBUFF_LEN=(1<<FX_CMD_RINGBUFF_BITS),
		FX_CMD_RINGBUFF_MASK=FX_CMD_RINGBUFF_LEN-1,
				
		FX_MAX=16
	};
		
	struct FxCommand {
	
		enum Type {
			CMD_START,
   			CMD_CHANGE,
      			CMD_STOP
		};
		
		Type type;
		AudioSFXID id;
		AudioSampleID sample_id;
		FxParams params;	
		
		FxCommand() { type=CMD_START; id=AUDIO_FX_INVALID_ID; sample_id=AUDIO_SAMPLE_INVALID_ID; }
	};
	
	struct {
		
		FxCommand data[FX_CMD_RINGBUFF_LEN];
		int read_pos;
		int write_pos;
	} cmd_rb;
	
	enum SFXStage {
		STAGE_OFF,
		STAGE_INIT,
  		STAGE_PLAY,
    		STAGE_DONE		
	};
	struct Fx {
		
		SFXStage stage;
		AudioVoiceID voice;
		FxParams params;
		int check;
		
		Fx() { stage=STAGE_OFF;  voice=AUDIO_VOICE_INVALID_ID; }
		
	} fx[FX_MAX];
	
	int fx_id_count;
	
	Uint8 gain;
	
protected:

	/**
	 * Mix "p_len" audio frames to stereo interleaved buffer (p_to_buffer). 
	 * p_mix_rate must be supplied
	 */

	void mix(Sint32 *p_to_buffer,int p_len,int p_mix_rate);
	virtual AudioMixer *get_mixer();
	
public:
		
	/* STREAM API */
	
	virtual void request_output_stream( Method2<Sint32*,int> p_method );
	virtual void cancel_output_stream(Method2<Sint32*,int> p_method);
	virtual int get_stream_mix_rate() const;

	/* SIMPLE FX PLAYBACK API */
		
	virtual AudioSFXID play_fx(AudioSampleID p_sample,const FxParams* p_fx_params=NULL);
	virtual void change_fx(AudioSFXID p_sample,const FxParams *p_fx_params);
	virtual Error get_fx_params(AudioSFXID p_sample,FxParams *p_fx_params);
	virtual bool is_fx_active(AudioSFXID p_sample) const;
	virtual void stop_fx(AudioSFXID p_sample);

	AudioServerSoftware(int p_sample_pool_size );
	~AudioServerSoftware();

};

#endif
