//
// C++ Interface: audio_sample_manager
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef AUDIO_SAMPLE_MANAGER_H
#define AUDIO_SAMPLE_MANAGER_H

#include "typedefs.h"


typedef int AudioSampleID;

enum {
	
	AUDIO_SAMPLE_INVALID_ID=-1
};

enum AudioSampleLoopType {
	
	AUDIO_SAMPLE_LOOP_NONE,
	AUDIO_SAMPLE_LOOP_FORWARD,
	AUDIO_SAMPLE_LOOP_PING_PONG	
};

enum AudioSampleFormat {

	AUDIO_SAMPLE_FORMAT_PCM8,
	AUDIO_SAMPLE_FORMAT_PCM16,
};

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class AudioSampleManager{
	
	static AudioSampleManager * singleton;

public:
	/* get the singleton instance */
	static AudioSampleManager *get_singleton();

	virtual AudioSampleID create(AudioSampleFormat p_format,bool p_stereo,Sint32 p_len)=0;
	virtual void store_data(AudioSampleID p_id,const void *p_sample)=0;
	virtual void retrieve_data(AudioSampleID p_id,void *p_dest)=0;	
	virtual void destroy(AudioSampleID p_id)=0;
	virtual bool check(AudioSampleID p_id) const=0; // return false if invalid

	virtual void set_c5_freq(AudioSampleID p_id,Sint32 p_freq)=0;
	virtual void set_loop_begin(AudioSampleID p_id,Sint32 p_begin)=0;
	virtual void set_loop_end(AudioSampleID p_id,Sint32 p_end)=0;
	virtual void set_loop_type(AudioSampleID p_id,AudioSampleLoopType p_type)=0;

	
	virtual Sint32 get_loop_begin(AudioSampleID p_id) const=0;
	virtual Sint32 get_loop_end(AudioSampleID p_id) const=0;
	virtual AudioSampleLoopType get_loop_type(AudioSampleID p_id) const=0;
	virtual Sint32 get_c5_freq(AudioSampleID p_id) const=0;
	virtual Sint32 get_size(AudioSampleID p_id) const=0;
	virtual AudioSampleFormat get_format(AudioSampleID p_id) const=0;
	virtual bool is_stereo(AudioSampleID p_id) const=0;
	
		
	AudioSampleManager();
	virtual ~AudioSampleManager();
};

#endif
